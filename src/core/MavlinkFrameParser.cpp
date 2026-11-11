#include "MavlinkFrameParser.h"

#include <mavlink.h>

namespace tlog {

namespace {

/**
 * @brief Checks whether a byte can start a MAVLink frame.
 * @param value The byte to check.
 * @return True for the MAVLink 2 (0xFD) or MAVLink 1 (0xFE) start marker.
 */
bool isStartMarker(std::uint8_t value)
{
    return value == MAVLINK_STX || value == MAVLINK_STX_MAVLINK1;
}

/**
 * @brief Converts a MAVLink library framing result to a FrameStatus.
 * @param framingResult One of the MAVLINK_FRAMING_* values.
 * @return The matching FrameStatus.
 */
FrameStatus toFrameStatus(std::uint8_t framingResult)
{
    switch (framingResult) {
    case MAVLINK_FRAMING_OK:
        return FrameStatus::Ok;
    case MAVLINK_FRAMING_BAD_CRC:
        return FrameStatus::BadChecksum;
    case MAVLINK_FRAMING_BAD_SIGNATURE:
        return FrameStatus::BadSignature;
    default:
        return FrameStatus::Incomplete;
    }
}

/**
 * @brief Copies the fields we need from a library message into a MavlinkPacket.
 * @param message A message returned by the MAVLink parser.
 * @return The packet with header fields and the received payload bytes.
 */
MavlinkPacket toPacket(const mavlink_message_t& message)
{
    const auto* payload = reinterpret_cast<const std::uint8_t*>(message.payload64);

    MavlinkPacket packet;
    packet.messageId = message.msgid;
    packet.systemId = message.sysid;
    packet.componentId = message.compid;
    packet.sequence = message.seq;
    packet.protocolVersion = (message.magic == MAVLINK_STX) ? 2 : 1;
    for (std::size_t i = 0; i < message.len; ++i) {
        packet.payload.push_back(payload[i]);
    }
    return packet;
}

}

FrameParseResult MavlinkFrameParser::parse(const std::vector<std::uint8_t>& bytes,
                                           std::size_t frameStart) const
{
    FrameParseResult result;
    if (frameStart >= bytes.size()) {
        return result;
    }
    if (!isStartMarker(bytes[frameStart])) {
        result.status = FrameStatus::InvalidMagic;
        return result;
    }

    mavlink_message_t parserBuffer{};
    mavlink_status_t parserStatus{};
    mavlink_message_t message{};
    mavlink_status_t messageStatus{};

    std::uint8_t framingResult = MAVLINK_FRAMING_INCOMPLETE;
    std::size_t index = frameStart;
    while (framingResult == MAVLINK_FRAMING_INCOMPLETE && index < bytes.size()) {
        framingResult = mavlink_frame_char_buffer(&parserBuffer, &parserStatus, bytes[index],
                                                  &message, &messageStatus);
        ++index;
    }

    result.status = toFrameStatus(framingResult);
    if (result.status == FrameStatus::Ok) {
        result.frameSize = index - frameStart;
        result.packet = toPacket(message);
    }
    return result;
}

}
