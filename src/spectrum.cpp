#include "spectrum.h"

Spectrum::Spectrum(QWidget *parent)
{
    Q_UNUSED (parent);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void Spectrum::paintEvent(QPaintEvent * /* event */) {}
