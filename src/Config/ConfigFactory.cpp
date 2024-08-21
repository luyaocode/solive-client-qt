#include "stdafx.h"
#include "ConfigFactory.h"
#include "JsonConfigStrategy.h"

namespace SoLive::Config
{
    std::unique_ptr<ConfigStrategy> ConfigFactory::createConfigStrategy(const std::string& type, const std::string& filename)
    {
        if (type == "json")
        {
            return std::make_unique<JsonConfigStrategy>(filename);
        }
        // Add more types here if needed
        throw std::runtime_error("Unknown config type: " + type);
    }
}
