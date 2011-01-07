#ifndef SHADOWSLIDER_H
#define SHADOWSLIDER_H

#include <QSlider>
#include <QPaintEvent>

class ShadowSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ShadowSlider(QWidget *parent = 0);

    int shadowPosition() const { return m_shadow_position; }
    void setShadowPosition(int value);

    void setSensitivity(float sensitivity);
    float sensitivity() const { return m_sensitivity; }

protected:
    void paintEvent(QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    int m_shadow_position;
    float m_sensitivity;

    QPoint m_mouse_origin;
    int m_position_origin;
    QPoint m_total_pixels_moved;

    bool m_mouse_down;
private:
    float pixelsPerPosition() const;
};

#endif // SHADOWSLIDER_H
