#include "ShadowMinimap.h"

#include <QPainter>
#include <QCursor>
#include <QApplication>
#include <QDebug>
#include <QToolTip>

ShadowMinimap::ShadowMinimap(QWidget *parent) :
    QWidget(parent),
    m_shadow_position(0, 0),
    m_position(0, 0),
    m_max_position(100, 100),
    m_min_position(0, 0),
    m_mouse_down(false),
    c_thumb_size(20, 16),
    c_margin(10, 8),
    m_sensitivity(1.0f)
{
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);
}

void ShadowMinimap::setShadowPosition(QPoint pt)
{
    m_shadow_position.setX(qMin(qMax(pt.x(), m_min_position.x()), m_max_position.x()));
    m_shadow_position.setY(qMin(qMax(pt.y(), m_min_position.y()), m_max_position.y()));

    this->update();
}

void ShadowMinimap::setPosition(QPoint pt)
{
    m_position.setX(qMin(qMax(pt.x(), m_min_position.x()), m_max_position.x()));
    m_position.setY(qMin(qMax(pt.y(), m_min_position.y()), m_max_position.y()));

    this->update();
}

void ShadowMinimap::setMaxPosition(QPoint pt)
{
    m_max_position = pt;

    this->update();
}

void ShadowMinimap::setMinPosition(QPoint pt)
{
    m_min_position = pt;

    this->update();
}

void ShadowMinimap::mousePressEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton)
        return;

    m_mouse_origin = QCursor::pos();
    m_position_origin = m_position;
    m_total_pixels_moved = QPoint(0,0);
    m_mouse_down = true;
    this->setCursor(Qt::BlankCursor);

    e->accept();
}

void ShadowMinimap::mouseMoveEvent(QMouseEvent * e)
{
    if (! m_mouse_down || ! (e->buttons() & Qt::LeftButton))
        return;

    m_total_pixels_moved += QCursor::pos() - m_mouse_origin;
    QCursor::setPos(m_mouse_origin);
    QPointF ppp = pixelsPerPosition();
    QPoint pixel_delta = m_total_pixels_moved * m_sensitivity;
    QPoint position_delta = QPoint(pixel_delta.x() / ppp.x(), pixel_delta.y() / ppp.y());
    QPoint new_position = m_position_origin + position_delta;
    setPosition(new_position);
    QToolTip::showText(m_mouse_origin,
        QString::number(m_position.x()) + QString(", ") + QString::number(m_position.y()));

    e->accept();
}

void ShadowMinimap::mouseReleaseEvent(QMouseEvent * e)
{
    if (! m_mouse_down || e->button() != Qt::LeftButton)
        return;

    this->setCursor(Qt::ArrowCursor);
    QToolTip::hideText();

    m_mouse_down = false;
    if (m_position_origin != m_position)
        emit positionChosen(m_position);

    e->accept();
}

void ShadowMinimap::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        QPoint delta = m_sensitivity * (this->maxPosition() - this->minPosition()) / 10.0f;
        switch(e->key()){
        case Qt::Key_Left: m_position.rx() -= delta.x(); break;
        case Qt::Key_Right: m_position.rx() += delta.x(); break;
        case Qt::Key_Up: m_position.ry() -= delta.y(); break;
        case Qt::Key_Down: m_position.ry() += delta.y(); break;
        }
        setPosition(m_position);
        emit positionChosen(m_position);
        break;
    }
}

QSize ShadowMinimap::fieldSize() const
{
    return QSize(this->width() - c_margin.x() * 2, this->height() - c_margin.y() * 2);
}

QPointF ShadowMinimap::pixelsPerPosition() const
{
    QSize field_size = fieldSize();
    return QPointF(field_size.width() / (float)(m_max_position.x() - m_min_position.x()),
                   field_size.height() / (float)(m_max_position.y() - m_min_position.y()));
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
    Qt::GlobalColor border_color = this->hasFocus() ? Qt::blue : Qt::lightGray;
    p.setPen(QPen(border_color, borderWidth));
    p.drawRect(borderWidth / 2, borderWidth / 2, this->width() - borderWidth, this->height() - borderWidth);

    // shadow
    QPoint shadow_center = valueToPoint(m_shadow_position);
    Qt::GlobalColor shadow_color = this->isEnabled() ? Qt::darkGray : Qt::lightGray;
    p.fillRect(shadow_center.x() - c_thumb_size.x() / 2, shadow_center.y() - c_thumb_size.y() / 2, c_thumb_size.x(), c_thumb_size.y(), shadow_color);

    // thumb
    QPoint position_center = valueToPoint(m_position);
    Qt::GlobalColor thumb_color = this->isEnabled() ? Qt::black : Qt::darkGray;
    p.fillRect(position_center.x() - c_thumb_size.x() / 2, position_center.y() - c_thumb_size.y() / 2, c_thumb_size.x(), c_thumb_size.y(), thumb_color);
}

void ShadowMinimap::setSensitivity(float sensitivity)
{
    m_sensitivity = sensitivity;
}
