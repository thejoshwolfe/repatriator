#include "IncomingMessage.h"

#include <QDataStream>

const qint8 MagicalResponseMessage::c_magical_response[] = { 0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07 };

QString IncomingMessage::readString(QDataStream &stream)
{
    qint32 string_byte_count;
    stream >> string_byte_count;
    QByteArray utf8_data = stream.device()->read(string_byte_count);
    return QString::fromUtf8(utf8_data.constData(), string_byte_count);
}

QImage IncomingMessage::readImage(QDataStream &stream)
{
    qint64 image_byte_count;
    stream >> image_byte_count;
    return QImage::fromData(stream.device()->read(image_byte_count));
}

void MagicalResponseMessage::parse(QDataStream &stream)
{
    is_valid = true;
    qint8 c;
    for (int i = 0; i < 8; i++) {
        stream >> c;
        if (c != c_magical_response[i]) {
            is_valid = false;
            // don't break, because we need to read 8 bytes from the stream
            // regardless of valid message
        }
    }
}

void ConnectionResultMessage::parse(QDataStream &stream)
{
    stream >> major_version;
    stream >> minor_version;
    stream >> revision_version;

    qint32 _connection_status;
    stream >> _connection_status;
    connection_status = (ConnectionResultStatus) _connection_status;

    qint32 permission_count;
    stream >> permission_count;
    for (qint32 i = 0; i < permission_count; i++) {
        qint32 permission;
        stream >> permission;
        permissions.insert((Server::Permission) permission);
    }
}

void FullUpdateMessage::parse(QDataStream &stream)
{
    for (int i = 1; i <= 5; i++) {
        qint64 pos;
        stream >> pos;
        motor_positions.append((long) pos);
    }
    image = readImage(stream);
}

void DirectoryListingResultMessage::parse(QDataStream &stream)
{
    qint32 file_count;
    stream >> file_count;
    for (qint32 i = 0; i < file_count; i++) {
        Server::DirectoryItem directory_item;
        directory_item.filename = readString(stream);
        directory_item.thumbnail = readImage(stream);
        directory_list.append(directory_item);
    }
}

void FileDownloadResultMessage::parse(QDataStream &stream)
{
    filename = readString(stream);
    qint64 file_byte_count;
    stream >> file_byte_count;
    file = stream.device()->read(file_byte_count);
}

void ErrorMessage::parse(QDataStream &stream)
{
    stream >> number;
    message = readString(stream);
}

void ListUserResultMessage::parse(QDataStream &stream)
{
    qint32 user_count;
    stream >> user_count;
    for (qint32 i = 0; i < user_count; i++) {
        Server::UserInfo user_info;
        user_info.username = readString(stream);
        qint32 permission_count;
        stream >> permission_count;
        for (qint32 j = 0; j < permission_count; j++) {
            qint32 permission;
            stream >> permission;
            user_info.permissions.insert((Server::Permission) permission);
        }
        users.append(user_info);
    }
}

