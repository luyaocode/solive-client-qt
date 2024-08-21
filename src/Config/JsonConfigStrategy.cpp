#include "stdafx.h"
#include "JsonConfigStrategy.h"
#include "ConfigDef.h"

namespace SoLive::Config
{
    namespace fs = std::filesystem;

    JsonConfigStrategy::JsonConfigStrategy(const std::string& filename)
    {
        loadConfig(filename);
    }

    void JsonConfigStrategy::loadConfig(const std::string& filename)
    {
        // 加载配置文件
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open config file: " + filename);
        }

        try
        {
            file >> _config;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            throw std::runtime_error("Error parsing config file: " + std::string(e.what()));
        }

        // 加载配置项
        fs::path rootPath = fs::current_path();
        std::string strRootPath = rootPath.string();
        std::replace(strRootPath.begin(), strRootPath.end(), '\\', '/');
        std::vector<std::string> paths;
        // recordSavePath
        std::string recordSavePath = strRootPath +"/" + _config[SoLive::Config::CONFIG_RECORD_SAVE_PATH].get<std::string>();
        _config[SoLive::Config::CONFIG_RECORD_SAVE_PATH] = recordSavePath;
        paths.emplace_back(recordSavePath);
        // logPath
        std::string logPath = strRootPath + "/" + _config[SoLive::Config::CONFIG_LOG_PATH].get<std::string>();
        _config[SoLive::Config::CONFIG_LOG_PATH] = logPath;
        paths.emplace_back(logPath);

        for (auto& strPath : paths)
        {
            fs::path path(strPath);
            if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
            {
                try
                {
                    fs::create_directories(path);
                }
                catch (const std::filesystem::filesystem_error& e)
                {
                    throw std::runtime_error("Failed to create directory: " + strPath);
                }
            }
        }
    }

    std::string JsonConfigStrategy::getUri(const std::string& environment) const
    {
        if (_config.contains(environment))
        {
            std::string protocol = _config[environment]["protocol"].get<std::string>();
            std::string host = _config[environment]["host"].get<std::string>();
            int port = _config[environment]["port"].get<int>();

            return protocol + "://" + host + ":" + std::to_string(port);
        }
        else
        {
            throw std::runtime_error("Environment not found in config: " + environment);
        }
    }

    std::any JsonConfigStrategy::getValue(const std::string& key) const
    {
        if (_config.contains(key))
        {
            const auto& value = _config.at(key);
            if (value.is_number_integer())
            {
                return std::any(get<int>(key));
            }
            else if (value.is_number_float())
            {
                return std::any(get<double>(key));
            }
            else if (value.is_string())
            {
                return std::any(get<std::string>(key));
            }
            else if (value.is_boolean())
            {
                return std::any(value.get<bool>());
            }
        }
        throw std::runtime_error("Key not found or unsupported type: " + key);
    }

    void JsonConfigStrategy::setValue(const std::string& key, const std::any& val)
    {
        if (val.type() == typeid(int))
        {
            set<int>(key, std::any_cast<int>(val));
        }
        else if (val.type() == typeid(double))
        {
            set<double>(key, std::any_cast<double>(val));
        }
        else if (val.type() == typeid(std::string))
        {
            set<std::string>(key, std::any_cast<std::string>(val));
        }
        else if (val.type() == typeid(bool))
        {
            set<bool>(key, std::any_cast<bool>(val));
        }
        else
        {
            throw std::runtime_error("Unsupported type");
        }
    }
}
