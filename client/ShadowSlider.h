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

protected:
    void paintEvent(QPaintEvent *ev);

private:
    int m_shadow_position;

};

#endif // SHADOWSLIDER_H
