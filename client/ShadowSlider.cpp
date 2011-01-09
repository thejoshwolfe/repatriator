#include "ShadowSlider.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QDebug>
#include <QToolTip>
#include <QCursor>

ShadowSlider::ShadowSlider(QWidget *parent) :
    QSlider(parent),
    m_sensitivity(1.0f),
    m_mouse_down(false)
{
    this->setTracking(false);
    this->setMouseTracking(true);
    setSensitivity(m_sensitivity);
}

void ShadowSlider::mousePressEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton)
        return;

    m_mouse_origin = QCursor::pos();
    m_position_origin = this->value();
    m_total_pixels_moved = QPoint(0, 0);
    m_mouse_down = true;
    this->setCursor(Qt::BlankCursor);

    e->accept();
}

void ShadowSlider::mouseMoveEvent(QMouseEvent * e)
{
    if (! m_mouse_down || ! (e->buttons() & Qt::LeftButton))
        return;

    m_total_pixels_moved += QCursor::pos() - m_mouse_origin;
    QCursor::setPos(m_mouse_origin);
    float ppp = pixelsPerPosition();
    QPoint pixel_delta = m_total_pixels_moved * m_sensitivity;
    float position_delta = ((this->orientation() == Qt::Horizontal) ? pixel_delta.x() : pixel_delta.y()) / ppp;
    int new_position = m_position_origin + position_delta;
    this->blockSignals(true);
    this->setValue(new_position);
    this->blockSignals(false);
    QToolTip::showText(m_mouse_origin, QString::number(this->value()));

    e->accept();
}

void ShadowSlider::mouseReleaseEvent(QMouseEvent * e)
{
    if (! m_mouse_down || e->button() != Qt::LeftButton)
        return;

    this->setCursor(Qt::ArrowCursor);
    QToolTip::hideText();

    m_mouse_down = false;
    if (m_position_origin != this->value())
        emit valueChanged(this->value());

    e->accept();
}

void ShadowSlider::keyPressEvent(QKeyEvent *ev)
{
    QSlider::keyPressEvent(ev);
    emit valueChanged(this->value());
}

void ShadowSlider::paintEvent(QPaintEvent * ev)
{
    QSlider::paintEvent(ev);
    QStyleOptionSlider option;

    option.initFrom(this);
    option.dialWrapping = false;
    option.maximum = this->maximum();
    option.minimum = this->minimum();
    option.notchTarget = 0;
    option.orientation = this->orientation();
    option.pageStep = 0;
    option.sliderPosition = m_shadow_position;
    option.sliderValue = m_shadow_position;
    option.tickInterval = 0;
    option.tickPosition = QSlider::NoTicks;
    option.upsideDown = this->orientation() == Qt::Vertical && ! this->invertedAppearance();
    option.subControls = QStyle::SC_DialHandle;

    QPainter painter(this);
    painter.setOpacity(0.5);
    this->style()->drawComplexControl(QStyle::CC_Slider, &option, &painter, this);
}

void ShadowSlider::setShadowPosition(int value)
{
    m_shadow_position = qMax(qMin(value, this->maximum()), this->minimum());
    this->update();
}

void ShadowSlider::setSensitivity(float sensitivity)
{
    m_sensitivity = sensitivity;
    this->setSingleStep(m_sensitivity * (this->maximum() - this->minimum()) / 10);
}

float ShadowSlider::pixelsPerPosition() const
{
    float size = (this->orientation() == Qt::Vertical) ? this->rect().height() : this->rect().width();
    return size / (float) (this->maximum() - this->minimum());
}
