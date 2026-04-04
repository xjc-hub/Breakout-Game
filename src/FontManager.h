#pragma once

#include "raylib.h"
#include <string>

class FontManager {
private:
    Font chineseFont;
    bool fontLoaded;
    
    // 私有构造函数（单例模式）
    FontManager();
    
public:
    // 获取单例实例
    static FontManager& GetInstance();
    
    // 禁止拷贝
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    
    // 初始化字体
    bool LoadChineseFont();
    
    // 绘制函数
    void DrawChineseText(const char* text, int x, int y, int fontSize, Color color);
    void DrawChineseTextCentered(const char* text, int y, int fontSize, Color color);
    
    // 获取字体状态
    bool IsFontLoaded() const { return fontLoaded; }
    Font GetFont() const { return chineseFont; }
};
