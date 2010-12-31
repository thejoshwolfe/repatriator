#ifndef INCOMINGMESSAGEPARSER_H
#define INCOMINGMESSAGEPARSER_H

#include "IncomingMessage.h"

#include <QObject>
#include <QIODevice>

class IncomingMessageParser : public QObject
{
    Q_OBJECT
public:
    // this is so we can get a pointer to an instance, which  we need for
    // qt's signals and slots mechanism.
    static IncomingMessageParser * instance();

    // return a child message class for the data
    // this method will not return until the entire message
    // is received or there is an error.
    IncomingMessage * readMessage(QIODevice * device);

signals:
    // emitted during message downloading
    void progress(qint64 bytesTransferred, qint64 bytesTotal, IncomingMessage * msg);
private:

    static const int c_read_timeout_ms;
    static IncomingMessage * createMessageOfType(IncomingMessage::MessageCode type);

    static IncomingMessageParser * s_instance;

};


#endif // INCOMINGMESSAGEPARSER_H
