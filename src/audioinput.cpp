#include "audioinput.h"
#include "fftworker.h"
#include "util.h"

#include <algorithm>
#include <math.h>
#include <QAudioInput>
#include <QThread>

// TODO: Templating should be done to get rid of constants
const int   NOTIFY_INTERVAL_MS  = 100;      // 0.1 sec
const int   SAMPLE_RATE         = 44100;    // 44100 samples per second
const int   SAMPLE_SIZE         = 32;       // Should be number of bit in sample type (ex: float -> 4 bytes -> 32 bits)

AudioInput::AudioInput()
    :   numBytesRead{0}
    ,   tempBuffer(0)
    ,   data(0)
    ,   cbuffer(0)
    ,   dbuffer(0)
    ,   plan_initialized(false)
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

    AUDIOINPUT_DEBUG << "AudioInput::AudioInput" << "Available audio input devices" << availableAudioInputDevices.size();
}

void AudioInput::setAudioInputDevice(const QAudioDeviceInfo &audioInputDevice)
{
    // Sets the selected audio input device
    if (audioInputDevice.deviceName() != this->audioInputDevice.deviceName()) {
        this->audioInputDevice = audioInputDevice;
        initialize();
    }
}

void AudioInput::setFormat(const QAudioFormat &format)
{
    //const bool changed = (format != this->format);
    this->format = format;
}

bool AudioInput::initialize() {
    audioInput = new QAudioInput(audioInputDevice, format, this);
    audioInput->setNotifyInterval(NOTIFY_INTERVAL_MS);

    bufferLength = util::bufferLength(format, NOTIFY_INTERVAL_MS);
    qbuffer = QAudioBuffer(static_cast<int>(bufferLength), format, -1);

    tempBuffer.resize(bufferLength);
    std::fill(tempBuffer.begin(), tempBuffer.end(), 0);

    cbuffer.set_capacity(bufferLength * 2);
    dbuffer.resize(bufferLength * 2);

    fftw_in.resize(bufferLength / sizeof(float));
    std::fill(fftw_in.begin(), fftw_in.end(), double{});

    fftw_out.resize(bufferLength / sizeof(float));
    std::fill(fftw_out.begin(), fftw_out.end(), std::complex<double>(0.0 , 0.0));

    data.resize(bufferLength / sizeof(float));

    plan_initialized = false;

    AUDIOINPUT_DEBUG << "AudioInput::initialize" << "bufferLength" << bufferLength;

    return true;
}

void AudioInput::startListening() {
    if (audioInput) {
        bufferPosition = 0;
        connect(audioInput, &QAudioInput::stateChanged, this, &AudioInput::audioStateChanged);
        connect(audioInput, &QAudioInput::notify,  this, &AudioInput::audioNotify);
        audioInputIODevice = audioInput->start();
        pre = std::chrono::high_resolution_clock::now();
        connect(audioInputIODevice, &QIODevice::readyRead, this, &AudioInput::audioDataReady);
    }
}

void AudioInput::stopListening() {
    audioInput->stop();
    audioInput->disconnect();
    audioInputIODevice = nullptr;
    bufferPosition = 0;
}

void AudioInput::calculateWaveForm() {
    //auto floatVec = util::charToFloatVector(tempBuffer);

    size_t numSamples = numBytesRead / 8 / sizeof(float);
    //emit waveFormChanged(floatVec, numSamples, NOTIFY_INTERVAL_MS);
}

// We assume that the type of samples is float
// TODO: templating, could be in a separate class with various other audio calculations
void AudioInput::calculateLevel() {
    //Q_ASSERT(numBytesRead <= cbuffer.size());

    double peakLevel = 0.0;
    double sum = 0.0;

    //const char * ptr = qbuffer.data<char>();
//    const char * ptr = tempBuffer.data();
//    const char *const end = ptr + numBytesRead;

//    // Calculate Root Mean Square
//    while (ptr < end) {
//        const float value = *reinterpret_cast<const float*>(ptr);
//        const double dblValue = static_cast<double>(value);
//        peakLevel = std::max(peakLevel, dblValue);
//        sum += std::pow(dblValue, 2);
//        ptr+=4;
//    }
//    size_t numSamples = numBytesRead / 8 / sizeof(float);
//    double rmsLevel = std::clamp(sqrt(sum / numSamples), 0.0, 1.0);


    auto it = data.begin();
    auto end = data.end();

    while (it < end) {
        const float value = *it;
        const double dblValue = static_cast<double>(value);
        peakLevel = std::max(peakLevel, dblValue);
        sum += std::pow(dblValue, 2);
        ++it;
    }
    size_t numSamples = numBytesRead / 8 / sizeof(float);
    double rmsLevel = std::clamp(sqrt(sum / numSamples), 0.0, 1.0);

    AUDIOINPUT_DEBUG_S << "AudioInput::calculateLevel" << "rms" << rmsLevel << "peak" << peakLevel;

    emit levelChanged(rmsLevel, peakLevel, numSamples);
}

