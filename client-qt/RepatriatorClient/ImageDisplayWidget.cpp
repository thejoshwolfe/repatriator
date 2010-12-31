#include "ImageDisplayWidget.h"

#include <QPainter>

ImageDisplayWidget::ImageDisplayWidget(QWidget *parent) :
    QWidget(parent),
    m_currentFrame(),
    m_videoMutex()
{
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    this->setAutoFillBackground(false);
    this->setAttribute(Qt::WA_OpaquePaintEvent);
}

void ImageDisplayWidget::prepareDisplayImage(QImage image)
{
    // change pointer to new frame
    m_videoMutex.lock();
    m_currentFrame = QPixmap::fromImage(image);
    scaleCurrentFrame();
    m_videoMutex.unlock();
    update();
}

void ImageDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRectF widgetSize = this->rect();
    float x = widgetSize.width() / 2 - m_currentFrame.width() / 2;
    float y = widgetSize.height() / 2 - m_currentFrame.height() / 2;

    if (m_videoMutex.tryLock()) {
        if (m_currentFrame.isNull()) {
            // haven't seen any frames yet, clear the runway
            painter.fillRect(widgetSize, Qt::black);
        } else {
            painter.drawPixmap(x, y, m_currentFrame);
        }
        m_videoMutex.unlock();
    }
}

void ImageDisplayWidget::resizeEvent(QResizeEvent *)
{
    m_videoMutex.lock();
    scaleCurrentFrame();
    m_videoMutex.unlock();
}

void ImageDisplayWidget::scaleCurrentFrame()
{
    QRectF widgetSize = this->rect();
    m_currentFrame = m_currentFrame.scaledToWidth(widgetSize.width());
}
