#ifndef SHADOWSLIDER_H
#define SHADOWSLIDER_H

#include <QSlider>
#include <QPaintEvent>

class ShadowSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ShadowSlider(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *ev);

};

#endif // SHADOWSLIDER_H
