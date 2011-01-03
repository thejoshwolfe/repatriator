#ifndef INCOMINGMESSAGEPARSER_H
#define INCOMINGMESSAGEPARSER_H

#include "IncomingMessage.h"

#include <QObject>
#include <QIODevice>
#include <QSharedPointer>

class IncomingMessageParser : public QObject
{
    Q_OBJECT
public:
    IncomingMessageParser(QIODevice * device);

signals:
    // emitted during message downloading
    void progress(qint64 bytesTransferred, qint64 bytesTotal, IncomingMessage * msg);

    // emitted when we gather enough data to put together a message
    void messageReceived(QSharedPointer<IncomingMessage> msg);
private:
    static const int c_read_timeout_ms;
    static IncomingMessage * createMessageOfType(IncomingMessage::MessageCode type);

    QIODevice * m_device;

    bool m_header_done;
    IncomingMessage * m_in_progress_msg;

private slots:
    void readMessage();

};


#endif // INCOMINGMESSAGEPARSER_H
