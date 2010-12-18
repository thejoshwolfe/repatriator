#ifndef OUTGOING_MESSAGE_H
#define OUTGOING_MESSAGE_H

#include "Server.h"

#include <QObject>
#include <QDataStream>
#include <QSet>

class OutgoingMessage : public QObject
{
    Q_OBJECT
public:
    void writeToStream(QDataStream & stream);

protected:
    enum MessageCode
    {
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
    };

    // serialize
    virtual void writeMessageBody(QDataStream & stream) = 0;
    virtual MessageCode type() const = 0;

    static void writeString(QDataStream & stream, QString string);

signals:

public slots:

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
    QSet<Server::Permission> permissions;
    UpdateUserMessage(QString username, QString password, QSet<Server::Permission> permissions) :
        username(username), password(password), permissions(permissions) {}
protected:
    virtual void writeMessageBody(QDataStream & stream);
    virtual MessageCode type() const { return UpdateUser; }

};

class AddUserMessage : public UpdateUserMessage
{
public:
    AddUserMessage(QString username, QString password, QSet<Server::Permission> permissions) :
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
#endif // OUTGOING_MESSAGE_H
