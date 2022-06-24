#ifndef BAIZE_LOGGER_H
#define BAIZE_LOGGER_H

#include "util/noncopyable.h"

#include <iostream>

namespace baize
{

namespace log
{

class Logger: util::noncopyable
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    Logger(const char* filename, int line, const char* func, LogLevel level, int savedErrno);
    ~Logger();

    std::ostream& stream() { return logStream_; }

    static LogLevel getLogLevel() { return logLevel_; };
    static void setLogLevel(LogLevel level) { logLevel_ = level; };
private:
    static LogLevel logLevel_; 

    const char* filename_;
    int line_;
    const char* func_;
    LogLevel level_;

    // todo: do not use cout
    std::ostream& logStream_; 
};


} //namespace log

#define LOG_TRACE if (log::Logger::LogLevel::TRACE >= log::Logger::getLogLevel()) \
    log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::TRACE, 0).stream()
#define LOG_DEBUG if (log::Logger::LogLevel::DEBUG >= log::Logger::getLogLevel()) \
    log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::DEBUG, 0).stream()
#define LOG_INFO if (log::Logger::LogLevel::INFO >= log::Logger::getLogLevel()) \
    log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::INFO, 0).stream()
#define LOG_WARN log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::WARN, 0).stream()
#define LOG_ERROR log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::ERROR, 0).stream()
#define LOG_FATAL log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::FATAL, 0).stream()
#define LOG_SYSERR log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::ERROR, errno).stream()
#define LOG_SYSFATAL log::Logger(__FILE__, __LINE__, __func__, log::Logger::LogLevel::FATAL, errno).stream()

} //namespace baize

#endif //BAIZE_LOGGER_H