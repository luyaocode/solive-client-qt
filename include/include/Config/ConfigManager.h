#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "ConfigFactory.h"
#include <memory>
#include <string>
#include <any>
#include "ConfigDef.h"

namespace SoLive::Config
{
    class ConfigManager
    {
    public:
        static ConfigManager& instance();
        void loadConfig(const std::string& type, const std::string& filename);
        std::string getUri(const std::string& environment) const;
        std::any getValue(const std::string& key) const;
        void setValue(const std::string& key, const std::any& val);
    private:
        ConfigManager() = default;
        ~ConfigManager() = default;
        std::unique_ptr<ConfigStrategy> _configStrategy;
    };
}
#endif // CONFIG_MANAGER_H
