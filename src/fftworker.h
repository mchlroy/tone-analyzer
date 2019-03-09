#ifndef FFTWORKER_H
#define FFTWORKER_H

#include <QObject>
#include <QDebug>
#include <vector>
#include <complex>
#include <fftw3.h>
#include <cstring>

class FFTWorker : public QObject
{
    Q_OBJECT
public:
    FFTWorker(const std::vector<char> &buffer) : buffer(buffer), plan_initialized(false) {
        fftw_in.resize(buffer.size() / sizeof(float));
        std::fill(fftw_in.begin(), fftw_in.end(), double{});

        fftw_out.resize(buffer.size() / sizeof(float));
        std::fill(fftw_out.begin(), fftw_out.end(), std::complex<double>(0.0 , 0.0));
    }
public slots:

    void doWork();
private:

    fftw_plan                           fftw_plan;
    const std::vector<char>             &buffer;
    std::vector<double>                 fftw_in;
    std::vector<std::complex<double>>   fftw_out;
    bool plan_initialized;
signals:
    void resultReady(const std::complex<double>* frequencies, std::size_t numSamples) const;
};

#endif // FFTWORKER_H
