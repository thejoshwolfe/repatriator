#include "IncomingMessageParser.h"

const int IncomingMessageParser::c_read_timeout_ms = 2000;
IncomingMessageParser * IncomingMessageParser::s_instance = NULL;

IncomingMessage * IncomingMessageParser::readMessage(QIODevice * device)
{
    const qint64 header_byte_count = 9;

    // wait for stream to fill up with header
    while (device->bytesAvailable() < header_byte_count) {
        if (! device->waitForReadyRead(c_read_timeout_ms)) {
            Q_ASSERT(false);
            qWarning() << "timeout when reading message header";
            return NULL;
        }
    }

    QDataStream stream(device);

    qint8 type;
    stream >> type;

    qint64 total_byte_count;
    stream >> total_byte_count;

    IncomingMessage * msg = createMessageOfType((IncomingMessage::MessageCode) type);

    msg->type = (IncomingMessage::MessageCode) type;
    msg->byte_count = total_byte_count;

    // wait for stream to fill up with entire message
    qint64 message_byte_count = total_byte_count - header_byte_count;
    emit progress(qMin(device->bytesAvailable(), message_byte_count), message_byte_count, msg);
    while (device->bytesAvailable() < message_byte_count) {
        if (! device->waitForReadyRead(c_read_timeout_ms)) {
            Q_ASSERT(false);
            qWarning() << "timeout when reading message body";
            return NULL;
        }
        emit progress(qMin(device->bytesAvailable(), message_byte_count), message_byte_count, msg);
    }
    msg->parse(stream);

    return msg;
}

IncomingMessage * IncomingMessageParser::createMessageOfType(IncomingMessage::MessageCode type)
{
    switch(type) {
    case IncomingMessage::MagicalResponse:
        return new MagicalResponseMessage;
    case IncomingMessage::ConnectionResult:
        return new ConnectionResultMessage;
    case IncomingMessage::FullUpdate:
        return new FullUpdateMessage;
    case IncomingMessage::DirectoryListingResult:
        return new DirectoryListingResultMessage;
    case IncomingMessage::FileDownloadResult:
        return new FileDownloadResultMessage;
    case IncomingMessage::ErrorMessage:
        return new ErrorMessage;
    case IncomingMessage::ListUserResult:
        return new ListUserResultMessage;
    }
    Q_ASSERT(false);
    return NULL;
}

IncomingMessageParser * IncomingMessageParser::instance()
{
    if (s_instance == NULL)
        s_instance = new IncomingMessageParser;
    return s_instance;
}
