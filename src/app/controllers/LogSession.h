#pragma once

#include "MavlinkPacket.h"
#include "PacketListModel.h"
#include "PlaybackController.h"
#include "TlogReader.h"

#include <QFutureWatcher>
#include <QGeoCoordinate>
#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <filesystem>
#include <vector>

/**
 * @brief Loads a telemetry log and keeps its packets for the views.
 *
 * The load screen calls load(); when the file holds valid packets,
 * loaded becomes true and the main screen is shown. Otherwise
 * errorMessage explains what went wrong.
 */
class LogSession : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(PacketListModel* packetModel READ packetModel CONSTANT)
    Q_PROPERTY(PlaybackController* playback READ playback CONSTANT)
    Q_PROPERTY(QList<QGeoCoordinate> flightPath READ flightPath NOTIFY loadedChanged)

public:
    /**
     * @brief Creates an empty session.
     * @param parent Parent object.
     */
    explicit LogSession(QObject* parent = nullptr);

    /**
     * @brief Tells whether a log has been loaded.
     * @return True after a successful load().
     */
    bool isLoaded() const;

    /**
     * @brief Tells whether a file is being read in the background.
     * @return True from load() until the result has been taken over.
     */
    bool isLoading() const;

    /**
     * @brief Returns the message of the last failed load.
     * @return The message, or an empty string if there is no error.
     */
    QString errorMessage() const;

    /**
     * @brief Returns the packet list shown in the raw data view.
     * @return The model owned by this session.
     */
    PacketListModel* packetModel();

    /**
     * @brief Returns the timeline shared by the map and the raw data view.
     * @return The controller owned by this session.
     */
    PlaybackController* playback();

    /**
     * @brief Returns every position of the vehicle in the order they were logged.
     * @return The route drawn on the map; empty before a log is loaded.
     */
    QList<QGeoCoordinate> flightPath() const;

    /**
     * @brief Starts reading a .tlog file in the background.
     *
     * Returns at once; loading stays true until the file has been read.
     * Calls made while a file is being read are ignored.
     * @param fileUrl Location of the file, as given by the file dialog.
     */
    Q_INVOKABLE void load(const QUrl& fileUrl);
signals:
    /**
     * @brief Emitted when loaded changes.
     */
    void loadedChanged();

    /**
     * @brief Emitted when loading changes.
     */
    void loadingChanged();

    /**
     * @brief Emitted when errorMessage changes.
     */
    void errorMessageChanged();

private:
    /**
     * @brief Everything read from a file, handed from the background thread to the session.
     */
    struct LoadedLog {
        tlog::TlogReadError error = tlog::TlogReadError::None;   ///< Why reading failed, or None.
        std::vector<tlog::MavlinkPacket> packets;                ///< All valid packets in file order.
        std::vector<std::uint64_t> timestampsUs;                 ///< Timestamp of every packet.
        std::vector<QGeoCoordinate> coordinates;                 ///< Vehicle position at every packet.
        QList<QGeoCoordinate> flightPath;                        ///< Every received position, for the map.
    };

    /**
     * @brief Reads a file and decodes the vehicle positions; runs on a background thread.
     *
     * Static so that it cannot touch the members of the session while
     * the main thread is using them.
     * @param filePath The file to read.
     * @return The packets and the timeline data, or the error.
     */
    static LoadedLog readLog(const std::filesystem::path& filePath);

    /**
     * @brief Takes over the result of readLog(); runs on the main thread.
     */
    void onLoadFinished();

    /**
     * @brief Changes errorMessage and notifies the views.
     * @param message The new message.
     */
    void setErrorMessage(const QString& message);

    PacketListModel m_packetModel;
    PlaybackController m_playback;
    QList<QGeoCoordinate> m_flightPath;
    bool m_loaded = false;
    bool m_loading = false;
    QString m_errorMessage;
    QFutureWatcher<LoadedLog> m_watcher;
};
