#ifndef AUDIOANALYZERTHREAD_H
#define AUDIOANALYZERTHREAD_H

#include <map>
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

    std::vector<double>                 fftw_in;
    std::vector<std::complex<double>>   fftw_out;
    fftw_plan                           fftw_plan;

    std::vector<double>                 data_out;
    bool                                plan_initialized;

    /**
     * @brief Applies a windowing function to the frequency spectrum (fftw_out)
     */
    void applyWindowingFunction();

    /**
     * @brief Calculates the note of the frequency spectrum (data_out)
     * @param sampleRate The sample rate of the incoming audio to analyze
     */
    void calculateNote(const int sampleRate);
public slots:
    /**
     * ** Some code taken/inspired from Qt example "Spectrum" **
     * @brief Calculates the audio level and emits a levelChanged signal
     * @param data The data to analyze
     */
    void calculateLevel(const std::vector<float> &data);

    /**
     * @brief Calculates the frequency spectrum and emits frequenciesChanged signal
     * @param sampleRate The sample rate of the incoming audio to analyze
     * @param data The data to analyze
     */
    void calculateSpectrum(const int sampleRate, const std::vector<float> &data);
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
    void frequenciesChanged(const double* frequencies, size_t numSamples);

    /**
     * @brief Signal for when the note is updated
     * @param note The changed note
     */
    void noteChanged(const QString note);
};

#endif // AUDIOANALYZERTHREAD_H
