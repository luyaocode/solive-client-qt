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
        std::shared_ptr<spdlog::logger> _consoleLogger;       // ϵͳ����̨��־
        std::shared_ptr<spdlog::logger> _fileLogger;          // ϵͳ�ļ���־
        std::shared_ptr<spdlog::logger> _userTrackFileLogger; // �û���Ϊ�ļ���־
    };
}

// ���� LOG ��
// ���QString��std::string��ϣ���ʹ�÷���Ϊ��
// LOG(Info,"1212"+std::string(qstr.toUtf8().constData()))
// std::stringתQString��QString::fromUtf8(str.c_str())
#define LOG(level, message) SoLive::Logger::Logger::getInstance().log(SoLive::Logger::Logger::SeverityLevel::level, message);

#endif // LOGGER_H
