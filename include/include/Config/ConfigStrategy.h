#ifndef CONFIG_STRATEGY_H
#define CONFIG_STRATEGY_H

#include <string>
#include <any>

namespace SoLive::Config
{
    class ConfigStrategy
    {
    public:
        virtual ~ConfigStrategy() = default;
        virtual std::string getUri(const std::string& environment) const = 0;
        virtual std::any getValue(const std::string& key) const = 0;
        virtual void setValue(const std::string& key, const std::any& val) = 0;
    };
}

#endif // CONFIG_STRATEGY_H
