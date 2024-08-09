#include "JsonConfigStrategy.h"
#include <fstream>
#include <stdexcept>

namespace SoLive::Config
{
    JsonConfigStrategy::JsonConfigStrategy(const std::string& filename)
    {
        loadConfig(filename);
    }

    void JsonConfigStrategy::loadConfig(const std::string& filename)
    {
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
    }

    std::string JsonConfigStrategy::getUri(const std::string& environment) const
    {
        if (_config.contains(environment))
        {
            std::string protocol = _config[environment]["protocol"];
            std::string host = _config[environment]["host"];
            int port = _config[environment]["port"];

            return protocol + "://" + host + ":" + std::to_string(port);
        }
        else
        {
            throw std::runtime_error("Environment not found in config: " + environment);
        }
    }
}
