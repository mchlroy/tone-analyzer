#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <vector>
#include <chrono>
#include <complex>
#include <memory>
#include <cstring>

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioBuffer>
#include <QIODevice>
#include <QThread>

#include <boost/circular_buffer.hpp>
#include <deque>
#include <fftw3.h>

QT_BEGIN_NAMESPACE
    class QAudioInput;
QT_END_NAMESPACE

class AudioInput : public QObject
{
    Q_OBJECT
    QThread fftWorkerThread;
public:
    AudioInput();

    /**
     * @brief Returns the available audio input devices on the system
     * @return List of devices @see{QAudioDeviceInfo}
     */
    const std::vector<QAudioDeviceInfo> &getAvailableAudioInputDevices() const { return availableAudioInputDevices; }

    /**
     * @brief Return the current audio format
     * @return Current audio format
     * @note May be QAudioFormat() if engine is not initialized @see{QAudioFormat}
     */
    const QAudioFormat& getFormat() const { return format; }

    /**
     * @brief Sets the current audio input device
     * @param audioInputDevice The audio input device to set
     */
    void setAudioInputDevice(const QAudioDeviceInfo &audioInputDevice);

    /**
     * @brief Sets the audio format for the input device
     * @param format The audio input device format to set
     */
    void setFormat(const QAudioFormat &format);

private:
    size_t                  numBytesRead;       // The number of bytes read since analysis
    size_t                  bufferLength;       // The length of the buffer
    size_t                  bufferPosition;     // The position in which to insert newly received data

    std::vector<QAudioDeviceInfo> availableAudioInputDevices;   // List of available audio input devices
    QAudioBuffer            qbuffer;            // Buffer containing the audio
    QByteArray              qbbuffer;
    std::vector<char>       tempBuffer;
    std::vector<float>      data;
    boost::circular_buffer<char>        cbuffer;
    std::deque<char>                    dbuffer;
    std::vector<double>                 fftw_in;
    std::vector<std::complex<double>>   fftw_out;
    QAudioFormat            format;             // Format of the receiving audio data
    QAudioDeviceInfo        audioInputDevice;   // Currently selected audio input device
    QAudioInput*            audioInput;         // Interface for receiving audio data
    QIODevice*              audioInputIODevice; // Interface receiving audio data (returned by audioInput->start())

    fftw_plan               fftw_plan;

    bool                    plan_initialized;

private:
    /**
     * @brief Initializes the audio input device for listening
     * @return True if the initilization was successful
     */
    bool initialize();

    void calculateWaveForm();

    /**
     * @brief Calculates the audio level and emits a levelChanged signal
     */
    void calculateLevel();

    /**
     * @brief Calculates the frequency spectrum and emits frequenciesChanged signal
     */
    void calculateSpectrum();

    std::chrono::time_point<std::chrono::high_resolution_clock> pre;
    std::chrono::time_point<std::chrono::high_resolution_clock> post;
private slots:
    /**
     * @brief To be called when receiving a certain timeframe of data (NOTIFY_INTERVAL_MS)
     */
    void audioNotify();

    /**
     * @brief To be called when the audio input has changed state
     * @param state The new audio device state
     */
    void audioStateChanged(QAudio::State state);

    /**
     * @brief To be called when new data is ready to be fetched from the input device
     */
    void audioDataReady();

public slots:
    /**
     * @brief Starts listening on the audio input device
     */
    void startListening();

    /**
     * @brief Stops listening on the audio input device
     */
    void stopListening();

    /**
     * @brief
     */
    void handleFFTResults(const std::complex<double>* frequencies, std::size_t numSamples);
signals:
    void waveFormChanged(const std::vector<float>& data, size_t numSamples, const size_t time);

    /**
     * Signal for audio level change
     * \param rmsLevel RMS level in range 0.0 - 1.0
     * \param peakLevel Peak level in range 0.0 - 1.0
     * \param numSamples Number of audio samples analyzed
     */
    void levelChanged(double rmsLevel, double peakLevel, size_t numSamples);

    /**
     * @brief Signal for audio frequencies change
     * @param frequencies The frequencies and their value
     * @param numSamples Number of audio samples analyzed
     */
    void frequenciesChanged(const std::complex<double>* frequencies, size_t numSamples);
};
#endif // AUDIOINPUT_H
