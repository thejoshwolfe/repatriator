#ifndef INCOMING_MESSAGE_H
#define INCOMING_MESSAGE_H

#include <QIODevice>
#include <QImage>

#include "ServerTypes.h"

class IncomingMessage
{
public:
    enum MessageCode {
        MagicalResponse = 0,
        ConnectionResult = 1,
        FullUpdate = 2,
        DirectoryListingResult = 3,
        FileDownloadResult = 4,
        ErrorMessage = 5,
        ListUserResult = 6,
        Pong = 7,
        InitInfo = 8,
    };

    MessageCode type;
    qint64 byte_count;

public:
    virtual ~IncomingMessage() {}

    // parse the message after the header and populate member variables
    // the stream is guaranteed to have the entire message ready to read
    virtual void parse(QDataStream & stream) = 0;

protected:
    static QString readString(QDataStream &stream);
    static QImage readImage(QDataStream &stream);
};

class MagicalResponseMessage : public IncomingMessage
{
public:
    bool is_valid;

    virtual void parse(QDataStream & stream);
private:
    static const qint8 c_magical_response[];
};

class ConnectionResultMessage : public IncomingMessage
{
public:
    enum ConnectionResultStatus {
        InvalidLogin = 0,
        InsufficientPrivileges = 1,
        Success = 2,
    };

    qint32 protocol;
    QString server_description;
    ConnectionResultStatus connection_status;
    QSet<ServerTypes::Permission> permissions;

    virtual void parse(QDataStream & stream);
};

class FullUpdateMessage : public IncomingMessage
{
public:
    // always A, B, X, Y, Z
    QVector<qint8> motor_states;
    QVector<qint64> motor_positions;
    QImage image;

    virtual void parse(QDataStream & stream);
};

class DirectoryListingResultMessage : public IncomingMessage
{
public:
    QList<ServerTypes::DirectoryItem> directory_list;

    virtual void parse(QDataStream & stream);
};

class FileDownloadResultMessage : public IncomingMessage
{
public:
    QString filename;
    QByteArray file;

    virtual void parse(QDataStream & stream);
};

class ErrorMessage : public IncomingMessage
{
public:
    QString message;
    int number;

    virtual void parse(QDataStream & stream);
};

class ListUserResultMessage : public IncomingMessage
{
public:
    QList<ServerTypes::UserInfo> users;

    virtual void parse(QDataStream & stream);
};

class PongMessage : public IncomingMessage
{
public:
    qint32 ping_id;

    virtual void parse(QDataStream &);
};

class InitInfoMessage : public IncomingMessage
{
public:
    struct MotorBoundaries {
        qint64 min;
        qint64 max;
    };

    QVector<MotorBoundaries> motor_boundaries;
    QVector<ServerTypes::Bookmark> static_bookmarks;
    QVector<ServerTypes::Bookmark> user_bookmarks;

    virtual void parse(QDataStream &);

    static void readBookmarkList(QDataStream & stream, QVector<ServerTypes::Bookmark> & bookmark_list);
};


#endif // INCOMING_MESSAGE_H
