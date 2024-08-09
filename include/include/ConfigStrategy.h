#ifndef CONFIG_STRATEGY_H
#define CONFIG_STRATEGY_H

#include <string>

namespace SoLive::Config
{
    class ConfigStrategy
    {
    public:
        virtual ~ConfigStrategy() = default;
        virtual std::string getUri(const std::string& environment) const = 0;
    };
}

#endif // CONFIG_STRATEGY_H
