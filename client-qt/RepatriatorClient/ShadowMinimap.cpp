#include "ShadowMinimap.h"

#include <QPainter>

ShadowMinimap::ShadowMinimap(QWidget *parent) :
    QWidget(parent),
    m_shadow_position(20, 20),
    m_position(80, 80),
    m_max_position(100, 100),
    m_mouse_down(false),
    c_thumb_size(20, 16),
    c_margin(10, 8)
{
    this->setMouseTracking(true);
}

void ShadowMinimap::setShadowPosition(QPoint pt)
{
    m_shadow_position.setX(qMin(qMax(pt.x(), 0), m_max_position.x()));
    m_shadow_position.setY(qMin(qMax(pt.y(), 0), m_max_position.y()));

    this->update();
}

void ShadowMinimap::setPosition(QPoint pt)
{
    m_position.setX(qMin(qMax(pt.x(), 0), m_max_position.x()));
    m_position.setY(qMin(qMax(pt.y(), 0), m_max_position.y()));

    this->update();
}

void ShadowMinimap::setMaxPosition(QPoint pt)
{
    m_max_position.setX(qMax(pt.x(), 0));
    m_max_position.setY(qMax(pt.y(), 0));

    this->update();
}

void ShadowMinimap::mousePressEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton)
        return;

    m_mouse_origin = e->pos();
    m_position_origin = m_position;
    m_mouse_down = true;

    e->accept();
}

void ShadowMinimap::mouseMoveEvent(QMouseEvent * e)
{
    if (! m_mouse_down || e->button() != Qt::LeftButton)
        return;

    QPoint pixel_delta = e->pos() - m_mouse_origin;
    QPointF ppp = pixelsPerPosition();
    QPoint position_delta = QPoint(pixel_delta.x() / ppp.x(), pixel_delta.y() / ppp.y());
    setPosition(m_position_origin + position_delta);

    e->accept();
}

void ShadowMinimap::mouseReleaseEvent(QMouseEvent * e)
{
    if (! m_mouse_down || e->button() != Qt::LeftButton)
        return;

    m_mouse_down = false;
    if (m_position_origin != m_position)
        emit positionChosen();

    e->accept();
}

QSize ShadowMinimap::fieldSize() const
{
    return QSize(this->width() - c_margin.x() * 2, this->height() - c_margin.y() * 2);
}

QPointF ShadowMinimap::pixelsPerPosition() const
{
    QSize field_size = fieldSize();
    return QPointF(field_size.width() / (float)m_max_position.x(), field_size.height() / (float)m_max_position.y());
}

QPoint ShadowMinimap::valueToPoint(QPoint value) const
{
    QPointF ppp = pixelsPerPosition();
    return QPoint(value.x() * ppp.x(), value.y() * ppp.y()) + c_margin;
}

void ShadowMinimap::paintEvent(QPaintEvent *)
{
    const int borderWidth = 4;

    QPainter p(this);

    // border
    p.setPen(QPen(Qt::lightGray, borderWidth));
    p.drawRect(borderWidth / 2, borderWidth / 2, this->width() - borderWidth, this->height() - borderWidth);

    // shadow
    QPoint shadow_center = valueToPoint(m_shadow_position);
    p.fillRect(shadow_center.x() - c_thumb_size.x() / 2, shadow_center.y() - c_thumb_size.y() / 2, c_thumb_size.x(), c_thumb_size.y(), Qt::darkGray);

    // thumb
    QPoint position_center = valueToPoint(m_position);
    p.fillRect(position_center.x() - c_thumb_size.x() / 2, position_center.y() - c_thumb_size.y() / 2, c_thumb_size.x(), c_thumb_size.y(), Qt::black);
}

