
#ifndef LAYER_H
#define LAYER_H

#include <QWidget>
#include <QColor>
#include <QTimer>

class QFrame;

class Layer : public QWidget
{
    Q_OBJECT

public:
    Layer(QWidget *parent);
    ~Layer();

    void init(QFrame *frame);

    virtual void fadeIn();
    virtual void fadeOut();

private:
    QFrame *frame;

    QTimer fadeTimer;
    float fadeAlpha;
    int fadeMode; // 1 = fade in, 2 = fade out

    inline QBrush brushSetAlpha(QBrush brush, const int alpha)
    {
        QColor color = brush.color();
        color.setAlpha(alpha);
        brush.setColor(color);
        return brush;
    }

private slots:
    void fadeAnim();
};

#endif
