#include "ShadowSlider.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QDebug>

ShadowSlider::ShadowSlider(QWidget *parent) :
    QSlider(parent)
{
    this->setTracking(false);
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
    option.upsideDown = ! this->invertedAppearance();
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
