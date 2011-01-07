#ifndef OUTGOING_MESSAGE_H
#define OUTGOING_MESSAGE_H

#include "ServerTypes.h"

#include <QObject>
#include <QDataStream>
#include <QSet>

class OutgoingMessage
{
public:
    virtual ~OutgoingMessage() {}

    void writeToStream(QDataStream & stream);

protected:
    enum MessageCode
    {
        DummyDisconnect = -1,
        MagicalRequest = 0,
        ConnectionRequest = 1,
        TakePicture = 2,
        MotorMovement = 3,
        DirectoryListingRequest = 4,
        FileDownloadRequest = 5,
        AddUser = 6,
        UpdateUser = 7,
        DeleteUser = 8,
        FileDeleteRequest = 9,
        ChangePasswordRequest = 10,
        ListUserRequest = 11,
        SetAutoFocusEnabled = 12,
        Ping = 13,
        SetStaticBookmarks = 14,
        SetUserBookmarks = 15,
    };

    // serialize
    virtual void writeMessageBody(QDataStream & stream) = 0;
    virtual MessageCode type() const = 0;

    static void writeString(QDataStream & stream, QString string);
};

class DummyDisconnectMessage : public OutgoingMessage
{
public:
    DummyDisconnectMessage() {}
protected:
    virtual void writeMessageBody(QDataStream & ) {}
    virtual MessageCode type() const { return DummyDisconnect; }
};

class MagicalRequestMessage : public OutgoingMessage
{
public:
    MagicalRequestMessage() {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return MagicalRequest; }
private:
    static const qint8 c_magical_data[];
};

class ConnectionRequestMessage : public OutgoingMessage
{
public:
    int client_major_version;
    int client_minor_version;
    int client_revision_number;
    QString username;
    QString password;
    bool hardware_access;

    ConnectionRequestMessage(
        int client_major_version,
        int client_minor_version,
        int client_revision_number,
        QString username,
        QString password,
        bool hardware_access) :
            client_major_version(client_major_version),
            client_minor_version(client_minor_version),
            client_revision_number(client_revision_number),
            username(username),
            password(password),
            hardware_access(hardware_access) {}

protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return ConnectionRequest; }
};

class TakePictureMessage : public OutgoingMessage
{
public:
    TakePictureMessage() {}
protected:
    virtual void writeMessageBody(QDataStream & ) {}
    virtual MessageCode type() const { return TakePicture; }
};

class MotorMovementMessage : public OutgoingMessage
{
public:
    // vector always contains positions of A, B, X, Y, Z respectively
    QVector<qint64> positions;
    MotorMovementMessage(QVector<qint64> positions) : positions(positions) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return MotorMovement; }
};

class DirectoryListingRequestMessage : public OutgoingMessage
{
public:
    DirectoryListingRequestMessage() {}
protected:
    virtual void writeMessageBody(QDataStream & ) {}
    virtual MessageCode type() const { return DirectoryListingRequest; }
};

class FileDownloadRequestMessage : public OutgoingMessage
{
public:
    QString filename;
    FileDownloadRequestMessage(QString filename) : filename(filename) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return FileDownloadRequest; }
};

class UpdateUserMessage : public OutgoingMessage
{
public:
    QString username;
    QString password;
    QSet<ServerTypes::Permission> permissions;
    UpdateUserMessage(QString username, QString password, QSet<ServerTypes::Permission> permissions) :
        username(username), password(password), permissions(permissions) {}
    virtual ~UpdateUserMessage() {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return UpdateUser; }
};

class AddUserMessage : public UpdateUserMessage
{
public:
    AddUserMessage(QString username, QString password, QSet<ServerTypes::Permission> permissions) :
            UpdateUserMessage(username, password, permissions) {}
protected:
    virtual MessageCode type() const { return AddUser; }
};

class DeleteUserMessage : public OutgoingMessage
{
public:
    QString username;
    DeleteUserMessage(QString username) : username(username) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return DeleteUser; }
};

class FileDeleteRequestMessage : public OutgoingMessage
{
public:
    QString filename;
    FileDeleteRequestMessage(QString filename) : filename(filename) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return FileDeleteRequest; }

};

class ChangePasswordRequestMessage : public OutgoingMessage
{
public:
    QString old_password;
    QString new_password;
    ChangePasswordRequestMessage(QString old_password, QString new_password) :
        old_password(old_password), new_password(new_password) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return ChangePasswordRequest; }
};

class ListUserRequestMessage : public OutgoingMessage
{
public:
    ListUserRequestMessage() {}
protected:
    virtual void writeMessageBody(QDataStream & ) {}
    virtual MessageCode type() const { return ListUserRequest; }
};

class SetAutoFocusEnabledMessage : public OutgoingMessage
{
public:
    bool value;
    SetAutoFocusEnabledMessage(bool value) : value(value) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return SetAutoFocusEnabled; }
};


class PingMessage : public OutgoingMessage
{
public:
    qint32 ping_id;
    PingMessage(qint32 ping_id) : ping_id(ping_id) {}
protected:
    virtual void writeMessageBody(QDataStream &stream);
    virtual MessageCode type() const { return Ping; }
};

class SetStaticBookmarksMessage : public OutgoingMessage
{
public:
    QVector<ServerTypes::Bookmark> bookmarks;
    SetStaticBookmarksMessage(QVector<ServerTypes::Bookmark> bookmarks)
        : bookmarks(bookmarks) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return SetStaticBookmarks; }
};

class SetUserBookmarksMessage : public SetStaticBookmarksMessage
{
public:
    SetUserBookmarksMessage(QVector<ServerTypes::Bookmark> bookmarks)
        : SetStaticBookmarksMessage(bookmarks) {}
protected:
    virtual MessageCode type() const { return SetUserBookmarks; }
};

#endif // OUTGOING_MESSAGE_H
