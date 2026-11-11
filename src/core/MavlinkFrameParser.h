#pragma once

#include "MavlinkPacket.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace tlog {

/**
 * @brief Outcome of trying to parse a MAVLink frame at a given position.
 */
enum class FrameStatus {
    Ok,             ///< A complete frame with a valid checksum.
    InvalidMagic,   ///< The first byte is not a MAVLink start marker.
    BadChecksum,    ///< The frame is complete but its checksum does not match.
    BadSignature,   ///< The frame signature was rejected.
    Incomplete,     ///< The data ended before the frame was complete.
};

/**
 * @brief Result of MavlinkFrameParser::parse().
 */
struct FrameParseResult {
    FrameStatus status = FrameStatus::Incomplete;   ///< What the parser found.
    std::size_t frameSize = 0;                      ///< Number of bytes in the frame; set only when status is Ok.
    MavlinkPacket packet;                           ///< Parsed packet; set only when status is Ok. Its timestamp is left at 0.
};

/**
 * @brief Parses single MAVLink 1 and MAVLink 2 frames.
 *
 * Framing, checksum and signature handling are delegated to the official
 * MAVLink C library (common dialect). Signed frames are accepted without
 * verifying the signature because the signing key is not available.
 */
class MavlinkFrameParser {
public:
    /**
     * @brief Parses the frame that starts at @p frameStart in @p bytes.
     * @param bytes      Buffer that contains the candidate frame.
     * @param frameStart Index of the first byte of the candidate frame.
     * @return The parse status, the frame size and the parsed packet.
     */
    FrameParseResult parse(const std::vector<std::uint8_t>& bytes, std::size_t frameStart) const;
};

}
