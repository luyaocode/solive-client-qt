#include "Logger.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <fstream>
constexpr auto LOG_DIRECTORY = "logs";
constexpr auto LOG_FILE_PATH = "logs/log.txt";

namespace SoLive::Logger
{
    Logger& Logger::getInstance()
    {
        static Logger instance;
        return instance;
    }

    Logger::Logger() : _currentLevel(SeverityLevel::Trace)
    {
    }

    void Logger::setLogLevel(SeverityLevel level)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _currentLevel = level;

        // Set log level for both console and file logger
        _consoleLogger->set_level(mapSeverityLevel(level));
        _fileLogger->set_level(mapSeverityLevel(level));
    }

    void Logger::log(SeverityLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        switch (level)
        {
        case SeverityLevel::Trace:
            _consoleLogger->trace(message);
            _fileLogger->trace(message);
            break;
        case SeverityLevel::Debug:
            _consoleLogger->debug(message);
            _fileLogger->debug(message);
            break;
        case SeverityLevel::Info:
            _consoleLogger->info(message);
            _fileLogger->info(message);
            break;
        case SeverityLevel::Warning:
            _consoleLogger->warn(message);
            _fileLogger->warn(message);
            break;
        case SeverityLevel::Error:
            _consoleLogger->error(message);
            _fileLogger->error(message);
            break;
        case SeverityLevel::Fatal:
            _consoleLogger->critical(message);
            _fileLogger->critical(message);
            break;
        }
    }

    void Logger::init()
    {
        if (!std::filesystem::exists(LOG_DIRECTORY))
        {
            std::filesystem::create_directory(LOG_DIRECTORY);
        }
        std::ofstream file(LOG_FILE_PATH, std::ios::app);
        if (!file)
        {
            std::cerr << "Failed to create file: " << LOG_FILE_PATH << std::endl;
            throw std::runtime_error("File creation failed");
        }
        try
        {
            std::string strName = "spdlog_console_logger";
            _consoleLogger = spdlog::stdout_color_mt(strName);
            _consoleLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

            _fileLogger = spdlog::basic_logger_mt("basic_logger", "logs/log.txt");
            _fileLogger->flush_on(spdlog::level::info);
            _fileLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] %v");

            setLogLevel(_currentLevel);
        }
        catch (const spdlog::spdlog_ex& e)
        {
            std::cerr << "Error initializing logger: " << e.what() << std::endl;
            assert(_fileLogger);
        }
    }

    spdlog::level::level_enum Logger::mapSeverityLevel(SeverityLevel level)
    {
        switch (level)
        {
        case SeverityLevel::Trace: return spdlog::level::trace;
        case SeverityLevel::Debug: return spdlog::level::debug;
        case SeverityLevel::Info: return spdlog::level::info;
        case SeverityLevel::Warning: return spdlog::level::warn;
        case SeverityLevel::Error: return spdlog::level::err;
        case SeverityLevel::Fatal: return spdlog::level::critical;
        }
        return spdlog::level::info;
    }
}
