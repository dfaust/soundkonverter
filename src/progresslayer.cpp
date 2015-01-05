
#include "progresslayer.h"

ProgressLayer::ProgressLayer(QWidget *parent) :
    Layer(parent)
{
    ui.setupUi(this);

    connect(ui.cancelButton, SIGNAL(clicked()), SIGNAL(canceled()));

    init(ui.frame);
}

ProgressLayer::~ProgressLayer()
{
}

void ProgressLayer::fadeIn()
{
    ui.cancelButton->setEnabled(true);

    Layer::fadeIn();
}

void ProgressLayer::setMessage(const QString &message)
{
    ui.messageLabel->setText(message);
}

void ProgressLayer::setMinimum(int minimum)
{
    ui.progressBar->setMinimum(minimum);
}

void ProgressLayer::setMaximum(int maximum)
{
    ui.progressBar->setMaximum(maximum);
}

void ProgressLayer::setValue(int value)
{
    ui.progressBar->setValue(value);
}
