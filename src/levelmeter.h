#ifndef LEVELMETER_H
#define LEVELMETER_H

#include <QTime>
#include <QWidget>

/**
 * ** Some code taken/inspired from Qt example "Spectrum" **
 * Widget which displays a vertical audio level meter, indicating the
 * RMS and peak levels of the window of audio samples most recently analyzed
 * by the AudioInput class.
 */
class LevelMeter: public QWidget
{
    Q_OBJECT

public:
    explicit LevelMeter(QWidget *parent = nullptr);
    ~LevelMeter() override;

    void paintEvent(QPaintEvent *event) override;

public slots:
    void reset();
    void levelChanged(float rmsLevel, float peakLevel, int numSamples);

private slots:
    void redrawTimerExpired();

private:
    /**
     * Height of RMS level bar.
     * Range 0.0 - 1.0.
     */
    float rmsLevel;

    /**
     * Most recent peak level.
     * Range 0.0 - 1.0.
     */
    float peakLevel;

    /**
     * Height of peak level bar.
     * This is calculated by decaying m_peakLevel depending on the
     * elapsed time since m_peakLevelChanged, and the value of m_decayRate.
     */
    float decayedPeakLevel;

    /**
     * Time at which m_peakLevel was last changed.
     */
    QTime peakLevelChanged;

    /**
     * Rate at which peak level bar decays.
     * Expressed in level units / millisecond.
     */
    float peakDecayRate;

    /**
     * High watermark of peak level.
     * Range 0.0 - 1.0.
     */
    float peakHoldLevel;

    /**
     * Time at which m_peakHoldLevel was last changed.
     */
    QTime peakHoldLevelChanged;

    QTimer *redrawTimer;

    QColor rmsColor;
    QColor peakColor;

};

#endif // LEVELMETER_H
