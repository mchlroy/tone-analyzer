#include "audioanalyzerthread.h"

#include <math.h>

#include <QThread>

#include <boost/circular_buffer.hpp>

#include "util.h"

AudioAnalyzerThread::AudioAnalyzerThread()
    : thread(new QThread(this))
    , cbuffer{}
    , fftw_in{}
    , fftw_out{}
    , plan_initialized{false}
{
    moveToThread(thread);
    thread->start();
}

AudioAnalyzerThread::~AudioAnalyzerThread() {}

void AudioAnalyzerThread::calculateLevel(const std::vector<float> &data) {
    float peakLevel = 0.0;
    float sum = 0.0;

    auto it = data.begin();
    auto end = data.end();

    while (it < end) {
        const float value = *it;
        peakLevel = std::max(peakLevel, value);
        sum += std::pow(value, 2.0f);
        ++it;
    }
    size_t numSamples = data.size();
    float rmsLevel = std::clamp(sqrt(sum / numSamples), 0.0f, 1.0f);

    //AUDIOANALYZER_DEBUG << "AudioInput::calculateLevel" << "rms" << rmsLevel << "peak" << peakLevel;

    emit levelChanged(rmsLevel, peakLevel, numSamples);
}

void AudioAnalyzerThread::calculateSpectrum(const std::vector<float> &data) {
    //AUDIOANALYZER_DEBUG << "AudioInput::calculateSpectrum" << "size" << data.size();

    if (!plan_initialized) {
        fftw_in.resize(data.size());
        fftw_out.resize(data.size());

        fftw_plan = fftw_plan_dft_r2c_1d(static_cast<int>(fftw_in.size()), fftw_in.data(), reinterpret_cast<fftw_complex*>(fftw_out.data()), FFTW_BACKWARD);
        plan_initialized = true;
    }

    std::transform(data.begin(), data.end(), fftw_in.begin(), [](float x) -> double { return double(x); });
    fftw_execute(fftw_plan);

    // We only need half, the rest is not useful by the nature of Discrete Fourier Transform / Fast Fourier Transform
    emit frequenciesChanged(fftw_out.data(), data.size() / 2);
}
