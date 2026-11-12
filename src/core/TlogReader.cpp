#include "TlogReader.h"

#include <fstream>

namespace tlog {

namespace {

constexpr std::size_t kTimestampSize = 8;

/**
 * @brief Reads the big-endian timestamp at the start of a log record.
 * @param bytes       Buffer that contains the record.
 * @param recordStart Index of the first of the 8 timestamp bytes.
 * @return The timestamp in microseconds since the Unix epoch.
 */
std::uint64_t readTimestamp(const std::vector<std::uint8_t>& bytes, std::size_t recordStart)
{
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < kTimestampSize; ++i) {
        value = (value << 8) | bytes[recordStart + i];
    }
    return value;
}

/**
 * @brief Loads a whole file into memory.
 * @param filePath Path of the file to load.
 * @param bytes    Receives the file contents.
 * @return True if the file could be opened.
 */
bool loadFile(const std::filesystem::path& filePath, std::vector<std::uint8_t>& bytes)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false;
    }
    char byte = 0;
    while (file.get(byte)) {
        bytes.push_back(static_cast<std::uint8_t>(byte));
    }
    return true;
}

}

TlogReadResult TlogReader::readFile(const std::filesystem::path& filePath) const
{
    std::vector<std::uint8_t> bytes;
    if (!loadFile(filePath, bytes)) {
        TlogReadResult result;
        result.error = TlogReadError::CannotOpenFile;
        return result;
    }
    return parseBytes(bytes);
}

TlogReadResult TlogReader::parseBytes(const std::vector<std::uint8_t>& bytes) const
{
    TlogReadResult result;
    if (bytes.empty()) {
        result.error = TlogReadError::EmptyFile;
        return result;
    }

    std::size_t recordStart = 0;
    while (recordStart + kTimestampSize < bytes.size()) {
        FrameParseResult frame = m_frameParser.parse(bytes, recordStart + kTimestampSize);
        if (frame.status != FrameStatus::Ok) {
            ++recordStart;
            continue;
        }

        std::uint64_t timestamp = readTimestamp(bytes, recordStart);
        if (!result.packets.empty() && timestamp < result.packets.back().timestampUs) {
            timestamp = result.packets.back().timestampUs;
        }

        frame.packet.timestampUs = timestamp;
        result.packets.push_back(frame.packet);
        recordStart += kTimestampSize + frame.frameSize;
    }

    if (result.packets.empty()) {
        result.error = TlogReadError::NoPackets;
    }
    return result;
}

}
