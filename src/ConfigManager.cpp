#include "ConfigManager.h"
#include <algorithm>
#include <cctype>

std::map<std::string, std::string> ConfigManager::configMap;

bool ConfigManager::LoadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开配置文件: " << filename << std::endl;
        return false;
    }
    
    configMap.clear();
    std::string line;
    while (std::getline(file, line)) {
        // 移除注释
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // 跳过空行和只有空格的行
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        // 移除首尾空格
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // 查找键值对
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // 移除键的引号和空格
            key.erase(0, key.find_first_not_of(" \t\""));
            key.erase(key.find_last_not_of(" \t\",") + 1);
            
            // 移除值的引号和空格
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\",") + 1);
            
            configMap[key] = value;
        }
    }
    
    file.close();
    return true;
}

std::string ConfigManager::GetString(const std::string& key, const std::string& defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return it->second;
    }
    return defaultValue;
}

int ConfigManager::GetInt(const std::string& key, int defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float ConfigManager::GetFloat(const std::string& key, float defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool ConfigManager::GetBool(const std::string& key, bool defaultValue) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value == "true" || value == "1" || value == "yes");
    }
    return defaultValue;
}
