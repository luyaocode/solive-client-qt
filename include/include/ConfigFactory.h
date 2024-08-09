#ifndef CONFIG_FACTORY_H
#define CONFIG_FACTORY_H

#include "ConfigStrategy.h"
#include <memory>
#include <string>
namespace SoLive::Config
{
    class ConfigFactory
    {
    public:
        static std::unique_ptr<ConfigStrategy> createConfigStrategy(const std::string& type, const std::string& filename);
    };
}
#endif // CONFIG_FACTORY_H
