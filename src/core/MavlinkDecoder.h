#pragma once

#include "MavlinkPacket.h"

#include <cstdint>
#include <optional>
#include <string>

namespace tlog {

/**
 * @brief A geographic position in decimal degrees.
 */
struct GeoPosition {
    double latitude = 0.0;    ///< Latitude in degrees, positive towards north.
    double longitude = 0.0;   ///< Longitude in degrees, positive towards east.
    double altitude = 0.0;    ///< Altitude above mean sea level in meters.
};

/**
 * @brief Turns the payload of MAVLink packets into readable text and positions.
 *
 * Uses the message definitions of the MAVLink common dialect.
 */
class MavlinkDecoder {
public:
    /**
     * @brief Returns the name of a message, e.g. "GLOBAL_POSITION_INT".
     * @param messageId MAVLink message id.
     * @return The message name, or "UNKNOWN_<id>" if the id is not defined.
     */
    std::string messageName(std::uint32_t messageId) const;

    /**
     * @brief Formats all fields of a packet as "name: value, name: value, ...".
     * @param packet The packet to format.
     * @return The formatted fields, or an empty string if the message is not defined.
     */
    std::string formatFields(const MavlinkPacket& packet) const;

    /**
     * @brief Extracts the vehicle position from a GLOBAL_POSITION_INT packet.
     * @param packet The packet to decode.
     * @return The position, or no value if the packet is another message
     *         or does not hold a valid position.
     */
    std::optional<GeoPosition> decodePosition(const MavlinkPacket& packet) const;
};

}
