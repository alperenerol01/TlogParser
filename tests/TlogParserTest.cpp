/**
 * @file TlogParserTest.cpp
 * @brief Tests for the requirements of the log viewer.
 *
 * Test cases:
 *  1. readsAllPacketsOfSampleLog      - Every MAVLink packet of the sample log is parsed.
 *  2. skipsDamagedData                - Garbage, a damaged record and a cut-off end are skipped.
 *  3. reportsFileThatCannotBeOpened   - Loading a missing file shows an error and stays on the load screen.
 *  4. loadsSampleLog                  - Loading a valid file switches to the viewer and fills the list.
 *  5. showsRowAsReadableText          - A row has a time, a message name and readable fields.
 *  6. showsTimeSinceStartOfLog        - Row times count from the first packet ("mm:ss").
 *  7. decodesLatitudeAndLongitude     - A GLOBAL_POSITION_INT packet gives the position in degrees.
 *  8. ignoresPositionWithoutGpsFix    - A 0, 0 position is not shown on the map.
 *  9. timelineCoversWholeLog          - The timeline spans the log and stays inside it.
 * 10. timelineSelectsSamePacketForBothViews - Moving the timeline selects one packet for the list and the map.
 */

#include "LogSession.h"
#include "MavlinkDecoder.h"
#include "PacketListModel.h"
#include "PlaybackController.h"
#include "TlogReader.h"

#include <QSignalSpy>
#include <QtTest>

#include <cstdint>
#include <fstream>
#include <optional>
#include <vector>

