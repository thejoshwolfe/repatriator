#include "IncomingMessage.h"

#include <QDataStream>
#include <QDebug>

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
        permissions.insert((ServerTypes::Permission) permission);
    }
}

void FullUpdateMessage::parse(QDataStream &stream)
{
    for (int i = 1; i <= 5; i++) {
        qint8 state;
        stream >> state;
        motor_states.append(state);
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
        ServerTypes::DirectoryItem directory_item;
        stream >> directory_item.byte_count;
        directory_item.filename = readString(stream);
        directory_item.thumbnail = readImage(stream);
        if (directory_item.byte_count > 0 && directory_item.filename.size() > 0 && ! directory_item.thumbnail.isNull())
            directory_list.append(directory_item);
        else
            qDebug() << "Skipping a directory list item, it's missing information.";
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
        ServerTypes::UserInfo user_info;
        user_info.username = readString(stream);
        qint32 permission_count;
        stream >> permission_count;
        for (qint32 j = 0; j < permission_count; j++) {
            qint32 permission;
            stream >> permission;
            user_info.permissions.insert((ServerTypes::Permission) permission);
        }
        users.append(user_info);
    }
}

void InitInfoMessage::parse(QDataStream & stream)
{
    for (int i = 0; i < 5; i++) {
        MotorBoundaries bounds;
        stream >> bounds.min;
        stream >> bounds.max;
        motor_boundaries.append(bounds);
    }
    readBookmarkList(stream, static_bookmarks);
    readBookmarkList(stream, user_bookmarks);
}
void InitInfoMessage::readBookmarkList(QDataStream & stream, QVector<ServerTypes::Bookmark> & bookmark_list)
{
    qint32 bookmark_list_len;
    stream >> bookmark_list_len;
    for (int i = 0; i < bookmark_list_len; i++) {
        ServerTypes::Bookmark bookmark;
        bookmark.name = readString(stream);
        for (int i = 0; i < 5; i++) {
            qint64 position;
            stream >> position;
            bookmark.motor_positions.append(position);
        }
        qint8 auto_focus;
        stream >> auto_focus;
        bookmark.auto_focus = (ServerTypes::AutoFocusSetting)auto_focus;
        bookmark_list.append(bookmark);
    }
}
