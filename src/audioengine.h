#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <QObject>

#include "audioanalyzerthread.h"
#include "audioinputthread.h"

class AudioEngine : public QObject
{
enum {
    NOTIFY_INTERVAL_MS = 100, SAMPLE_RATE = 44100, SAMPLE_SIZE = 32
};

public:
    AudioEngine();

    /**
     * @brief Returns the available audio input devices on the system
     * @return List of devices @see{QAudioDeviceInfo}
     */
    const std::vector<QAudioDeviceInfo>& getAvailableAudioInputDevices() const { return availableAudioInputDevices; }

    /**
     * @brief Returns the audio analyzer thread
     * @return The analyzer thread
     */
    const AudioAnalyzerThread* getAudioAnalyzerThread() {return audioAnalyzerThread; }

    /**
     * @brief Sets the current audio input device
     * @param The index of of the audio input device as returned by QAudioDeviceInfo::availableDevices @see{QAudioDeviceInfo::availableDevices::availableDevices}
     */
    void setAudioInputDevice(size_t index);

private:
    AudioInputThread                *audioInputThread;      // The thread for the instance of the audio input class
    AudioAnalyzerThread             *audioAnalyzerThread;   // The thread for the instance of the audio analyzer class

    QAudioFormat                    format;                         // Format of the receiving audio data
    QAudioDeviceInfo                audioInputDevice;               // Currently selected audio input device
    std::vector<QAudioDeviceInfo>   availableAudioInputDevices;     // List of available audio input devices

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
     * @brief Receives the audio data sent by the instance of AudioInputThread
     * @param data
     */
    void receiveAudioData(const std::vector<float> &data);
private:
    /**
     * @brief Initializes the audio input device for listening
     * @return True if the initilization was successful
     */
    bool initialize();
};

#endif // AUDIOENGINE_H
