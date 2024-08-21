#include "JsonConfigStrategy.h"

namespace SoLive::Config
{
    template <typename T>
    T JsonConfigStrategy::get(const std::string& key) const
    {
        if (_config.contains(key))
        {
            const auto& value = _config.at(key);
            return value.get<T>();
        }
        throw std::runtime_error("Key not found or unsupported type: " + key);
    }

    template <typename T>
    void JsonConfigStrategy::set(const std::string& key, const T& val)
    {
        try
        {
            _config[key] = val;
        }
        catch (const std::runtime_error& err)
        {
            throw std::runtime_error("Key not found or unsupported type: " + key);
        }
    }
}