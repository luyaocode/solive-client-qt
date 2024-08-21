#ifndef JSON_CONFIG_STRATEGY_H
#define JSON_CONFIG_STRATEGY_H

#include "ConfigStrategy.h"
#include <json.hpp>

namespace SoLive::Config
{
    class JsonConfigStrategy : public ConfigStrategy
    {
    public:
        JsonConfigStrategy(const std::string& filename);

        std::string getUri(const std::string& environment) const override;
        std::any getValue(const std::string& key) const override;
        void setValue(const std::string& key, const std::any& val);
        template <typename T>
        T get(const std::string& key) const;
        template <typename T>
        void set(const std::string& key, const T& val);

    private:
        void loadConfig(const std::string& filename);
        nlohmann::json _config;
    };
}
#include "JsonConfigStrategyImpl.h"
#endif // JSON_CONFIG_STRATEGY_H
