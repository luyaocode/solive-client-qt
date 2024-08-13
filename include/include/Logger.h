#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <mutex>

namespace SoLive::Logger
{
    class Logger
    {
    public:
        enum class SeverityLevel
        {
            Trace,
            Debug,
            Info,
            Warning,
            Error,
            Fatal
        };
        static Logger& getInstance();
        void setLogLevel(SeverityLevel level);
        void log(SeverityLevel level, const std::string& message);
        void init();
    private:
        Logger();
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        spdlog::level::level_enum mapSeverityLevel(SeverityLevel level);
    private:
        SeverityLevel _currentLevel;
        std::mutex _mutex;
        std::shared_ptr<spdlog::logger> _consoleLogger;
        std::shared_ptr<spdlog::logger> _fileLogger;
    };
}

// ∂®“Â LOG ∫Í
#define LOG(level, message) SoLive::Logger::Logger::getInstance().log(SoLive::Logger::Logger::SeverityLevel::level, message);

#endif // LOGGER_H
