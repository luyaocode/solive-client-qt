#include "stdafx.h"
#include "ConfigManager.h"
#include "ConfigFactory.h"

using namespace SoLive::Config;
ConfigManager& ConfigManager::instance()
{
	static ConfigManager instance;
	return instance;
}

void ConfigManager::loadConfig(const std::string& type, const std::string& filename)
{
	_configStrategy = ConfigFactory::createConfigStrategy(type, filename);
}

std::string ConfigManager::getUri(const std::string& environment) const
{
	return _configStrategy->getUri(environment);
}

std::any ConfigManager::getValue(const std::string& key) const
{
	return _configStrategy->getValue(key);
}

void ConfigManager::setValue(const std::string& key, const std::any& val)
{
	return _configStrategy->setValue(key, val);
}