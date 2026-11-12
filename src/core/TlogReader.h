#pragma once

#include "MavlinkFrameParser.h"
#include "MavlinkPacket.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace tlog {

/**
 * @brief Reasons why a telemetry log could not be read.
 */
enum class TlogReadError {
    None,             ///< The log was read successfully.
    CannotOpenFile,   ///< The file does not exist or cannot be opened.
    EmptyFile,        ///< The file contains no data.
    NoPackets,        ///< The file contains no valid MAVLink packet.
};

/**
 * @brief Result of TlogReader::readFile() and TlogReader::parseBytes().
 */
struct TlogReadResult {
    TlogReadError error = TlogReadError::None;   ///< TlogReadError::None on success.
    std::vector<MavlinkPacket> packets;          ///< Valid packets in file order.
};

/**
 * @brief Reads MAVLink packets from a telemetry log (.tlog) file.
 *
 * A .tlog file is a sequence of records, each made of an 8-byte big-endian
 * timestamp (microseconds since the Unix epoch) followed by one MAVLink frame.
 * Damaged or unknown data is skipped one byte at a time until the next valid
 * record is found.
 */
class TlogReader {
public:
    /**
     * @brief Reads all packets from a .tlog file.
     * @param filePath Path of the file to read.
     * @return The packets found and the error state.
     */
    TlogReadResult readFile(const std::filesystem::path& filePath) const;

    /**
     * @brief Parses all packets from the contents of a .tlog file.
     * @param bytes The complete file contents.
     * @return The packets found and the error state.
     */
    TlogReadResult parseBytes(const std::vector<std::uint8_t>& bytes) const;

private:
    MavlinkFrameParser m_frameParser;
};

}
