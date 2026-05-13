#include "game.h"

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "双人打砖块 - 终极版");
    SetTargetFPS(60);
    
    Game game;
    InitGame(game);
    
    while (!WindowShouldClose()) {
        UpdateGame(game);
        DrawGame(game);
    }
    
    CloseWindow();
    return 0;
}

