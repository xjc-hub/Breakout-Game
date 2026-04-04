#include "src/ConfigManager.h"
#include <iostream>

int main() {
    if (ConfigManager::LoadConfig("config.json")) {
        std::cout << "配置加载成功！" << std::endl;
        std::cout << "屏幕宽度: " << ConfigManager::GetInt("screenWidth") << std::endl;
        std::cout << "屏幕高度: " << ConfigManager::GetInt("screenHeight") << std::endl;
        std::cout << "初始生命: " << ConfigManager::GetInt("initialLives") << std::endl;
        std::cout << "窗口标题: " << ConfigManager::GetString("windowTitle") << std::endl;
    } else {
        std::cout << "配置加载失败！" << std::endl;
    }
    return 0;
}
