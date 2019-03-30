#include "audioanalyzerthread.h"

#include <math.h>

#include <qmath.h>
#include <QThread>

#include <boost/circular_buffer.hpp>

#include "util.h"

AudioAnalyzerThread::AudioAnalyzerThread()
    : thread(new QThread(this))
    , fftw_in{}
    , fftw_out{}
    , data_out{}
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

void AudioAnalyzerThread::applyWindowingFunction() {
    double x = 0.0;

    for (size_t i=0; i<fftw_out.size(); ++i) {
        x = 0.5 * (1 - qCos((2 * M_PI * i) / (fftw_out.size())));
        fftw_out[i] = fftw_out[i] * x;
    }
}

void AudioAnalyzerThread::calculateSpectrum(const int sampleRate, const std::vector<float> &data) {
    if (!plan_initialized) {
        fftw_in.resize(data.size());
        fftw_out.resize(data.size());
        data_out.resize(data.size());
        fftw_plan = fftw_plan_dft_r2c_1d(static_cast<int>(fftw_in.size()), fftw_in.data(), reinterpret_cast<fftw_complex*>(fftw_out.data()), FFTW_BACKWARD);
        plan_initialized = true;
    }

    std::transform(data.begin(), data.end(), fftw_in.begin(), [](float x) -> double { return double(x); });
    fftw_execute(fftw_plan);

    applyWindowingFunction();

    // Transform complex numbers to floating point numbers
    std::transform(fftw_out.begin(), fftw_out.end(), data_out.begin(), [](std::complex<double> x) -> double {
        return std::sqrt(std::pow(x.real(), 2) + std::pow(x.imag(), 2));
    });

    // Crude detection of pitch, just take frequency with highest value
    // TODO: Detection of harmonics
    double hz_step = sampleRate / 2.0 / data.size() / 2.0;
    double max_freq = std::numeric_limits<double>::min();
    int index = 0;
    for (int i = 0; i < NB_NOTES; i++) {
        size_t fftw_out_index = static_cast<size_t>(frequencies[i] / hz_step);
        double v1 = data_out[fftw_out_index];
        double v2 = data_out[fftw_out_index + 1];
        double value = (v1 + v2) / 2.0;
        if (max_freq < value) {
            index = i;
            max_freq = value;
        }
    }

    // We only need half, the rest is not useful by the nature of Discrete Fourier Transform / Fast Fourier Transform
    emit frequenciesChanged(data_out.data(), data_out.size());
    emit noteChanged(notes[index]);
}
