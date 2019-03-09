#include "fftworker.h"

void FFTWorker::doWork() {
    // 00111111001101010011001100110011
    // 00110011001100110011010100111111
    // ? &#8220; 3 3

//    std::vector<char> test(8);

//   qDebug() << "DOING WORK";

//   if (!plan_initialized) {
//       fftw_plan = fftw_plan_dft_r2c_1d(static_cast<int>(fftw_in.size()), fftw_in.data(), reinterpret_cast<fftw_complex*>(fftw_out.data()), FFTW_BACKWARD);
//       plan_initialized = true;
//       qDebug() << "Initialized";
//   }

//   //std::vector<float> floatVec = charToFloatVector(buffer);
//   qDebug() << buffer.size();
//   qDebug() << fftw_in.size();
//   qDebug() << floatVec.size();
//   std::transform(floatVec.begin(), floatVec.begin() + static_cast<int>(floatVec.size()), fftw_in.begin(), [](float x) -> double { return double(x); });

//   fftw_execute(fftw_plan);

//   qDebug() << "Before emit";
//   emit resultReady(fftw_out.data(), buffer.size() / 4 / 2);
//   qDebug() << "After emit";
}
