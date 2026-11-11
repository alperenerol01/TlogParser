#pragma once

#include <cstdint>
#include <vector>

namespace tlog {

/**
 * @brief A single MAVLink message taken from a telemetry log.
 *
 * Holds the header fields that identify the message and its payload bytes.
 * The payload is stored exactly as it was received; MAVLink 2 may have
 * trimmed trailing zero bytes from it.
 */
struct MavlinkPacket {
    std::uint64_t timestampUs = 0;        ///< Time the ground station logged the packet (microseconds since Unix epoch).
    std::uint32_t messageId = 0;          ///< MAVLink message id, e.g. 33 for GLOBAL_POSITION_INT.
    std::uint32_t systemId = 0;           ///< Id of the sending system (vehicle or ground station).
    std::uint8_t componentId = 0;         ///< Id of the sending component inside that system.
    std::uint8_t sequence = 0;            ///< Per-sender packet counter, wraps after 255.
    std::uint8_t protocolVersion = 2;     ///< MAVLink protocol version of the frame (1 or 2).
    std::vector<std::uint8_t> payload;    ///< Payload bytes as received.
};

}
