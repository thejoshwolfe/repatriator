#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "ConnectionSettings.h"
#include "Server.h"

#include <QObject>

class QProgressDialog;

// Connector will display a progress dialog and status and try to connect
// to a ConnectionSettings.
// it takes care of the handshake and athentication for you and then sends
// either a failure or success signal.
// don't ever delete the pointer to a Connector. It takes care of that itself.
class Connector : public QObject
{
    Q_OBJECT
public:
    enum FailureReason {
        UnableToConnect,
        ServerIsBogus,
        LoginIsInvalid,
        InsufficientPrivileges,
        Cancelled,
    };

public:
    // use this instead of a constructor
    static Connector * create(ConnectionSettings * connection, bool need_hardware);

public slots:
    // after you hook up your slots, call go. you will either get success
    // or failure.
    void go();

signals:
    // be careful because the object no longer exists when these signals are emitted.
    // success comes with a server pointer that is connected and authenticated.
    void success(QSharedPointer<Server> server);
    void failure(int reason); // type FailureReason

private:
    ConnectionSettings * m_connection;
    bool m_need_hardware;
    QProgressDialog * m_progressDialog;
    QSharedPointer<Server> m_server;

private:
    explicit Connector(ConnectionSettings * connection, bool need_hardware);
    ~Connector() {}

    void fail(FailureReason reason);
    void bye();
private slots:
    void updateProgressFromLoginStatus(Server::LoginStatus status);
    void cancel();

};

#endif // CONNECTOR_H
