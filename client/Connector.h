#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "ConnectionSettings.h"
#include "Server.h"

#include <QObject>
#include <QProgressDialog>
#include <QSharedPointer>

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
    explicit Connector(QSharedPointer<Server> server);
    virtual ~Connector();

public slots:
    // after you hook up your slots, call go. you will either get the
    // success or failure signal
    void go();

signals:
    // be careful because the object no longer exists when these signals are emitted.
    // success comes with a server pointer that is connected and authenticated.
    void success();
    void failure(Connector::FailureReason reason);

private:
    QSharedPointer<QProgressDialog> m_progressDialog;
    QSharedPointer<Server> m_server;
    bool m_done;

private:
    void fail(FailureReason reason);
    void cleanup(bool kill_connection = true);

private slots:
    void updateProgressFromLoginStatus(ServerTypes::LoginStatus status);
    void cancel();

};

#endif // CONNECTOR_H
