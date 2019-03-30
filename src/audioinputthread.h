#ifndef AUDIOINPUTTHREAD_H
#define AUDIOINPUTTHREAD_H

#include <vector>

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QIODevice>
#include <QThread>

#include <boost/circular_buffer.hpp>

QT_BEGIN_NAMESPACE
    class QAudioInput;
QT_END_NAMESPACE

class AudioInputThread : public QObject
{
    Q_OBJECT
public:
    AudioInputThread(int notifyIntervalMs);
    ~AudioInputThread();

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
    QThread*                        thread;             // The thread it will be running on

    int                             notifyIntervalMs;   // The interval in ms we want to process the raw data
    size_t                          numBytesRead;       // The number of bytes read since analysis
    size_t                          bufferLength;       // The length of the buffer
    size_t                          bufferPosition;     // The position in which to insert newly received data

    std::vector<char>               tempBuffer;         // Temp buffer for storing the raw data coming from audio device
    boost::circular_buffer<char>    cbuffer;            // Circular buffer container the fetched raw datas

    QAudioFormat                    format;             // Format of the receiving audio data
    QAudioDeviceInfo                audioInputDevice;   // Currently selected audio input device
    QAudioInput*                    audioInput;         // Interface for receiving audio data
    QIODevice*                      audioInputIODevice; // Interface receiving audio data (returned by audioInput->start())
    std::vector<char>               first_half;
private:
    /**
     * @brief Initializes the audio input device for listening
     * @return True if the initilization was successful
     */
    bool initialize();

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

signals:

    /**
     * @brief Signal for audio frequencies change
     * @param frequencies The frequencies and their value
     * @param numSamples Number of audio samples analyzed
     */
    void dataReady(const std::vector<float> &data);
};

#endif // AUDIOINPUTTHREAD_H
