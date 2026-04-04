#pragma once

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

class ConfigManager {
private:
    static std::map<std::string, std::string> configMap;
    
public:
    static bool LoadConfig(const std::string& filename);
    static std::string GetString(const std::string& key, const std::string& defaultValue = "");
    static int GetInt(const std::string& key, int defaultValue = 0);
    static float GetFloat(const std::string& key, float defaultValue = 0.0f);
    static bool GetBool(const std::string& key, bool defaultValue = false);
};
