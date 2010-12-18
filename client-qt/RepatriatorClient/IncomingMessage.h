#ifndef INCOMING_MESSAGE_H
#define INCOMING_MESSAGE_H

#include <QObject>
#include <QIODevice>
#include <QImage>

#include "Server.h"

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
    };

    MessageCode type;
    qint64 byte_count;

public:
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

    qint32 major_version;
    qint32 minor_version;
    qint32 revision_version;
    ConnectionResultStatus connection_status;
    QSet<Server::Permission> permissions;

    virtual void parse(QDataStream & stream);
};

class FullUpdateMessage : public IncomingMessage
{
public:
    // always A, B, X, Y, Z
    QVector<qint64> motor_positions;
    QImage image;

    virtual void parse(QDataStream & stream);
};

class DirectoryListingResultMessage : public IncomingMessage
{
public:
    QList<Server::DirectoryItem> directory_list;

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
    QList<Server::UserInfo> users;

    virtual void parse(QDataStream & stream);
};


#endif // INCOMING_MESSAGE_H
