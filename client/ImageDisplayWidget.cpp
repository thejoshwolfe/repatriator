#include "ImageDisplayWidget.h"

#include <QPainter>

ImageDisplayWidget::ImageDisplayWidget(QWidget *parent) :
    QWidget(parent),
    m_currentFrame(),
    m_videoMutex(),
    m_focusPoint(0.5f, 0.5f)
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
    float x = widgetSize.width() / 2 - m_currentFrame.width() / 2;
    float y = widgetSize.height() / 2 - m_currentFrame.height() / 2;

    if (m_videoMutex.tryLock()) {
        painter.fillRect(widgetSize, Qt::black);
        if (!m_currentFrame.isNull())
            painter.drawPixmap(x, y, m_currentFrame);
        m_videoMutex.unlock();
    }

    int focusPtWidth = 20;
    int focusPtHeight = focusPtWidth / 1.61803;
    painter.drawRect(m_focusPoint.x()*this->rect().width()-focusPtWidth/2,
                     m_focusPoint.y()*this->rect().height()-focusPtHeight/2, focusPtWidth, focusPtHeight);
}

void ImageDisplayWidget::resizeEvent(QResizeEvent *)
{
    m_videoMutex.lock();
    scaleCurrentFrame();
    m_videoMutex.unlock();
}

void ImageDisplayWidget::scaleCurrentFrame()
{
    if (! m_currentFrame.isNull())
        m_currentFrame = m_currentFrame.scaled(this->rect().size(), Qt::KeepAspectRatio);
}

void ImageDisplayWidget::setFocusPoint(QPointF point)
{
    m_focusPoint = point;
    update();
}

void ImageDisplayWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_focusPoint.setX(e->x() / (float)this->rect().width());
        m_focusPoint.setY(e->y() / (float)this->rect().height());
        update();
        e->accept();
    }
}

void ImageDisplayWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        emit focusPointChanged(m_focusPoint);
        e->accept();
    }
}
