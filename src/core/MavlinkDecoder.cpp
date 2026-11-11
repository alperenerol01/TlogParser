#include "MavlinkDecoder.h"

#define MAVLINK_USE_MESSAGE_INFO
#include <mavlink.h>

#include <cstring>
#include <sstream>
#include <vector>

namespace tlog {

namespace {

/**
 * @brief Copies the payload into a buffer of the maximum payload length.
 *
 * MAVLink 2 drops the trailing zero bytes of a payload, so the bytes that
 * were not received are zeros.
 * @param packet The packet whose payload is copied.
 * @return A buffer of MAVLINK_MAX_PAYLOAD_LEN bytes.
 */
std::vector<std::uint8_t> paddedPayload(const MavlinkPacket& packet)
{
    std::vector<std::uint8_t> payload(MAVLINK_MAX_PAYLOAD_LEN, 0);
    for (std::size_t i = 0; i < packet.payload.size() && i < payload.size(); ++i) {
        payload[i] = packet.payload[i];
    }
    return payload;
}

/**
 * @brief Reads a value of type T from the payload.
 *
 * MAVLink payloads are little-endian, the same byte order as x86 and ARM,
 * so the bytes can be copied into the value as they are.
 * @tparam T Type of the value.
 * @param payload Padded payload.
 * @param offset  Index of the first byte of the value.
 * @return The value, or zero if the value does not fit inside the payload.
 */
template <typename T>
T readValue(const std::vector<std::uint8_t>& payload, std::size_t offset)
{
    T value{};
    if (offset + sizeof(T) <= payload.size()) {
        std::memcpy(&value, &payload[offset], sizeof(T));
    }
    return value;
}

/**
 * @brief Returns the size of one value of a MAVLink field type.
 * @param type The field type.
 * @return Size in bytes.
 */
std::size_t typeSize(mavlink_message_type_t type)
{
    switch (type) {
    case MAVLINK_TYPE_CHAR:
        return 1;
    case MAVLINK_TYPE_UINT8_T:
        return 1;
    case MAVLINK_TYPE_INT8_T:
        return 1;
    case MAVLINK_TYPE_UINT16_T:
        return 2;
    case MAVLINK_TYPE_INT16_T:
        return 2;
    case MAVLINK_TYPE_UINT32_T:
        return 4;
    case MAVLINK_TYPE_INT32_T:
        return 4;
    case MAVLINK_TYPE_FLOAT:
        return 4;
    case MAVLINK_TYPE_UINT64_T:
        return 8;
    case MAVLINK_TYPE_INT64_T:
        return 8;
    case MAVLINK_TYPE_DOUBLE:
        return 8;
    }
    return 1;
}

/**
 * @brief Writes a single value to the output.
 * @param out     Output stream.
 * @param payload Padded payload.
 * @param offset  Index of the first byte of the value.
 * @param type    Type of the value.
 */
void writeValue(std::ostringstream& out, const std::vector<std::uint8_t>& payload,
                std::size_t offset, mavlink_message_type_t type  )
{
    switch (type) {
    case MAVLINK_TYPE_CHAR:
        out << readValue<char>(payload, offset);
        break;
    case MAVLINK_TYPE_UINT8_T:
        out << static_cast<unsigned>(readValue<std::uint8_t>(payload, offset));
        break;
    case MAVLINK_TYPE_INT8_T:
        out << static_cast<int>(readValue<std::int8_t>(payload, offset));
        break;
    case MAVLINK_TYPE_UINT16_T:
        out << readValue<std::uint16_t>(payload, offset);
        break;
    case MAVLINK_TYPE_INT16_T:
        out << readValue<std::int16_t>(payload, offset);
        break;
    case MAVLINK_TYPE_UINT32_T:
        out << readValue<std::uint32_t>(payload, offset);
        break;
    case MAVLINK_TYPE_INT32_T:
        out << readValue<std::int32_t>(payload, offset);
        break;
    case MAVLINK_TYPE_UINT64_T:
        out << readValue<std::uint64_t>(payload, offset);
        break;
    case MAVLINK_TYPE_INT64_T:
        out << readValue<std::int64_t>(payload, offset);
        break;
    case MAVLINK_TYPE_FLOAT:
        out << readValue<float>(payload, offset);
        break;
    case MAVLINK_TYPE_DOUBLE:
        out << readValue<double>(payload, offset);
        break;
    }
}

/**
 * @brief Writes a char array field as text, up to the first zero byte.
 * @param out     Output stream.
 * @param payload Padded payload.
 * @param offset  Index of the first character.
 * @param length  Size of the char array.
 */
void writeText(std::ostringstream& out, const std::vector<std::uint8_t>& payload,
               std::size_t offset, std::size_t length)
{
    for (std::size_t i = 0; i < length; ++i) {
        const char character = static_cast<char>(payload[offset + i]);
        if (character == '\0') {
            break;
        }
        out << character;
    }
}

/**
 * @brief Writes the value of one field: a single value, an array or text.
 * @param out     Output stream.
 * @param payload Padded payload.
 * @param field   Field description from the MAVLink message table.
 */
void writeField(std::ostringstream& out, const std::vector<std::uint8_t>& payload,
                const mavlink_field_info_t& field)
{
    if (field.array_length == 0) {
        writeValue(out, payload, field.wire_offset, field.type);
        return;
    }

    if (field.type == MAVLINK_TYPE_CHAR) {
        writeText(out, payload, field.wire_offset, field.array_length);
        return;
    }

    const std::size_t elementSize = typeSize(field.type);
    out << '[';
    for (std::size_t i = 0; i < field.array_length; ++i) {
        if (i > 0) {
            out << ", ";
        }
        writeValue(out, payload, field.wire_offset + i * elementSize, field.type);
    }
    out << ']';
}

}

std::string MavlinkDecoder::messageName(std::uint32_t messageId) const
{
    const mavlink_message_info_t* info = mavlink_get_message_info_by_id(messageId);
    if (info == nullptr) {
        return "UNKNOWN_" + std::to_string(messageId);
    }
    return info->name;
}

std::string MavlinkDecoder::formatFields(const MavlinkPacket& packet) const
{
    const mavlink_message_info_t* info = mavlink_get_message_info_by_id(packet.messageId);
    if (info == nullptr) {
        return "";
    }

    const std::vector<std::uint8_t> payload = paddedPayload(packet);
    std::ostringstream out;
    for (unsigned i = 0; i < info->num_fields; ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << info->fields[i].name << ": ";
        writeField(out, payload, info->fields[i]);
    }
    return out.str();
}

std::optional<GeoPosition> MavlinkDecoder::decodePosition(const MavlinkPacket& packet) const
{
    if (packet.messageId != MAVLINK_MSG_ID_GLOBAL_POSITION_INT) {
        return std::nullopt;
    }

    const std::vector<std::uint8_t> payload = paddedPayload(packet);
    mavlink_global_position_int_t message{};
    std::memcpy(&message, payload.data(), sizeof(message));

    if (message.lat == 0 && message.lon == 0) {
        return std::nullopt;
    }

    GeoPosition position;
    position.latitude = static_cast<double>(message.lat) / 10000000;
    position.longitude = static_cast<double>(message.lon) / 10000000;
    position.altitude = static_cast<double>(message.alt) / 1000;

    if (position.latitude < -90.0 || position.latitude > 90.0
        || position.longitude < -180.0 || position.longitude > 180.0) {
        return std::nullopt;
    }
    return position;
}

}
