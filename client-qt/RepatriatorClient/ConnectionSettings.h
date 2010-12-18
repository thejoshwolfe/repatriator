#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QString>

class ConnectionSettings
{
public:
    QString host;
    int port;
    QString username;
    QString password; // empty string means no saved password
    QString downloadDirectory;

    ConnectionSettings() :
        host(),
        port(0),
        username(),
        password(""),
        downloadDirectory()
    {}
};

#endif // CONNECTIONSETTINGS_H
