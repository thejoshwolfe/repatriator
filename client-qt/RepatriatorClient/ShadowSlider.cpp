#include "ShadowSlider.h"

#include <QPainter>

ShadowSlider::ShadowSlider(QWidget *parent) :
    QSlider(parent)
{
}

void ShadowSlider::paintEvent(QPaintEvent *ev)
{
    QSlider::paintEvent(ev);

    // also paint shadow
    QPainter p(this);
    p.fillRect(0, 0, 10, 10, Qt::black);
}
