#include "levelmeter.h"

#include <math.h>

#include <QPainter>
#include <QTimer>

// Constants
const int RedrawInterval = 100;         // ms
const float PeakDecayRate = 0.001f;
const int PeakHoldLevelDuration = 2000; // ms

LevelMeter::LevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   rmsLevel(0.0)
    ,   peakLevel(0.0)
    ,   decayedPeakLevel(0.0)
    ,   peakDecayRate(PeakDecayRate)
    ,   peakHoldLevel(0.0)
    ,   redrawTimer(new QTimer(this))
    ,   rmsColor(Qt::red)
    ,   peakColor(255, 200, 200, 255)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(30);

    connect(redrawTimer, &QTimer::timeout, this, &LevelMeter::redrawTimerExpired);
    redrawTimer->start(RedrawInterval);
}

LevelMeter::~LevelMeter() {}

void LevelMeter::reset()
{
    rmsLevel = 0.0;
    peakLevel = 0.0;
    update();
}

void LevelMeter::levelChanged(float rmsLevel, float peakLevel, int numSamples)
{
    // Smooth the RMS signal
    const float smooth = std::pow(0.9f, static_cast<float>(numSamples) / 256); // TODO: remove this magic number
    this->rmsLevel = (this->rmsLevel * smooth) + (rmsLevel * (1.0f - smooth));

    if (peakLevel > decayedPeakLevel) {
        this->peakLevel = peakLevel;
        this->decayedPeakLevel = peakLevel;
        this->peakLevelChanged.start();
    }

    if (peakLevel > peakHoldLevel) {
        this->peakHoldLevel = peakLevel;
        this->peakHoldLevelChanged.start();
    }

    update();
}

void LevelMeter::redrawTimerExpired()
{
    // Decay the peak signal
    const int elapsedMs = peakLevelChanged.elapsed();
    const float decayAmount = peakDecayRate * elapsedMs;
    if (decayAmount < peakLevel)
        decayedPeakLevel = peakLevel - decayAmount;
    else
        decayedPeakLevel = 0.0;

    // Check whether to clear the peak hold level
    if (peakHoldLevelChanged.elapsed() > PeakHoldLevelDuration)
        peakHoldLevel = 0.0;

    update();
}

void LevelMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    QRect bar = rect();

    bar.setTop(static_cast<int>(rect().top() + (1.0f - peakHoldLevel) * rect().height()));
    bar.setBottom(bar.top() + 5);
    painter.fillRect(bar, rmsColor);
    bar.setBottom(rect().bottom());

    bar.setTop(static_cast<int>(rect().top() + (1.0f - decayedPeakLevel) * rect().height()));
    painter.fillRect(bar, peakColor);

    bar.setTop(static_cast<int>(rect().top() + (1.0f - rmsLevel) * rect().height()));
    painter.fillRect(bar, rmsColor);
}
