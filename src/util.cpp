#include "util.h"

#include <cstring>

namespace util {
    size_t bufferLength(const QAudioFormat &format, const int interval)
    {
        /*
         * Let's explain
         *
         * We want to get the number of bytes that the buffer needs to be able to store
         * sampleRate (ex: 44100, meaning 44100 per seconds) *
         * channelCount (self explanatory ie: 1 for mono, 2 for stereo) *
         * sampleSize (Number of bit in sample type, ie: 32 for float / 8 (a byte) = 4 bytes for a float)
         * So we would need, for example, a buffer of length
         * 44100 (samples) * 1 (channelCount) * 4 (sizeof(sampleType)) = 176 400 (176 400 bytes)
         *
         * But we process the buffers at 'interval' in ms, we don't need the buffer to hold all the data for the whole second
         * We can therefore divide it by the number of time the 'interval' fits in a second: 1000ms / 'interval'.
        */
        return static_cast<size_t>((format.sampleRate() * format.channelCount() * (format.sampleSize() / 8)) / (1000 / interval));
    }

    std::vector<float> charToFloatVector(const boost::circular_buffer<char>& data, size_t size)
    {
        Q_ASSERT(data.size() >= size && size % 4 == 0);
        std::vector<float> floats(size / sizeof(float));
        std::memcpy(floats.data(), &*data.begin(), size);
        return floats;
    }
}
