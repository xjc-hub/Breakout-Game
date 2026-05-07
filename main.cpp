#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
// 道具类型
enum PowerUpType { SPEED_UP, SPEED_DOWN, WIDE_PADDLE, MULTI_BALL };
struct Brick {
    Rectangle rect;
    Color color;
    bool active;
    bool hasPowerUp;
    PowerUpType powerUpType;
};
struct PowerUp {
    Rectangle rect;
    PowerUpType type;
    bool active;
    float speed;
};
struct Particle {
    Vector2 pos;
    Vector2 speed;
    Color color;
    int life;
};
struct Paddle {
    Rectangle rect;
    int score;
    int lives;
};
struct Ball {
    Vector2 pos;
    Vector2 speed;
    float radius;
    bool active;
};
int main() {
    const int screenWidth = 1000;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Double Breakout Deluxe");
    SetTargetFPS(60);
    srand(time(0));
    // 左右分屏参数
    const int leftMargin = 10;
    const int rightMargin = 10;
    const int midMargin = 20;
    const int playWidth = (screenWidth - leftMargin - rightMargin - midMargin) / 2;
    const int topMargin = 80;
    const int bottomMargin = 50;
    int p1Left = leftMargin;
    int p1Right = leftMargin + playWidth;
    int p2Left = leftMargin + playWidth + midMargin;
    int p2Right = screenWidth - rightMargin;
    // 游戏状态
    int gamePhase = 0; // 0=倒计时, 1=游戏中, 2=游戏结束
    int countdown = 3;
    float countdownTimer = 0;
    // 玩家1
    Paddle paddle1 = { { (float)(p1Left + playWidth/2 - 50), (float)(screenHeight - 40), 100, 10 }, 0, 3 };
    Ball ball1 = { { (float)(p1Left + playWidth/2), (float)(screenHeight * 0.7f) }, { 0, 0 }, 8, true };
    std::vector<Brick> bricks1;
    std::vector<PowerUp> powerUps1;
    std::vector<Particle> particles1;
    // 玩家2
    Paddle paddle2 = { { (float)(p2Left + playWidth/2 - 50), (float)(screenHeight - 40), 100, 10 }, 0, 3 };
    Ball ball2 = { { (float)(p2Left + playWidth/2), (float)(screenHeight * 0.7f) }, { 0, 0 }, 8, true };
    std::vector<Brick> bricks2;
    std::vector<PowerUp> powerUps2;
    std::vector<Particle> particles2;
    int level = 1;
    // 生成砖块
    auto GenerateBricks = [&](std::vector<Brick>& bricks, int left, int width, Color baseColor) {
        bricks.clear();
        int rows = 4 + level;
        int cols = 6;
        float bw = (width - 20) / cols;
        Color colors[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, SKYBLUE };
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                Brick brick;
                brick.rect = { (float)(left + 10 + col * bw), (float)(topMargin + 10 + row * 25), bw - 4, 20 };
                brick.color = colors[row % 8];
                brick.active = true;
                // 每5个砖块中约1个有道具
                brick.hasPowerUp = (rand() % 5 == 0);
                brick.powerUpType = (PowerUpType)(rand() % 4);
                bricks.push_back(brick);
            }
        }
    };
    GenerateBricks(bricks1, p1Left, playWidth, BLUE);
    GenerateBricks(bricks2, p2Left, playWidth, RED);
    // 音频初始化 - 使用简单音调代替（避免需要音频文件）
    InitAudioDevice();
    Sound hitSound = LoadSoundFromWave(GenSoundWave(1000, 0.1f, 440.0f));
    while (!WindowShouldClose()) {
        // === 更新 ===
        // 倒计时阶段
        if (gamePhase == 0) {
            countdownTimer += GetFrameTime();
            if (countdownTimer > 1.0f) {
                countdown--;
                countdownTimer = 0;
                if (countdown < 0) {
                    gamePhase = 1;
                    ball1.speed = { 4, -4 };
                    ball2.speed = { -4, -4 };
                }
            }
        }
        if (gamePhase == 1) {
            // 玩家1控制
            if (IsKeyDown(KEY_A)) paddle1.rect.x -= 5;
            if (IsKeyDown(KEY_D)) paddle1.rect.x += 5;
            if (paddle1.rect.x < p1Left) paddle1.rect.x = p1Left;
            if (paddle1.rect.x + paddle1.rect.width > p1Right) paddle1.rect.x = p1Right - paddle1.rect.width;
            if (IsKeyDown(KEY_LEFT)) paddle2.rect.x -= 5;
            if (IsKeyDown(KEY_RIGHT)) paddle2.rect.x += 5;
            if (paddle2.rect.x < p2Left) paddle2.rect.x = p2Left;
            if (paddle2.rect.x + paddle2.rect.width > p2Right) paddle2.rect.x = p2Right - paddle2.rect.width;
            // 更新球1
            ball1.pos.x += ball1.speed.x;
            ball1.pos.y += ball1.speed.y;
            if (ball1.pos.x + ball1.radius > p1Right || ball1.pos.x - ball1.radius < p1Left)
                ball1.speed.x *= -1;
            if (ball1.pos.y - ball1.radius < topMargin)
                ball1.speed.y *= -1;
            if (CheckCollisionCircleRec(ball1.pos, ball1.radius, paddle1.rect) && ball1.speed.y > 0) {
                ball1.speed.y *= -1;
                PlaySound(hitSound);
            }
            for (auto &brick : bricks1) {
                if (brick.active && CheckCollisionCircleRec(ball1.pos, ball1.radius, brick.rect)) {
                    brick.active = false;
                    ball1.speed.y *= -1;
                    paddle1.score += 10;
                    PlaySound(hitSound);
                    // 粒子效果
                    for (int i = 0; i < 8; i++) {
                        particles1.push_back({ 
                            { brick.rect.x + brick.rect.width/2, brick.rect.y + brick.rect.height/2 },
                            { (float)(rand() % 6 - 3), (float)(rand() % 6 - 3) },
                            brick.color, 30 
                        });
                    }
                    // 生成道具
                    if (brick.hasPowerUp) {
                        PowerUp pu;
                        pu.rect = { brick.rect.x + brick.rect.width/2 - 10, brick.rect.y, 20, 20 };
                        pu.type = brick.powerUpType;
                        pu.active = true;
                        pu.speed = 2;
                        powerUps1.push_back(pu);
                    }
                    break;
                }
            }
            if (ball1.pos.y > screenHeight) {
                paddle1.lives--;
                if (paddle1.lives <= 0) {
                    gamePhase = 2;
                } else {
                    ball1.pos = { (float)(p1Left + playWidth/2), (float)(screenHeight * 0.7f) };
                    ball1.speed = { 0, 0 };
                    gamePhase = 0;
                    countdown = 3;
                    countdownTimer = 0;
                }
            }
            // 更新球2
            ball2.pos.x += ball2.speed.x;
            ball2.pos.y += ball2.speed.y;
            if (ball2.pos.x + ball2.radius > p2Right || ball2.pos.x - ball2.radius < p2Left)
                ball2.speed.x *= -1;
            if (ball2.pos.y - ball2.radius < topMargin)
                ball2.speed.y *= -1;
            if (CheckCollisionCircleRec(ball2.pos, ball2.radius, paddle2.rect) && ball2.speed.y > 0) {
                ball2.speed.y *= -1;
                PlaySound(hitSound);
            }
            for (auto &brick : bricks2) {
                if (brick.active && CheckCollisionCircleRec(ball2.pos, ball2.radius, brick.rect)) {
                    brick.active = false;
                    ball2.speed.y *= -1;
                    paddle2.score += 10;
                    PlaySound(hitSound);
                    for (int i = 0; i < 8; i++) {
                        particles2.push_back({ 
                            { brick.rect.x + brick.rect.width/2, brick.rect.y + brick.rect.height/2 },
                            { (float)(rand() % 6 - 3), (float)(rand() % 6 - 3) },
                            brick.color, 30 
                        });
                    }
                    if (brick.hasPowerUp) {
                        PowerUp pu;
                        pu.rect = { brick.rect.x + brick.rect.width/2 - 10, brick.rect.y, 20, 20 };
                        pu.type = brick.powerUpType;
                        pu.active = true;
                        pu.speed = 2;
                        powerUps2.push_back(pu);
                    }
                    break;
                }
            }
            if (ball2.pos.y > screenHeight) {
                paddle2.lives--;
                if (paddle2.lives <= 0) {
                    gamePhase = 2;
                } else {
                    ball2.pos = { (float)(p2Left + playWidth/2), (float)(screenHeight * 0.7f) };
                    ball2.speed = { 0, 0 };
                    gamePhase = 0;
                    countdown = 3;
                    countdownTimer = 0;
                }
            }
            // 更新道具
            for (auto &pu : powerUps1) {
                if (pu.active) {
                    pu.rect.y += pu.speed;
                    if (CheckCollisionRecs(pu.rect, paddle1.rect)) {
                        pu.active = false;
                        switch(pu.type) {
                            case SPEED_UP: ball1.speed.x *= 1.2f; ball1.speed.y *= 1.2f; break;
                            case SPEED_DOWN: ball1.speed.x *= 0.8f; ball1.speed.y *= 0.8f; break;
                            case WIDE_PADDLE: paddle1.rect.width = 150; break;
                            case MULTI_BALL: break;
                        }
                    }
                }
            }
            for (auto &pu : powerUps2) {
                if (pu.active) {
                    pu.rect.y += pu.speed;
                    if (CheckCollisionRecs(pu.rect, paddle2.rect)) {
                        pu.active = false;
                        switch(pu.type) {
                            case SPEED_UP: ball2.speed.x *= 1.2f; ball2.speed.y *= 1.2f; break;
                            case SPEED_DOWN: ball2.speed.x *= 0.8f; ball2.speed.y *= 0.8f; break;
                            case WIDE_PADDLE: paddle2.rect.width = 150; break;
                            case MULTI_BALL: break;
                        }
                    }
                }
            }
            // 更新粒子
            for (auto &p : particles1) {
                p.pos.x += p.speed.x;
                p.pos.y += p.speed.y;
                p.speed.y += 0.2f;
                p.life--;
            }
            particles1.erase(std::remove_if(particles1.begin(), particles1.end(), 
                [](Particle &p) { return p.life <= 0; }), particles1.end());
            for (auto &p : particles2) {
                p.pos.x += p.speed.x;
                p.pos.y += p.speed.y;
                p.speed.y += 0.2f;
                p.life--;
            }
            particles2.erase(std::remove_if(particles2.begin(), particles2.end(), 
                [](Particle &p) { return p.life <= 0; }), particles2.end());
        }
        if (gamePhase == 2) {
            if (IsKeyPressed(KEY_R)) {
                // 重置游戏
                paddle1 = { { (float)(p1Left + playWidth/2 - 50), (float)(screenHeight - 40), 100, 10 }, 0, 3 };
                ball1 = { { (float)(p1Left + playWidth/2), (float)(screenHeight * 0.7f) }, { 0, 0 }, 8, true };
                paddle2 = { { (float)(p2Left + playWidth/2 - 50), (float)(screenHeight - 40), 100, 10 }, 0, 3 };
                ball2 = { { (float)(p2Left + playWidth/2), (float)(screenHeight * 0.7f) }, { 0, 0 }, 8, true };
                powerUps1.clear();
                powerUps2.clear();
                particles1.clear();
                particles2.clear();
                level = 1;
                GenerateBricks(bricks1, p1Left, playWidth, BLUE);
                GenerateBricks(bricks2, p2Left, playWidth, RED);
                gamePhase = 0;
                countdown = 3;
                countdownTimer = 0;
            }
        }
        // === 绘制 ===
        BeginDrawing();
        ClearBackground(BLACK);
        // 绘制分隔线
        int midX = leftMargin + playWidth + midMargin/2;
        DrawLine(midX, 0, midX, screenHeight, Color{ 50, 50, 50, 255 });
        // 玩家1区域
        DrawRectangleLines(p1Left, topMargin, p1Right - p1Left, screenHeight - topMargin - bottomMargin, BLUE);
        DrawText("Player 1", p1Left + 10, 10, 25, BLUE);
        DrawText(TextFormat("Score: %i", paddle1.score), p1Left + 10, 35, 20, WHITE);
        DrawText(TextFormat("Lives: %i", paddle1.lives), p1Left + 10, 55, 15, GREEN);
        DrawText("[A/D] Move", p1Left + 10, screenHeight - 30, 15, GRAY);
        for (auto &brick : bricks1) {
            if (brick.active) {
                DrawRectangleRec(brick.rect, brick.color);
                DrawRectangleLinesEx(brick.rect, 1, WHITE);
                if (brick.hasPowerUp) {
                    DrawCircle(brick.rect.x + brick.rect.width/2, brick.rect.y + brick.rect.height/2, 3, GOLD);
                }
            }
        }
        for (auto &p : particles1) {
            DrawCircle(p.pos.x, p.pos.y, 3, Color{ p.color.r, p.color.g, p.color.b, (unsigned char)(p.life * 8) });
        }
        for (auto &pu : powerUps1) {
            if (pu.active) {
                Color puColor;
                switch(pu.type) {
                    case SPEED_UP: puColor = RED; break;
                    case SPEED_DOWN: puColor = BLUE; break;
                    case WIDE_PADDLE: puColor = GREEN; break;
                    case MULTI_BALL: puColor = YELLOW; break;
                }
                DrawRectangleRec(pu.rect, puColor);
            }
        }
        DrawRectangleRec(paddle1.rect, BLUE);
        DrawCircleV(ball1.pos, ball1.radius, WHITE);
        // 玩家2区域
        DrawRectangleLines(p2Left, topMargin, p2Right - p2Left, screenHeight - topMargin - bottomMargin, RED);
        DrawText("Player 2", p2Left + 10, 10, 25, RED);
        DrawText(TextFormat("Score: %i", paddle2.score), p2Left + 10, 35, 20, WHITE);
        DrawText(TextFormat("Lives: %i", paddle2.lives), p2Left + 10, 55, 15, GREEN);
        DrawText("[<- / ->] Move", p2Left + 10, screenHeight - 30, 15, GRAY);
        for (auto &brick : bricks2) {
            if (brick.active) {
                DrawRectangleRec(brick.rect, brick.color);
                DrawRectangleLinesEx(brick.rect, 1, WHITE);
                if (brick.hasPowerUp) {
                    DrawCircle(brick.rect.x + brick.rect.width/2, brick.rect.y + brick.rect.height/2, 3, GOLD);
                }
            }
        }
        for (auto &p : particles2) {
            DrawCircle(p.pos.x, p.pos.y, 3, Color{ p.color.r, p.color.g, p.color.b, (unsigned char)(p.life * 8) });
        }
        for (auto &pu : powerUps2) {
            if (pu.active) {
                Color puColor;
                switch(pu.type) {
                    case SPEED_UP: puColor = RED; break;
                    case SPEED_DOWN: puColor = BLUE; break;
                    case WIDE_PADDLE: puColor = GREEN; break;
                    case MULTI_BALL: puColor = YELLOW; break;
                }
                DrawRectangleRec(pu.rect, puColor);
            }
        }
        DrawRectangleRec(paddle2.rect, RED);
        DrawCircleV(ball2.pos, ball2.radius, WHITE);
        // 倒计时
        if (gamePhase == 0) {
            DrawText(TextFormat("%i", countdown), screenWidth/2 - 20, screenHeight/2 - 30, 60, WHITE);
        }
        // 游戏结束
        if (gamePhase == 2) {
            DrawRectangle(0, screenHeight/2 - 60, screenWidth, 120, Color{ 0, 0, 0, 200 });
            DrawText("GAME OVER", screenWidth/2 - 120, screenHeight/2 - 30, 40, RED);
            DrawText(TextFormat("P1: %i  vs  P2: %i", paddle1.score, paddle2.score), 
                     screenWidth/2 - 100, screenHeight/2 + 10, 20, WHITE);
            DrawText("Press R to restart", screenWidth/2 - 100, screenHeight/2 + 40, 20, WHITE);
        }
        EndDrawing();
    }
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
