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

    enum AutoFocusSetting
    {
        NotSpecified = 0,
        SetOn = 1,
        SetOff = 2,
    };

    struct MotorBoundaries {
        qint64 min;
        qint64 max;
        MotorBoundaries() {}
        MotorBoundaries(qint64 min, qint64 max) : min(min), max(max) {}
    };

    const qint64 MotorPositionNotSpecified = -0x8000000000000000;
    struct Bookmark
    {
        QString name;
        QVector<qint64> motor_positions;
        AutoFocusSetting auto_focus;
    };
}

#endif // SERVERTYPES_H
