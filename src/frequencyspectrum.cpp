#include "frequencyspectrum.h"

#include <math.h>

#include <QPainter>
#include <QTimer>

#include <QDebug>

FrequencySpectrum::FrequencySpectrum(QWidget *parent)
    :   QWidget(parent)
    ,   numSamples(0)
    ,   maxPower(1)
    ,   redrawTimer(new QTimer(this))
    ,   frequencyColor(Qt::red)
{

}

void FrequencySpectrum::frequenciesChanged(const double* frequencies, const size_t numSamples)
{
    this->frequencies = frequencies;
    this->numSamples = numSamples;
    qDebug() << numSamples;
    update();
}


FrequencySpectrum::~FrequencySpectrum() {}

void FrequencySpectrum::reset()
{
    maxPower = 0.0;
    update();
}

void FrequencySpectrum::redrawTimerExpired()
{
    update();
}

void FrequencySpectrum::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (numSamples == 0) return;

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setPen(frequencyColor);

    QRect bar = rect();
    int maxHeight = (bar.bottom() - bar.top());
    QPoint lastPoint = QPoint(0, maxHeight);
    painter.drawPoint(lastPoint);

    int samplesPerX = static_cast<int>(numSamples) / (bar.right() - bar.left());
    int currentX = 0;
    double avg = 0;
    for (int i = 0; i < static_cast<int>(numSamples); i++) {
        maxPower = std::max(maxPower, frequencies[i]);

        // We have more samples than pixels wide, so we average samplesPerX samples per x coord
        if (static_cast<int>(i / samplesPerX) == currentX) {
            avg += frequencies[i];
        }
        else {
            avg = avg / samplesPerX;
            currentX++;

            QPoint currentPoint = QPoint(i, maxHeight - static_cast<int>(frequencies[i] / maxPower * maxHeight));
            painter.drawLine(lastPoint, currentPoint);
            lastPoint = currentPoint;
            currentX++;
            avg = 0;
        }
    }
}
