#include "IncomingMessageParser.h"

#include <QDebug>

const int IncomingMessageParser::c_read_timeout_ms = 2000;

IncomingMessageParser::IncomingMessageParser(QIODevice *device) :
    m_device(device),
    m_header_done(false)
{
    bool success;
    success = connect(m_device, SIGNAL(readyRead()), this, SLOT(readMessage()), Qt::QueuedConnection);
    Q_ASSERT(success);
}

void IncomingMessageParser::readMessage()
{
    const qint64 header_byte_count = 9;

    QDataStream stream(m_device);

    // until bytes available isn't enough
    forever {
        if (! m_header_done) {
            // wait for enough bytes
            if (m_device->bytesAvailable() < header_byte_count)
                return;

            qint8 type;
            stream >> type;

            qint64 total_byte_count;
            stream >> total_byte_count;

            m_in_progress_msg = createMessageOfType((IncomingMessage::MessageCode) type);

            m_in_progress_msg->type = (IncomingMessage::MessageCode) type;
            m_in_progress_msg->byte_count = total_byte_count;

            m_header_done = true;

            qint64 message_byte_count = m_in_progress_msg->byte_count - header_byte_count;
            emit progress(qMin(m_device->bytesAvailable(), message_byte_count), message_byte_count, m_in_progress_msg);
        }

        // wait for stream to fill up with entire message
        qint64 message_byte_count = m_in_progress_msg->byte_count - header_byte_count;
        emit progress(qMin(m_device->bytesAvailable(), message_byte_count), message_byte_count, m_in_progress_msg);
        if (m_device->bytesAvailable() < message_byte_count)
            return;
        m_in_progress_msg->parse(stream);
        emit messageReceived(QSharedPointer<IncomingMessage>(m_in_progress_msg));

        m_header_done = false;
    }
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
