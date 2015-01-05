
#include "layer.h"

#include <QFrame>

Layer::Layer(QWidget *parent) :
    QWidget(parent)
{
    connect(&fadeTimer, SIGNAL(timeout()), this, SLOT(fadeAnim()));
    fadeAlpha = 0.0f;
}

Layer::~Layer()
{
}

void Layer::init(QFrame *frame)
{
    this->frame = frame;
}

void Layer::fadeIn()
{
    fadeTimer.start(50);
    fadeMode = 1;
    QPalette newPalette = palette();
    newPalette.setBrush(QPalette::Window, brushSetAlpha(newPalette.window(), 0));
    setPalette(newPalette);
    newPalette = frame->palette();
    newPalette.setBrush(QPalette::Window, brushSetAlpha(newPalette.window(), 0));
    frame->setPalette(newPalette);
    frame->hide();
    show();
}

void Layer::fadeOut()
{
    fadeTimer.start(50);
    fadeMode = 2;
    frame->hide();
}

void Layer::fadeAnim()
{
    if( fadeMode == 1 )
    {
        fadeAlpha += 255.0f / 50.0f * 8.0f;
    }
    else if( fadeMode == 2 )
    {
        fadeAlpha -= 255.0f / 50.0f * 8.0f;
    }

    if( fadeAlpha <= 0.0f )
    {
        fadeAlpha = 0.0f;
        fadeMode = 0;
        hide();
    }
    else if( fadeAlpha >= 255.0f )
    {
        fadeAlpha = 255.0f;
        fadeMode = 0;
        frame->show();
    }
    else
    {
        fadeTimer.start(50);
    }

    QPalette newPalette = palette();
    newPalette.setBrush(QPalette::Window, brushSetAlpha(newPalette.window(), 192.0f / 255.0f * fadeAlpha));
    setPalette(newPalette);

    newPalette = frame->palette();
    newPalette.setBrush(QPalette::Window, brushSetAlpha(newPalette.window(), 230.0f / 255.0f * fadeAlpha));
    frame->setPalette(newPalette);
}
