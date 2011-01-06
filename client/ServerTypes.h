#ifndef SERVERTYPES_H
#define SERVERTYPES_H

#include <QString>
#include <QImage>
#include <QSet>

namespace ServerTypes {
    enum LoginStatus
    {
        Disconnected,
        Connecting,
        WaitingForMagicalResponse,
        WaitingForConnectionResult,
        ServerIsBogus,
        LoginIsInvalid,
        InsufficientPrivileges,
        Success,
        SocketError,
    };

    enum Permission
    {
        OperateHardware = 0,
        ManageUsers = 1,
    };

    class UserInfo
    {
    public:
        QString username;
        QSet<Permission> permissions;

        UserInfo(QString username, QSet<Permission> permissions) :
            username(username),
            permissions(permissions)
        {}
        UserInfo() :
            username(),
            permissions()
        {}
    };

    class DirectoryItem
    {
    public:
        qint64 byte_count;
        QString filename;
        QImage thumbnail;
    };
}

#endif // SERVERTYPES_H
