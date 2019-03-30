#include "audioinputthread.h"

#include <QAudioInput>

#include "util.h"

AudioInputThread::AudioInputThread(int notifyIntervalMs)
    :   notifyIntervalMs{notifyIntervalMs}
    ,   thread{new QThread}
    ,   numBytesRead{0}
    ,   tempBuffer(0)
    ,   cbuffer(0)
    ,   first_half{}
{
    moveToThread(thread);
    thread->start();
}

AudioInputThread::~AudioInputThread() {

}

void AudioInputThread::setAudioInputDevice(const QAudioDeviceInfo &audioInputDevice)
{
    // Sets the selected audio input device
    if (audioInputDevice.deviceName() != this->audioInputDevice.deviceName()) {
        this->audioInputDevice = audioInputDevice;
        initialize();
    }
}

void AudioInputThread::setFormat(const QAudioFormat &format)
{
    this->format = format;
}

bool AudioInputThread::initialize() {
    audioInput = new QAudioInput(audioInputDevice, format, this);
    audioInput->setNotifyInterval(notifyIntervalMs);

    bufferLength = util::bufferLength(format, notifyIntervalMs);
    tempBuffer.resize(bufferLength);
    cbuffer.set_capacity(bufferLength * 2);
    first_half.resize(bufferLength / 2);
    std::fill(first_half.begin(), first_half.end(), 0);

    AUDIOINPUT_DEBUG_S << "AudioInputThread::initialize" << "bufferLength" << bufferLength;

    return true;
}

void AudioInputThread::startListening() {
    if (audioInput) {
        bufferPosition = 0;
        connect(audioInput, &QAudioInput::stateChanged, this, &AudioInputThread::audioStateChanged);
        connect(audioInput, &QAudioInput::notify,  this, &AudioInputThread::audioNotify);

        audioInputIODevice = audioInput->start();
        connect(audioInputIODevice, &QIODevice::readyRead, this, &AudioInputThread::audioDataReady);
    }
}

void AudioInputThread::stopListening() {
    audioInput->stop();
    audioInput->disconnect();
    audioInputIODevice = nullptr;
    bufferPosition = 0;
}


/************************************************************/
/*      PRIVATE SLOTS                                       */
/************************************************************/
void AudioInputThread::audioNotify()
{
    // If we have gathered sampleRate / notifyInterval samples, emit data ready for analysis
    if (cbuffer.size() >= bufferLength) {
        // Converts the circular buffer of char to a vector of float (sample type)
        // TODO: Don't hardcode the type
        AUDIOINPUT_DEBUG << "bufferLength" << bufferLength << "cbuffer.size()" << cbuffer.size();

        std::vector<char> data = util::overlap(first_half, cbuffer, bufferLength);
        emit dataReady(util::charToFloatVector(data, bufferLength));

        // Keep last half of current set of samples
        std::copy(cbuffer.begin() + bufferLength / 2, cbuffer.begin() + static_cast<int>(bufferLength), first_half.begin());

        cbuffer.erase(cbuffer.begin(), cbuffer.begin() + static_cast<int>(bufferLength));
    }
}

void AudioInputThread::audioStateChanged(QAudio::State state)
{
   // AUDIOINPUT_DEBUG << "Engine::audioStateChanged" << "state" << state;

    if (QAudio::StoppedState == state) {
        // Check error
        QAudio::Error error = QAudio::NoError;
        error = audioInput->error();
        if (error != QAudio::NoError) {
            stopListening();
        //    AUDIOINPUT_DEBUG << "AudioInputThread::audioStateChanged" << "Error" << error;
            return;
        }
    }
}

/*
 * For my audio input device and format (44100, floats), this is called every 40 ms, meaning I get
 * 7056 samples each time (1000 / 40 * 7056 = 176400 bytes = 44100 * 4 bytes)
 */
void AudioInputThread::audioDataReady()
{
    size_t bytesReady = static_cast<size_t>(audioInput->bytesReady());
    size_t bytesToBeRead = std::min(bytesReady, tempBuffer.size());

    const long long bytesRead = audioInputIODevice->read(reinterpret_cast<char*>(tempBuffer.data()), static_cast<long long>(bytesToBeRead));
    cbuffer.insert(cbuffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesRead);
    AUDIOINPUT_DEBUG << "bytesRead" << bytesRead;
}
