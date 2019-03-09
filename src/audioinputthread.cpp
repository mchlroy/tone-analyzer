#include "audioinputthread.h"

#include <QAudioInput>

#include "util.h"

AudioInputThread::AudioInputThread(int notifyIntervalMs)
    :   notifyIntervalMs{notifyIntervalMs}
    ,   thread{new QThread}
    ,   numBytesRead{0}
    ,   tempBuffer(0)
    ,   cbuffer(0)
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
        emit dataReady(util::charToFloatVector(cbuffer, bufferLength));

        // Done with these data, free space
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
 * We process the data in audioNotify at notify_interval_ms in the same thread,
 * which take a bit more time and causes audioDataReady to be called later with more
 * samples ready which in turn causes a need for the buffer to be bigger.
 * TODO: see if this affects the datas and if using a different thread and biffer circular buffer would
 * be a good idea
 */
void AudioInputThread::audioDataReady()
{
    Q_ASSERT(bufferPosition < tempBuffer.size());

    size_t bytesReady = static_cast<size_t>(audioInput->bytesReady());
    size_t bytesToBeRead = std::min(bytesReady, tempBuffer.size() - bufferPosition);

    const long long bytesRead = audioInputIODevice->read(reinterpret_cast<char*>(tempBuffer.data()), static_cast<long long>(bytesToBeRead));
    cbuffer.insert(cbuffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesRead);
}
