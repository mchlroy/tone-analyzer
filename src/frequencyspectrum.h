#ifndef FREQUENCYSPECTRUM_H
#define FREQUENCYSPECTRUM_H

#include <QTime>
#include <QWidget>

#include <complex>

#include <fftw3.h>

class FrequencySpectrum : public QWidget
{
    Q_OBJECT

public:
    explicit FrequencySpectrum(QWidget *parent = nullptr);
    ~FrequencySpectrum() override;

    void paintEvent(QPaintEvent *event) override;


public slots:
    void reset();
    void frequenciesChanged(const double* frequencies, const size_t numSamples);

private slots:
    void redrawTimerExpired();

private:
    /**
     * Height of peak level bar.
     * This is calculated by decaying m_peakLevel depending on the
     * elapsed time since m_peakLevelChanged, and the value of m_decayRate.
     */
    const double* frequencies;

    size_t numSamples;

    double maxPower;
    /**
     * Time at which m_peakHoldLevel was last changed.
     */
    //QTime frequenciesChanged;

    QTimer *redrawTimer;

    QColor frequencyColor;

};

#endif // FREQUENCYSPECTRUM_H
