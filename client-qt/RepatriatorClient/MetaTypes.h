#ifndef METATYPES_H
#define METATYPES_H

#include <QMetaType>
#include <QAbstractSocket>

#include "IncomingMessage.h"
#include "OutgoingMessage.h"
#include "Connector.h"
#include "Server.h"

namespace MetaTypes {
    void registerMetaTypes() {
        qRegisterMetaType<Connector::FailureReason>("Connector::FailureReason");
        qRegisterMetaType<QSharedPointer<IncomingMessage> >("QSharedPointer<IncomingMessage>");
        qRegisterMetaType<QSharedPointer<OutgoingMessage> >("QSharedPointer<OutgoingMessage>");
        qRegisterMetaType<ServerTypes::LoginStatus>("ServerTypes::LoginStatus");
        qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    }
}

#endif // METATYPES_H