// We assume that the type of samples is float
// TODO: templating, could be in a separate class with various other audio calculations
void AudioInput::calculateSpectrum() {
    Q_ASSERT(numBytesRead <= bufferLength);

    std::vector<float> doubleVec(tempBuffer.begin(), tempBuffer.end());
    std::transform(doubleVec.begin(), doubleVec.begin() + static_cast<int>(numBytesRead), fftw_in.begin(), [](float x) -> double { return double(x); });

    fftw_execute(fftw_plan);
    emit frequenciesChanged(fftw_out.data(), numBytesRead / 4 / 2 + 1);
}

/************************************************************/
/*      PRIVATE SLOTS                                       */
/************************************************************/
void AudioInput::audioNotify()
{
    AUDIOINPUT_DEBUG_S << "AudioInput::Notify" << "processUSecs" << audioInput->processedUSecs() << "numBytesRead" << numBytesRead;

    if (numBytesRead == 0) return;

    if (!plan_initialized) {
        fftw_plan = fftw_plan_dft_r2c_1d(static_cast<int>(fftw_in.size()), fftw_in.data(), reinterpret_cast<fftw_complex*>(fftw_out.data()), FFTW_BACKWARD);
        plan_initialized = true;
    }



//    calculateWaveForm();
    calculateLevel();
//    calculateSpectrum();

    if (cbuffer.end() - cbuffer.begin() >= static_cast<int>(bufferLength)) {
        data = util::charToFloatVector(cbuffer, bufferLength);
        cbuffer.erase(cbuffer.begin(), cbuffer.begin() + static_cast<int>(bufferLength));
    }

    bufferPosition = 0;
    numBytesRead = 0;
}

void AudioInput::handleFFTResults(const std::complex<double>* frequencies, size_t numSamples) {
    qDebug() << "OH";
    emit frequenciesChanged(frequencies, numSamples);
}

void AudioInput::audioStateChanged(QAudio::State state)
{
    AUDIOINPUT_DEBUG << "Engine::audioStateChanged" << "state" << state;

    if (QAudio::StoppedState == state) {
        // Check error
        QAudio::Error error = QAudio::NoError;
        error = audioInput->error();
        if (error != QAudio::NoError) {
            stopListening();
            AUDIOINPUT_DEBUG << "AudioInput::audioStateChanged" << "Error" << error;
            return;
        }
    }
}

/*
 * For my audio input device and format (44100, floats), this is called every 40 ms, meaning I get
 * 7056 samples each time (1000 / 40 * 7056 = 176400 bytes = 44100 * 4 bytes)
 * We process the data in audioNotify at NOTIFY_INTERVAL_MS in the same thread,
 * which take a bit more time and causes audioDataReady to be called later with more
 * samples ready which in turn causes a need for the buffer to be bigger.
 * TODO: see if this affects the datas and if using a different thread and biffer circular buffer would
 * be a good idea
 */
void AudioInput::audioDataReady()
{
    Q_ASSERT(bufferPosition < tempBuffer.size());

    post = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(post-pre);
    pre = post;

    size_t bytesReady = static_cast<size_t>(audioInput->bytesReady());
    size_t bytesToBeRead = std::min(bytesReady, tempBuffer.size() - bufferPosition);

    //const long long bytesRead = audioInputIODevice->read(qbuffer.data<char>() + bufferPosition, static_cast<long long>(bytesToBeRead));
    const long long bytesRead = audioInputIODevice->read(reinterpret_cast<char*>(tempBuffer.data()), static_cast<long long>(bytesToBeRead));
    //AUDIOINPUT_DEBUG_S << "AudioInput::audioDataReady" << "(" << diff.count() << ") " << bytesReady << bytesToBeRead << bytesRead;

    numBytesRead += static_cast<size_t>(bytesRead);
    //bufferPosition += static_cast<size_t>(bytesRead);

    //dbuffer.insert(dbuffer.end(), buffer.begin(), buffer.end());
    cbuffer.insert(cbuffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesRead);
    AUDIOINPUT_DEBUG_S << "Assigning" << "(" << diff.count() << ") " << bytesRead << " | " << cbuffer.size();
    //
}
