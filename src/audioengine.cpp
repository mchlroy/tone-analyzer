#include "audioengine.h"

#include "util.h"

AudioEngine::AudioEngine()
    : audioInputThread(new AudioInputThread(NOTIFY_INTERVAL_MS))
    , audioAnalyzerThread(new AudioAnalyzerThread())
{
    // Initializes the audio format
    format = QAudioFormat();
    format.setSampleRate(SAMPLE_RATE);
    format.setByteOrder(QAudioFormat::Endian::LittleEndian);
    format.setSampleType(QAudioFormat::Float);
    format.setSampleSize(SAMPLE_SIZE);
    format.setCodec("audio/pcm");
    format.setChannelCount(1);

    // Fetched the list of available audio input devices
    QList<QAudioDeviceInfo> audioDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    std::for_each(audioDevices.begin(), audioDevices.end(), [&](QAudioDeviceInfo audioDevice) {
        if (audioDevice.isFormatSupported(format)) {
            this->availableAudioInputDevices.push_back(audioDevice);
        }
    });

    connect(audioInputThread, &AudioInputThread::dataReady, this, &AudioEngine::receiveAudioData);
}

void AudioEngine::setAudioInputDevice(size_t index) {
    Q_ASSERT(index < availableAudioInputDevices.size());

    // Sets the selected audio input device
    if (audioInputDevice.deviceName() != availableAudioInputDevices.at(index).deviceName()) {
        audioInputDevice = availableAudioInputDevices.at(index);
        initialize();
    }
}

bool AudioEngine::initialize() {
    audioInputThread->setFormat(format);
    audioInputThread->setAudioInputDevice(audioInputDevice);

    AUDIOENGINE_DEBUG << "AudioEngine::initialize" << "device" << audioInputDevice.deviceName();
    AUDIOENGINE_DEBUG << "AudioEngine::initialize" << "format" << format;
    AUDIOENGINE_DEBUG << "AudioEngine::initialize" << "notifyIntervalMs" << NOTIFY_INTERVAL_MS;

    return true;
}


/************************************************************/
/*      PUBLIC SLOTS                                        */
/************************************************************/
void AudioEngine::startListening() {
    audioInputThread->startListening();
}

void AudioEngine::stopListening() {
    audioInputThread->stopListening();
}

void AudioEngine::receiveAudioData(const std::vector<float> &data){
    //AUDIOENGINE_DEBUG << "AudioEngine::receiveAudioData" << "size" << data.size();

    audioAnalyzerThread->calculateLevel(data);
    audioAnalyzerThread->calculateSpectrum(SAMPLE_RATE, data);
}
