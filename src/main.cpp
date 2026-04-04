#include "raylib.h"
#include "Game.h"

int main() {
    // 初始化窗口
    const int screenWidth = ConfigManager::GetInt("screenWidth", 800);
    const int screenHeight = ConfigManager::GetInt("screenHeight", 600);
    
    InitWindow(screenWidth, screenHeight, ConfigManager::GetString("windowTitle", "Breakout - Enhanced Edition").c_str());
    SetTargetFPS(60);
    
    // 创建游戏实例
    Game game;
    game.Initialize();
    
    // 主游戏循环
    while (!WindowShouldClose()) {
        // 处理输入
        game.ProcessInput();
        
        // 更新游戏状态
        game.Update();
        
        // 开始绘制
        BeginDrawing();
        
        // 绘制游戏
        game.Draw();
        
        // 结束绘制
        EndDrawing();
    }
    
    // 关闭窗口
    CloseWindow();
    
    return 0;
}
