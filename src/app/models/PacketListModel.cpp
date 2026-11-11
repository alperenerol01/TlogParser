#include "PacketListModel.h"

#include "TimeFormat.h"

#include <utility>

PacketListModel::PacketListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int PacketListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_packets.size());
}

QVariant PacketListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return QVariant();
    }

    const tlog::MavlinkPacket& packet = m_packets[static_cast<std::size_t>(index.row())];
    const qint64 elapsedMs = static_cast<qint64>((packet.timestampUs - m_startTimestampUs) / 1000);

    switch (role) {
    case TimeRole:
        return formatTime(elapsedMs);
    case NameRole:
        return QString::fromStdString(m_decoder.messageName(packet.messageId));
    case FieldsRole:
        return QString::fromStdString(m_decoder.formatFields(packet));
    }
    return QVariant();
}

QHash<int, QByteArray> PacketListModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[TimeRole] = "time";
    names[NameRole] = "name";
    names[FieldsRole] = "fields";
    return names;
}

void PacketListModel::setPackets(std::vector<tlog::MavlinkPacket> packets)
{
    beginResetModel();
    m_packets = std::move(packets);
    m_startTimestampUs = 0;
    if (!m_packets.empty()) {
        m_startTimestampUs = m_packets.front().timestampUs;
    }
    endResetModel();
}
