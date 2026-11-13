#include "PlaybackController.h"

#include "TimeFormat.h"

#include <utility>

PlaybackController::PlaybackController(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(50);
    connect(&m_timer, &QTimer::timeout, this, &PlaybackController::advance);
}

qint64 PlaybackController::positionMs() const
{
    return m_positionMs;
}

void PlaybackController::setPositionMs(qint64 positionMs)
{
    if (positionMs < 0) {
        positionMs = 0;
    }
    if (positionMs > m_durationMs) {
        positionMs = m_durationMs;
    }

    if (positionMs != m_positionMs) {
        m_positionMs = positionMs;
        emit positionChanged();
    }

    const int index = findPacketIndex(positionMs);
    if (index != m_currentIndex) {
        m_currentIndex = index;
        emit currentIndexChanged();
    }
}

qint64 PlaybackController::durationMs() const
{
    return m_durationMs;
}

QString PlaybackController::positionText() const
{
    return formatTime(m_positionMs);
}

QString PlaybackController::durationText() const
{
    return formatTime(m_durationMs);
}

int PlaybackController::currentIndex() const
{
    return m_currentIndex;
}

QGeoCoordinate PlaybackController::currentCoordinate() const
{
    if (m_currentIndex < 0 || static_cast<std::size_t>(m_currentIndex) >= m_coordinates.size()) {
        return QGeoCoordinate();
    }
    return m_coordinates[static_cast<std::size_t>(m_currentIndex)];
}

void PlaybackController::setTimeline(std::vector<std::uint64_t> timestampsUs,
                                     std::vector<QGeoCoordinate> coordinates)
{
    pause();
    m_timestampsUs = std::move(timestampsUs);
    m_coordinates = std::move(coordinates);

    m_durationMs = 0;
    if (!m_timestampsUs.empty()) {
        const std::uint64_t durationUs = m_timestampsUs.back() - m_timestampsUs.front();
        m_durationMs = static_cast<qint64>(durationUs / 1000);
        if (durationUs % 1000 != 0) {
            ++m_durationMs;
        }
    }
    emit durationChanged();

    m_currentIndex = -1;
    moveToStart();
}

bool PlaybackController::isPlaying() const
{
    return m_playing;
}



void PlaybackController::play()
{
    if (m_playing || m_durationMs == 0) {
        return;
    }
    if (m_positionMs >= m_durationMs) {
        moveToStart();
    }
    m_playing = true;
    m_timer.start();
    emit playingChanged();
}

void PlaybackController::pause()
{
    if (!m_playing) {
        return;
    }
    m_playing = false;
    m_timer.stop();
    emit playingChanged();
}

void PlaybackController::selectPacket(int index)
{
    if (index < 0 || static_cast<std::size_t>(index) >= m_timestampsUs.size()) {
        return;
    }
    pause();

    const std::uint64_t elapsedUs = m_timestampsUs[static_cast<std::size_t>(index)]
                                  - m_timestampsUs.front();
    const qint64 positionMs = static_cast<qint64>(elapsedUs / 1000);
    if (positionMs != m_positionMs) {
        m_positionMs = positionMs;
        emit positionChanged();
    }

    if (index != m_currentIndex) {
        m_currentIndex = index;
        emit currentIndexChanged();
    }
}

void PlaybackController::setSpeed(int newSpeed)
{

        if (newSpeed < 1 || newSpeed == m_speed) {
            return;
        }
        m_speed = newSpeed;
        emit speedChanged();

}

void PlaybackController::moveToStart()
{
    if (m_positionMs != 0) {
        m_positionMs = 0;
        emit positionChanged();
    }

    int index = -1;
    if (!m_timestampsUs.empty()) {
        index = 0;
    }
    if (index != m_currentIndex) {
        m_currentIndex = index;
        emit currentIndexChanged();
    }
}

void PlaybackController::advance()
{
    qint64 stepMs = 50;

    setPositionMs(m_positionMs + stepMs * m_speed);
    if (m_positionMs >= m_durationMs) {
        pause();
    }
}

int PlaybackController::findPacketIndex(qint64 positionMs) const
{
    if (m_timestampsUs.empty()) {
        return -1;
    }

    const std::uint64_t targetUs = m_timestampsUs.front()
                                 + static_cast<std::uint64_t>(positionMs) * 1000;

    std::size_t low = 0;
    std::size_t high = m_timestampsUs.size();
    while (low < high) {
        const std::size_t middle = low + (high - low) / 2;
        if (m_timestampsUs[middle] <= targetUs) {
            low = middle + 1;
        } else {
            high = middle;
        }
    }
    return static_cast<int>(low) - 1;
}



int PlaybackController::speed() const
{
    return m_speed;
}
