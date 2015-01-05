
#ifndef PROGRESSLAYER_H
#define PROGRESSLAYER_H

#include "layer.h"
#include "ui_progresslayer.h"

class ProgressLayer : public Layer
{
    Q_OBJECT

public:
    ProgressLayer(QWidget *parent);
    ~ProgressLayer();

    void fadeIn();

    void setMessage(const QString &message);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setValue(int value);

private:
    Ui::ProgressLayer ui;

signals:
    void canceled();
};

#endif
