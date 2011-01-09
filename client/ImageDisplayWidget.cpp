#include "ImageDisplayWidget.h"

#include <QPainter>

ImageDisplayWidget::ImageDisplayWidget(QWidget *parent) :
    QWidget(parent),
    m_currentFrame(),
    m_videoMutex(),
    m_focusPoint(0.5f, 0.5f),
    m_focusSize(1, 1),
    m_frameDrawLocation(0, 0)
{
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    this->setAutoFillBackground(false);
    this->setAttribute(Qt::WA_OpaquePaintEvent);
    this->setMouseTracking(true);
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

void ImageDisplayWidget::clear()
{
    m_videoMutex.lock();
    m_currentFrame = QPixmap();
    m_videoMutex.unlock();
    update();
}

void ImageDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRectF widgetSize = this->rect();

    m_videoMutex.lock();
    painter.fillRect(widgetSize, Qt::black);
    bool frame_null = m_currentFrame.isNull();
    if (!frame_null)
        painter.drawPixmap(m_frameDrawLocation, m_currentFrame);
    m_videoMutex.unlock();

    if (! frame_null) {
        QRect focus_rect(m_focusPoint.x()*widgetSize.width()-m_focusSize.width()/2,
                         m_focusPoint.y()*widgetSize.height()-m_focusSize.height()/2,
                         m_focusSize.width(), m_focusSize.height());
        painter.setPen(QPen(Qt::white, 3));
        painter.drawRect(focus_rect);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(focus_rect);
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
    if (m_currentFrame.isNull())
        return;

    m_currentFrame = m_currentFrame.scaled(this->rect().size(), Qt::KeepAspectRatio);
    m_focusSize = m_currentFrame.size() / 5.0f;
    QRectF widgetSize = this->rect();
    m_frameDrawLocation = QPoint(widgetSize.width() / 2 - m_currentFrame.width() / 2,
                                  widgetSize.height() / 2 - m_currentFrame.height() / 2);
}

void ImageDisplayWidget::setFocusPoint(QPointF point)
{
    m_focusPoint = point;
    update();
}

void ImageDisplayWidget::mousePressEvent(QMouseEvent * e)
{
    mouseMoveEvent(e);
}

void ImageDisplayWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_videoMutex.lock();
        m_focusPoint.setX((e->x() - m_frameDrawLocation.x()) / (float)m_currentFrame.size().width());
        m_focusPoint.setY((e->y() - m_frameDrawLocation.y()) / (float)m_currentFrame.size().height());
        m_videoMutex.unlock();
        update();
        e->accept();
    }
}

void ImageDisplayWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        emit focusPointChanged(m_focusPoint);
        e->accept();
    }
}
