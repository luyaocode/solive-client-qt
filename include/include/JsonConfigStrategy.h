#ifndef JSON_CONFIG_STRATEGY_H
#define JSON_CONFIG_STRATEGY_H

#include "ConfigStrategy.h"
#include <nlohmann/json.hpp>

namespace SoLive::Config
{
    class JsonConfigStrategy : public ConfigStrategy
    {
    public:
        JsonConfigStrategy(const std::string& filename);
        std::string getUri(const std::string& environment) const override;

    private:
        void loadConfig(const std::string& filename);
        nlohmann::json _config;
    };
}

#endif // JSON_CONFIG_STRATEGY_H
