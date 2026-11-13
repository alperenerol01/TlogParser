#pragma once

#include <QGeoCoordinate>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <vector>

/**
 * @brief Keeps the current position of the timeline and the active packet.
 *
 * The timeline writes positionMs; the active packet and the vehicle
 * position at that moment are derived from it. Both the map and the raw
 * data view read their state from this object, so they always show the
 * same packet.
 */
class PlaybackController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("PlaybackController is created by LogSession.")
    Q_PROPERTY(qint64 positionMs READ positionMs WRITE setPositionMs NOTIFY positionChanged)
    Q_PROPERTY(qint64 durationMs READ durationMs NOTIFY durationChanged)
    Q_PROPERTY(QString positionText READ positionText NOTIFY positionChanged)
    Q_PROPERTY(QString durationText READ durationText NOTIFY durationChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QGeoCoordinate currentCoordinate READ currentCoordinate NOTIFY currentIndexChanged)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)

public:
    /**
     * @brief Creates a controller with an empty timeline.
     * @param parent Parent object.
     */
    explicit PlaybackController(QObject* parent = nullptr);

    /**
     * @brief Returns the timeline position.
     * @return Milliseconds since the first packet.
     */
    qint64 positionMs() const;

    /**
     * @brief Moves the timeline and selects the packet at that moment.
     * @param positionMs Milliseconds since the first packet; values outside
     *                   the timeline are moved to its nearest end.
     */
    void setPositionMs(qint64 positionMs);

    /**
     * @brief Returns the length of the timeline.
     * @return Milliseconds from the first to the last packet.
     */
    qint64 durationMs() const;

    /**
     * @brief Returns the timeline position as text.
     * @return The position in "mm:ss" form.
     */
    QString positionText() const;

    /**
     * @brief Returns the length of the timeline as text.
     * @return The length in "mm:ss" form.
     */
    QString durationText() const;

    /**
     * @brief Returns the row of the active packet.
     * @return The index of the last packet at or before the position,
     *         or -1 if the timeline is empty.
     */
    int currentIndex() const;

    /**
     * @brief Returns the vehicle position at the active packet.
     * @return The last position received up to the active packet, or an
     *         invalid coordinate if no position has been received yet.
     */
    QGeoCoordinate currentCoordinate() const;

    /**
     * @brief Replaces the timeline and moves it to the start.
     * @param timestampsUs Timestamp of every packet in microseconds, in file order.
     * @param coordinates  Vehicle position at every packet; same length as timestampsUs.
     */
    void setTimeline(std::vector<std::uint64_t> timestampsUs,
                     std::vector<QGeoCoordinate> coordinates);

    /**
     * @brief Tells whether the timeline is moving forward on its own.
     * @return True between play() and pause() or the end of the timeline.
     */
    bool isPlaying() const;

    /**
     * @brief Starts moving the timeline forward every 50 ms.
     *
     * If the timeline is already at its end, playback starts again from the beginning.
     */
    Q_INVOKABLE void play();

    /**
     * @brief Stops moving the timeline; the position is kept.
     */
    Q_INVOKABLE void pause();

    /**
     * @brief Makes a packet the active one and moves the timeline to its time.
     *
     * Unlike setPositionMs(), the given packet is selected even when other
     * packets share the same millisecond. Playback is paused.
     * @param index Row of the packet; values outside the timeline are ignored.
     */
    Q_INVOKABLE void selectPacket(int index);
    void setSpeed(int newSpeed);

    int speed() const;

signals:
    /**
     * @brief Emitted when positionMs changes.
     */
    void positionChanged();

    /**
     * @brief Emitted when durationMs changes.
     */
    void durationChanged();

    /**
     * @brief Emitted when the active packet changes.
     */
    void currentIndexChanged();

    /**
     * @brief Emitted when playing changes.
     */
    void playingChanged();

    void speedChanged();

private:
    /**
     * @brief Moves the timeline one step forward; called by the timer.
     */
    void advance();

    /**
     * @brief Moves the timeline to 0 and makes the first packet active, even when
     *        other packets share the first timestamp.
     */
    void moveToStart();

    /**
     * @brief Finds the last packet whose timestamp is at or before a position.
     * @param positionMs Milliseconds since the first packet.
     * @return The packet index, or -1 if the timeline is empty.
     */
    int findPacketIndex(qint64 positionMs) const;

    std::vector<std::uint64_t> m_timestampsUs;
    std::vector<QGeoCoordinate> m_coordinates;
    qint64 m_positionMs = 0;
    qint64 m_durationMs = 0;
    int m_currentIndex = -1;
    QTimer m_timer;
    bool m_playing = false;
    int m_speed = 1;
};
