#ifndef AUDIOANALYZERTHREAD_H
#define AUDIOANALYZERTHREAD_H

#include <complex>

#include <QObject>

#include <boost/circular_buffer.hpp>
#include <fftw3.h>

class AudioAnalyzerThread : public QObject
{
    Q_OBJECT
public:
    AudioAnalyzerThread();
    ~AudioAnalyzerThread();
private:
    // The thread it will be running on
    QThread* thread;

    boost::circular_buffer<int> cbuffer;

    std::vector<double>                 fftw_in;
    std::vector<std::complex<double>>   fftw_out;
    fftw_plan                           fftw_plan;

    bool                                plan_initialized;
public slots:
    /**
     * ** Some code taken/inspired from Qt example "Spectrum" **
     * @brief Calculates the audio level and emits a levelChanged signal
     * @param data The data to analyze
     */
    void calculateLevel(const std::vector<float> &data);

    /**
     * @brief Calculates the frequency spectrum and emits frequenciesChanged signal
     * @param data The data to analyze
     */
    void calculateSpectrum(const std::vector<float> &data);
signals:
    /**
     * Signal for audio level change
     * \param rmsLevel RMS level in range 0.0 - 1.0
     * \param peakLevel Peak level in range 0.0 - 1.0
     * \param numSamples Number of audio samples analyzed
     */
    void levelChanged(float rmsLevel, float peakLevel, size_t numSamples);

    /**
     * @brief Signal for audio frequencies change
     * @param frequencies The frequencies and their value
     * @param numSamples Number of audio samples analyzed
     */
    void frequenciesChanged(const std::complex<double>* frequencies, size_t numSamples);
};

#endif // AUDIOANALYZERTHREAD_H
