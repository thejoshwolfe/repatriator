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
    void clear();
    void setFocusPoint(QPointF point);

signals:
    void focusPointChanged(QPointF point);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);

private:
    QPixmap m_currentFrame;
    QMutex m_videoMutex;
    QPointF m_focusPoint; // percent

private:
    void scaleCurrentFrame();
};

#endif // IMAGEDISPLAYWIDGET_H
