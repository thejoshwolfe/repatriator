#include "OutgoingMessage.h"

#include <QDebug>

const qint8 MagicalRequestMessage::c_magical_data[] = { 0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4 };

void OutgoingMessage::writeToStream(QDataStream &stream)
{
    // write body to a buffer so we can measure byte counts
    QByteArray message_body;
    QDataStream body_stream(&message_body, QIODevice::WriteOnly);
    writeMessageBody(body_stream);

    // write header
    const qint64 header_size = 9;
    qint8 _type = type();
    stream << _type;
    qint64 byte_count = message_body.size() + header_size;
    stream << byte_count;

    // write body
    stream.device()->write(message_body);
}

void OutgoingMessage::writeString(QDataStream &stream, QString string)
{
    QByteArray utf8_data = string.toUtf8();
    qint32 byte_count = utf8_data.size();
    stream << byte_count;
    stream.device()->write(utf8_data);
}

void MagicalRequestMessage::writeMessageBody(QDataStream &stream)
{
    for (int i = 0; i < 8; i++)
        stream << c_magical_data[i];
}

void ConnectionRequestMessage::writeMessageBody(QDataStream &stream)
{
    stream << (qint32) newest_protocol_supported;
    writeString(stream, username);
    writeString(stream, password);
    stream << (qint8) (hardware_access ? 1 : 0);
}

void FileDownloadRequestMessage::writeMessageBody(QDataStream &stream)
{
    writeString(stream, filename);
}

void UpdateUserMessage::writeMessageBody(QDataStream &stream)
{
    writeString(stream, username);
    writeString(stream, password);
    stream << (qint32) permissions.count();
    foreach (ServerTypes::Permission permission, permissions)
        stream << (qint32) permission;
}

void DeleteUserMessage::writeMessageBody(QDataStream &stream)
{
    writeString(stream, username);
}

void FileDeleteRequestMessage::writeMessageBody(QDataStream &stream)
{
    writeString(stream, filename);
}

void ChangePasswordRequestMessage::writeMessageBody(QDataStream &stream)
{
    qDebug() << "writing change password message";
    writeString(stream, old_password);
    writeString(stream, new_password);
}

void SetAutoFocusEnabledMessage::writeMessageBody(QDataStream &stream)
{
    stream << this->value;
}

void MotorMovementMessage::writeMessageBody(QDataStream &stream)
{
    for (int i = 0; i < positions.count(); i++)
        stream << (qint64) positions.at(i);
}

void PingMessage::writeMessageBody(QDataStream &stream)
{
    stream << (qint32) ping_id;
}

void SetStaticBookmarksMessage::writeMessageBody(QDataStream &stream)
{
    stream << (qint32)bookmarks.size();
    foreach (ServerTypes::Bookmark bookmark, bookmarks)
    {
        writeString(stream, bookmark.name);
        foreach (qint64 motor_position, bookmark.motor_positions)
            stream << motor_position;
        stream << (qint8)bookmark.auto_focus;
    }
}

void ChangeFocusLocationMessage::writeMessageBody(QDataStream &stream)
{
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << (float) pt.x();
    stream << (float) pt.y();
}

void ExposureCompensationMessage::writeMessageBody(QDataStream &stream)
{
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << (float) value;
}

void SetMotorBoundsMessage::writeMessageBody(QDataStream &stream)
{
    foreach (ServerTypes::MotorBoundaries bound, bounds) {
        stream << bound.min;
        stream << bound.max;
    }
}
