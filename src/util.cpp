#include "util.h"

#include <cstring>
#include <QDebug>
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

    std::vector<float> charToFloatVector(const std::vector<char>& data, size_t size)
    {
        Q_ASSERT(data.size() >= size && size % 4 == 0);
        std::vector<float> floats(size / sizeof(float));
        std::memcpy(floats.data(), &*data.begin(), size);
        return floats;
    }

    std::vector<float> charToFloatVector(const boost::circular_buffer<char>& data, size_t size)
    {
        Q_ASSERT(data.size() >= size && size % 4 == 0);
        std::vector<float> floats(size / sizeof(float));
        std::memcpy(floats.data(), &*data.begin(), size);
        return floats;
    }

    std::vector<float> charToFloatVector(const boost::circular_buffer<char>& data, size_t start, size_t size)
    {
        Q_ASSERT(data.size() >= size && size % 4 == 0);
        std::vector<float> floats(size / sizeof(float));
        std::memcpy(floats.data(), &*(data.begin() + static_cast<int>(start)), size);
        return floats;
    }

    std::vector<char> overlap(std::vector<char> first_half, const boost::circular_buffer<char>& rest, size_t size)
    {
        Q_ASSERT(rest.size() >= size / 2);
        Q_ASSERT(first_half.size() == size / 2);

        std::vector<char> overlapped(size);
        std::memcpy(overlapped.data(), first_half.data(), size / 2);
        std::memcpy(overlapped.data() + (size / 2), &*rest.begin(), size / 2);
        return overlapped;
    }
}
