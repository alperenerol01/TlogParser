#include "LogSession.h"

#include "MavlinkDecoder.h"
#include "TlogReader.h"

#include <QGeoCoordinate>
#include <QtConcurrent/QtConcurrentRun>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <utility>
#include <vector>

namespace {

/**
 * @brief Returns the message shown on the load screen for a read error.
 * @param error The error returned by TlogReader.
 * @return The message in Turkish.
 */
QString errorText(tlog::TlogReadError error)
{
    switch (error) {
    case tlog::TlogReadError::None:
        return QString();
    case tlog::TlogReadError::CannotOpenFile:
        return QStringLiteral("Dosya açılamadı.");
    case tlog::TlogReadError::EmptyFile:
        return QStringLiteral("Dosya boş.");
    case tlog::TlogReadError::NoPackets:
        return QStringLiteral("Dosyada geçerli bir MAVLink paketi bulunamadı.");
    }
    return QString();
}

}

LogSession::LogSession(QObject* parent)
    : QObject(parent)
{
    connect(&m_watcher, &QFutureWatcher<LoadedLog>::finished, this, &LogSession::onLoadFinished);
}

bool LogSession::isLoaded() const
{
    return m_loaded;
}

bool LogSession::isLoading() const
{
    return m_loading;
}

QString LogSession::errorMessage() const
{
    return m_errorMessage;
}

PacketListModel* LogSession::packetModel()
{
    return &m_packetModel;
}

PlaybackController* LogSession::playback()
{
    return &m_playback;
}

QList<QGeoCoordinate> LogSession::flightPath() const
{
    return m_flightPath;
}

void LogSession::load(const QUrl& fileUrl)
{
    if (m_loading) {
        return;
    }

    const std::filesystem::path filePath(fileUrl.toLocalFile().toStdU16String());

    setErrorMessage(QString());
    m_loading = true;
    emit loadingChanged();

    m_watcher.setFuture(QtConcurrent::run(&LogSession::readLog, filePath));
}

void LogSession::onLoadFinished()
{
    LoadedLog log = m_watcher.future().takeResult();

    if (log.error != tlog::TlogReadError::None) {
        setErrorMessage(errorText(log.error));
    } else {
        m_packetModel.setPackets(std::move(log.packets));
        m_playback.setTimeline(std::move(log.timestampsUs), std::move(log.coordinates));
        m_flightPath = std::move(log.flightPath);
        m_loaded = true;
        emit loadedChanged();
    }

    m_loading = false;
    emit loadingChanged();
}

LogSession::LoadedLog LogSession::readLog(const std::filesystem::path& filePath)
{
    LoadedLog log;

    tlog::TlogReader reader;
    tlog::TlogReadResult result = reader.readFile(filePath);
    log.error = result.error;
    if (result.error != tlog::TlogReadError::None) {
        return log;
    }

    const tlog::MavlinkDecoder decoder;
    QGeoCoordinate lastCoordinate;
    for (const tlog::MavlinkPacket& packet : result.packets) {
        const std::optional<tlog::GeoPosition> position = decoder.decodePosition(packet);
        if (position.has_value()) {
            lastCoordinate = QGeoCoordinate(position->latitude, position->longitude,
                                            position->altitude);
            log.flightPath.append(lastCoordinate);
        }
        log.timestampsUs.push_back(packet.timestampUs);
        log.coordinates.push_back(lastCoordinate);
    }

    log.packets = std::move(result.packets);
    return log;
}

void LogSession::setErrorMessage(const QString& message)
{
    if (m_errorMessage == message) {
        return;
    }
    m_errorMessage = message;
    emit errorMessageChanged();
}
