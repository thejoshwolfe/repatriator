#ifndef IMAGEDISPLAYWIDGET_H
#define IMAGEDISPLAYWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QMutex>
#include <QPaintEvent>
#include <QResizeEvent>

class ImageDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplayWidget(QWidget *parent = 0);

public slots:
    void prepareDisplayImage(QImage image);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

private:
    QPixmap m_currentFrame;
    QMutex m_videoMutex;

private:
    void scaleCurrentFrame();
};

#endif // IMAGEDISPLAYWIDGET_H
