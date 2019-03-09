#ifndef UTILS_H
#define UTILS_H

#include <cstring>

#include <QDebug>
#include <QAudioFormat>

#include <boost/circular_buffer.hpp>

class NullDebug
{
public:
    template <typename T>
    NullDebug& operator<<(const T&) { return *this; }
};

inline NullDebug nullDebug() { return NullDebug(); }

// Audio engine debug messages
#ifdef LOG_AUDIOENGINE
#   define AUDIOENGINE_DEBUG qDebug()
#else
#   define AUDIOENGINE_DEBUG nullDebug()
#endif

// Audio analyzer debug messages
#ifdef LOG_AUDIOANALYZER
#   define AUDIOANALYZER_DEBUG qDebug()
#else
#   define AUDIOANALYZER_DEBUG nullDebug()
#endif

// Audio input debug messages
#ifdef LOG_AUDIOINPUT
#   define AUDIOINPUT_DEBUG qDebug()
#else
#   define AUDIOINPUT_DEBUG nullDebug()
#endif

// Audio input debug messages (Silly)
#ifdef LOG_AUDIOINPUT_S
#   define AUDIOINPUT_DEBUG_S qDebug()
#else
#   define AUDIOINPUT_DEBUG_S nullDebug()
#endif

namespace util {
    /**
     * @brief Returns the length the buffer should have to store all the samples for the interval
     * of time (milliseconds) for the format
     * @param format The format of the audio
     * @param interval The interval of time in ms at which the buffer will be processed and therefore
     * in which it needs to be able to store all the data for that interval (TODO: let's find a way to better explain this)
     * @return The size the buffer should have
     */
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
         * We can therefore divide it by the number of time the 'interval' fits in a second: 1000ms / 'interval'. We multiply by 2.5
         * to add some padding in case we are too slow to process (TODO: different thread? circular buffer?)
        */
        return static_cast<size_t>((format.sampleRate() * format.channelCount() * (format.sampleSize() / 8)) / (1000 / interval) );
    }

    std::vector<float> charToFloatVector(const boost::circular_buffer<char>& data)
    {
        Q_ASSERT(data.size() % 4 == 0);
        std::vector<float> floats(data.size() / 4);
        std::memcpy(floats.data(), &*data.begin(), floats.size() * sizeof(float));
        return floats;
    }
}

#endif // UTILS_H
