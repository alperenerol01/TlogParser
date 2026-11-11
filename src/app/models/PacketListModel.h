#pragma once

#include "MavlinkDecoder.h"
#include "MavlinkPacket.h"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QVariant>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <vector>

/**
 * @brief Lists the packets of a log for the raw data view.
 *
 * Each row is one packet with three roles: the time since the start of
 * the log, the message name and the decoded fields. 
 */
class PacketListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("PacketListModel is created by LogSession.")

public:
    /**
     * @brief Data roles of a row.
     */
    enum Role {
        TimeRole = Qt::UserRole + 1,   ///< Time since the first packet, "mm:ss".
        NameRole,                      ///< Message name, e.g. "GLOBAL_POSITION_INT".
        FieldsRole,                    ///< Decoded fields, "name: value, ...".
    };

    /**
     * @brief Creates an empty model.
     * @param parent Parent object.
     */
    explicit PacketListModel(QObject* parent = nullptr);

    /**
     * @brief Returns the number of packets.
     * @param parent Unused for a list; must be invalid.
     * @return The number of rows.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Returns the text of one role of a row.
     * @param index Row of the packet.
     * @param role  One of the Role values.
     * @return The text, or an empty value for an invalid row or role.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns the role names used in QML.
     * @return "time", "name" and "fields".
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Replaces the listed packets.
     * @param packets The packets of the log, in file order.
     */
    void setPackets(std::vector<tlog::MavlinkPacket> packets);

private:
    std::vector<tlog::MavlinkPacket> m_packets;
    std::uint64_t m_startTimestampUs = 0;
    tlog::MavlinkDecoder m_decoder;
};
