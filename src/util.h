#ifndef UTIL_H
#define UTIL_H

#include <QAudioFormat>
#include <QDebug>

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
    size_t bufferLength(const QAudioFormat &format, const int interval);
    std::vector<float> charToFloatVector(const boost::circular_buffer<char>& data, size_t size);
}

#endif // UTIL_H