namespace {

/**
 * @brief Loads the sample log shipped with the project.
 * @return The raw bytes of example-drone.tlog.
 */
std::vector<std::uint8_t> loadSampleBytes()
{
    std::vector<std::uint8_t> bytes;
    std::ifstream file(SAMPLE_TLOG_PATH, std::ios::binary);
    char byte = 0;
    while (file.get(byte)) {
        bytes.push_back(static_cast<std::uint8_t>(byte));
    }
    return bytes;
}

/**
 * @brief Builds a packet with the given message id and payload.
 * @param messageId MAVLink message id.
 * @param payload   Payload bytes.
 * @return The packet.
 */
tlog::MavlinkPacket makePacket(std::uint32_t messageId, std::vector<std::uint8_t> payload)
{
    tlog::MavlinkPacket packet;
    packet.messageId = messageId;
    packet.payload = payload;
    return packet;
}

/**
 * @brief Loads a file into a session and waits until reading has finished.
 * @param session The session to load into.
 * @param filePath Path of the file.
 * @return True if reading finished within five seconds.
 */
bool loadAndWait(LogSession& session, const QString& filePath)
{
    QSignalSpy spy(&session, &LogSession::loadingChanged);
    session.load(QUrl::fromLocalFile(filePath));
    while (session.isLoading()) {
        if (!spy.wait(5000)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Returns the text of one role of a row in the packet list.
 * @param model The packet list.
 * @param row   Row number.
 * @param role  One of PacketListModel::Role.
 * @return The text shown in the list.
 */
QString rowText(PacketListModel& model, int row, int role)
{
    return model.data(model.index(row, 0), role).toString();
}

}

/**
 * @brief Tests for the requirements of the log viewer.
 */
class TlogParserTest : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief Loads the sample log once for the tests that need a loaded session.
     */
    void initTestCase()
    {
        QVERIFY(loadAndWait(m_session, SAMPLE_TLOG_PATH));
    }

    /**
     * @brief Every MAVLink packet of the sample log is parsed.
     */
    void readsAllPacketsOfSampleLog()
    {
        const tlog::TlogReadResult result = tlog::TlogReader().readFile(SAMPLE_TLOG_PATH);

        QCOMPARE(result.error, tlog::TlogReadError::None);
        QCOMPARE(result.packets.size(), std::size_t{86371});
        QCOMPARE(result.packets.front().messageId, std::uint32_t{141});
    }

    /**
     * @brief Garbage at the start, a damaged record and a cut-off end are skipped,
     *        and every other packet is still read.
     */
    void skipsDamagedData()
    {
        std::vector<std::uint8_t> bytes = loadSampleBytes();
        bytes[83] ^= 0xFF;
        bytes.resize(bytes.size() - 10);
        bytes.insert(bytes.begin(), {0x11, 0x22, 0xFD, 0x33, 0x44});

        const tlog::TlogReadResult result = tlog::TlogReader().parseBytes(bytes);

        QCOMPARE(result.error, tlog::TlogReadError::None);
        QCOMPARE(result.packets.size(), std::size_t{86371 - 2});
    }

    /**
     * @brief Loading a missing file shows an error and stays on the load screen.
     */
    void reportsFileThatCannotBeOpened()
    {
        LogSession session;

        QVERIFY(loadAndWait(session, "missing-file.tlog"));

        QVERIFY(!session.isLoaded());
        QCOMPARE(session.errorMessage(), QString("Dosya açılamadı."));
    }

    /**
     * @brief Loading a valid file switches to the viewer and fills the packet list.
     */
    void loadsSampleLog()
    {
        QVERIFY(m_session.isLoaded());
        QVERIFY(m_session.errorMessage().isEmpty());
        QCOMPARE(m_session.packetModel()->rowCount(), 86371);
        QVERIFY(!m_session.flightPath().isEmpty());
    }

    /**
     * @brief A row of the raw data list has a time, a message name and readable fields.
     */
    void showsRowAsReadableText()
    {
        PacketListModel& model = *m_session.packetModel();

        QCOMPARE(rowText(model, 0, PacketListModel::TimeRole), QString("00:00"));
        QCOMPARE(rowText(model, 0, PacketListModel::NameRole), QString("ALTITUDE"));
        QVERIFY(rowText(model, 0, PacketListModel::FieldsRole).startsWith("time_usec: "));
    }

    /**
     * @brief Row times count from the first packet in "mm:ss" form.
     */
    void showsTimeSinceStartOfLog()
    {
        PacketListModel& model = *m_session.packetModel();
        const int lastRow = model.rowCount() - 1;

        QCOMPARE(rowText(model, lastRow, PacketListModel::TimeRole), QString("04:23"));
    }

    /**
     * @brief A GLOBAL_POSITION_INT packet gives latitude and longitude in degrees.
     */
    void decodesLatitudeAndLongitude()
    {
        const tlog::MavlinkPacket packet = makePacket(33, {0x00, 0x00, 0x00, 0x00,
                                                           0x3E, 0x5B, 0x40, 0x1C,
                                                           0x86, 0x0A, 0x18, 0x05});

        const std::optional<tlog::GeoPosition> position = tlog::MavlinkDecoder().decodePosition(packet);

        QVERIFY(position.has_value());
        QCOMPARE(position->latitude, 47.3979710);
        QCOMPARE(position->longitude, 8.5461638);
    }

    /**
     * @brief A 0, 0 position, sent before the GPS has a fix, is not shown on the map.
     */
    void ignoresPositionWithoutGpsFix()
    {
        const tlog::MavlinkPacket packet = makePacket(33, {0x00, 0x00, 0x00, 0x00});

        QVERIFY(!tlog::MavlinkDecoder().decodePosition(packet).has_value());
    }

    /**
     * @brief The timeline spans the whole log and positions outside it are moved to its ends.
     */
    void timelineCoversWholeLog()
    {
        PlaybackController& playback = *m_session.playback();

        QCOMPARE(playback.durationMs(), qint64{263157});
        QCOMPARE(playback.durationText(), QString("04:23"));

        playback.setPositionMs(999999);
        QCOMPARE(playback.positionMs(), playback.durationMs());
        QCOMPARE(playback.currentIndex(), m_session.packetModel()->rowCount() - 1);

        playback.setPositionMs(-1);
        QCOMPARE(playback.positionMs(), qint64{0});
    }

    /**
     * @brief Moving the timeline selects one packet: the last packet logged up to
     *        that moment. The highlighted row of the list and the position on the
     *        map both belong to it.
     */
    void timelineSelectsSamePacketForBothViews()
    {
        const tlog::TlogReadResult log = tlog::TlogReader().readFile(SAMPLE_TLOG_PATH);
        const std::uint64_t targetUs = log.packets.front().timestampUs + 60000000;
        PlaybackController& playback = *m_session.playback();
        QSignalSpy spy(&playback, &PlaybackController::currentIndexChanged);

        playback.setPositionMs(60000);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(playback.positionText(), QString("01:00"));

        const std::size_t index = static_cast<std::size_t>(playback.currentIndex());
        QVERIFY(log.packets[index].timestampUs <= targetUs);
        QVERIFY(log.packets[index + 1].timestampUs > targetUs);

        const tlog::MavlinkDecoder decoder;
        std::optional<tlog::GeoPosition> lastPosition;
        for (std::size_t i = 0; i <= index; ++i) {
            const std::optional<tlog::GeoPosition> position = decoder.decodePosition(log.packets[i]);
            if (position.has_value()) {
                lastPosition = position;
            }
        }
        QVERIFY(lastPosition.has_value());
        QCOMPARE(playback.currentCoordinate().latitude(), lastPosition->latitude);
        QCOMPARE(playback.currentCoordinate().longitude(), lastPosition->longitude);
    }

private:
    LogSession m_session;
};

QTEST_GUILESS_MAIN(TlogParserTest)
#include "TlogParserTest.moc"
