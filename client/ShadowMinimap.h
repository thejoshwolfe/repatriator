#ifndef SHADOWMINIMAP_H
#define SHADOWMINIMAP_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>

class ShadowMinimap : public QWidget
{
    Q_OBJECT
public:
    explicit ShadowMinimap(QWidget *parent = 0);

    QPoint shadowPosition() const { return m_shadow_position; }
    void setShadowPosition(QPoint pt);

    QPoint position() const { return m_position; }
    void setPosition(QPoint pt);

    QPoint maxPosition() const { return m_max_position; }
    void setMaxPosition(QPoint pt);

signals:
    void positionChosen(QPoint position);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    QPoint m_shadow_position;
    QPoint m_position;
    QPoint m_max_position;

    QPoint m_mouse_origin;
    QPoint m_position_origin;
    bool m_mouse_down;

    const QPoint c_thumb_size;
    const QPoint c_margin;

private:
    QSize fieldSize() const;
    QPointF pixelsPerPosition() const;
    QPoint valueToPoint(QPoint value) const;
};

#endif // SHADOWMINIMAP_H
