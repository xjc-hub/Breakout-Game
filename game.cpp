#include "game.h"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iomanip>

void InitGame(Game &game) {
    game.state = GameState::MENU;
    game.menuSelection = 0;
    game.settingsSelection = 0;
    game.shopSelection = 0;
    game.levelSelection = 0;
    game.avgFps = 60;
    game.minFps = 60;
    game.maxFps = 60;
    game.frameCount = 0;
    game.fpsAccum = 0;
    game.totalCoins = 200;
    game.bgScroll = 0;
    game.bgScroll2 = 0;
    game.globalTime = 0;
    game.showTutorial = true;
    game.tutorialStep = 0;
    game.unlockedLevel = 1;
    
    for (int i = 0; i < 12; i++) game.unlockedItems[i] = 0;
    game.unlockedItems[0] = 1;
    game.unlockedItems[1] = 1;
    
    game.settings.difficulty = 1;
    game.settings.ballSpeed = 5.0f;
    game.settings.brickRows = 5;
    game.settings.brickCols = 8;
    game.settings.maxLives = 3;
    game.settings.gameTime = 120;
    game.settings.soundEnabled = true;
    game.settings.particlesEnabled = true;
    game.settings.screenShake = true;
    game.settings.vsync = true;
    game.settings.musicVolume = 0.7f;
    game.settings.sfxVolume = 1.0f;
    
    for (int i = 0; i < MAX_LEVELS; i++) {
        game.levels[i].rows = 4 + (i / 3);
        game.levels[i].cols = 7 + (i / 2);
        game.levels[i].hardCount = i * 2;
        game.levels[i].metalCount = std::max(0, i - 3) * 2;
        game.levels[i].goldCount = std::max(0, i - 5);
        game.levels[i].hasBombs = i >= 2;
        game.levels[i].hasRainbow = i >= 3;
        game.levels[i].hasEnemies = i >= 4;
        game.levels[i].timeLimit = 120 + i * 10;
        game.levels[i].name = "Level " + std::to_string(i + 1);
    }
    
    LoadHighScores(game);
}

void InitPlayer(PlayerGame &p, int offsetX, int level) {
    p.paddle.rect = { (float)(offsetX + 30), (float)(SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2), 
                      (float)PADDLE_WIDTH, (float)PADDLE_HEIGHT };
    p.paddle.score = 0;
    p.paddle.lives = 3;
    p.paddle.originalWidth = PADDLE_WIDTH;
    p.paddle.powerUpTimer = 0;
    p.paddle.shielded = false;
    p.paddle.shieldTimer = 0;
    p.paddle.hasLaser = false;
    p.paddle.laserTimer = 0;
    p.paddle.laserBeams.clear();
    
    ResetBall(p.ball, offsetX, 5.0f);
    p.ball.speedMultiplier = 1.0f;
    p.ball.fire = false;
    p.ball.ghost = false;
    p.ball.trailIndex = 0;
    for (int i = 0; i < 10; i++) p.ball.trail[i] = (Color){ 0, 0, 0, 0 };
    
    p.extraBalls.clear();
    p.enemies.clear();
    
    GenerateLevel(p, offsetX, level);
    
    for (int i = 0; i < MAX_PARTICLES; i++) p.particles[i].active = false;
    
    p.powerUps.clear();
    p.gameTime = 120 + level * 10;
    p.gameOver = false;
    p.won = false;
    p.combo = 0;
    p.maxCombo = 0;
    p.collisionTime = 0;
    p.drawTime = 0;
    p.level = level;
    p.shakeTimer = 0;
    p.shakeOffset = {0, 0};
    p.comboDisplayTimer = 0;
    p.bricksDestroyed = 0;
    p.totalBricks = 0;
    p.accuracy = 100;
    p.totalShots = 0;
    p.hits = 0;
}

void GenerateLevel(PlayerGame &p, int offsetX, int level) {
    p.bricks.clear();
    p.totalBricks = 0;
    
    int rows = 4 + (level / 3);
    int cols = 7 + (level / 2);
    if (rows > 9) rows = 9;
    if (cols > 11) cols = 11;
    
    Color colors[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, SKYBLUE, LIME, MAROON };
    
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Brick brick;
            brick.rect = { (float)(offsetX + 15 + col * (BRICK_WIDTH + 5)), 
                          (float)(35 + row * (BRICK_HEIGHT + 5)), 
                          (float)BRICK_WIDTH, (float)BRICK_HEIGHT };
            brick.originalPos = { brick.rect.x, brick.rect.y };
            brick.active = true;
            brick.animTimer = (float)(row * cols + col) * 0.05f;
            brick.glowTimer = 0;
            
            int r = GetRandomValue(0, 100);
            int diff = level;
            
            if (diff >= 5 && r < 3) {
                brick.type = BrickType::SPIKE;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = DARKPURPLE;
            } else if (diff >= 4 && r < 6) {
                brick.type = BrickType::POISON;
                brick.hitPoints = 2;
                brick.maxHitPoints = 2;
                brick.color = GREEN;
            } else if (diff >= 3 && r < 10) {
                brick.type = BrickType::GOLD_BRICK;
                brick.hitPoints = 5;
                brick.maxHitPoints = 5;
                brick.color = GOLD;
            } else if (diff >= 2 && r < 18) {
                brick.type = BrickType::METAL;
                brick.hitPoints = 3;
                brick.maxHitPoints = 3;
                brick.color = GRAY;
            } else if (diff >= 1 && r < 30) {
                brick.type = BrickType::HARD;
                brick.hitPoints = 2;
                brick.maxHitPoints = 2;
                brick.color = (Color){ 150, 100, 50, 255 };
            } else if (r < 6) {
                brick.type = BrickType::BOMB;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = RED;
            } else if (r < 10) {
                brick.type = BrickType::RAINBOW;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = colors[row % 10];
            } else if (r < 13) {
                brick.type = BrickType::ICE;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = SKYBLUE;
            } else if (r < 15) {
                brick.type = BrickType::HEAL;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = PINK;
            } else if (r < 17) {
                brick.type = BrickType::MYSTERY;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = PURPLE;
            } else {
                brick.type = BrickType::NORMAL;
                brick.hitPoints = 1;
                brick.maxHitPoints = 1;
                brick.color = colors[row % 10];
            }
            p.bricks.push_back(brick);
            p.totalBricks++;
        }
    }
    
    if (level >= 4) {
        int enemyCount = (level - 3) / 2;
        for (int i = 0; i < enemyCount && i < 3; i++) {
            EnemyType et = EnemyType::PATROL;
            if (level >= 6 && GetRandomValue(0, 100) < 30) et = EnemyType::CHASER;
            if (level >= 8 && GetRandomValue(0, 100) < 15) et = EnemyType::SHOOTER;
            SpawnEnemy(p, offsetX, et, 100 + i * 150);
        }
    }
}

void SpawnEnemy(PlayerGame &p, int offsetX, EnemyType type, float y) {
    Enemy enemy;
    enemy.rect = { (float)(offsetX + 50), y, 30, 30 };
    enemy.type = type;
    enemy.active = true;
    enemy.shootTimer = 0;
    
    switch (type) {
        case EnemyType::PATROL:
            enemy.health = 2;
            enemy.speed = 60;
            enemy.color = RED;
            enemy.direction = { 1, 0 };
            break;
        case EnemyType::CHASER:
            enemy.health = 3;
            enemy.speed = 80;
            enemy.color = DARKPURPLE;
            enemy.direction = { 0, 0 };
            break;
        case EnemyType::SHOOTER:
            enemy.health = 2;
            enemy.speed = 40;
            enemy.color = ORANGE;
            enemy.direction = { 1, 0 };
            break;
        case EnemyType::BOSS:
            enemy.health = 10;
            enemy.speed = 30;
            enemy.color = MAROON;
            enemy.direction = { 1, 0 };
            enemy.rect.width = 60;
            enemy.rect.height = 40;
            break;
        default:
            enemy.health = 1;
            enemy.speed = 50;
            enemy.color = GRAY;
            enemy.direction = { 1, 0 };
    }
    p.enemies.push_back(enemy);
}

void ResetBall(Ball &ball, int offsetX, float speed) {
    ball.pos = { (float)(offsetX + HALF_WIDTH/2), (float)(SCREEN_HEIGHT/2) };
    float angle = (float)(GetRandomValue(-45, 45)) * DEG2RAD;
    ball.speed = { speed * cosf(angle), speed * sinf(angle) };
    if (GetRandomValue(0, 1) == 0) ball.speed.x *= -1;
    ball.radius = BALL_SIZE;
    ball.active = true;
    ball.speedMultiplier = 1.0f;
    ball.fire = false;
    ball.ghost = false;
    ball.trailIndex = 0;
}

void SpawnParticles(PlayerGame &p, Vector2 pos, Color color, int count, float size) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!p.particles[i].active) {
            p.particles[i].pos = pos;
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float speed = (float)GetRandomValue(50, 400);
            p.particles[i].vel = { speed * cosf(angle), speed * sinf(angle) };
            p.particles[i].color = color;
            p.particles[i].life = (float)GetRandomValue(30, 120) / 100.0f;
            p.particles[i].maxLife = p.particles[i].life;
            p.particles[i].active = true;
            p.particles[i].size = size + (float)GetRandomValue(0, 20) / 10.0f;
            count--;
        }
    }
}

void UpdateParticles(PlayerGame &p, float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (p.particles[i].active) {
            p.particles[i].pos.x += p.particles[i].vel.x * dt;
            p.particles[i].pos.y += p.particles[i].vel.y * dt;
            p.particles[i].life -= dt;
            p.particles[i].vel.y += 400 * dt;
            p.particles[i].vel.x *= 0.98f;
            if (p.particles[i].life <= 0) p.particles[i].active = false;
        }
    }
}

void DrawParticles(const PlayerGame &p) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (p.particles[i].active) {
            float alpha = p.particles[i].life / p.particles[i].maxLife;
            Color c = p.particles[i].color;
            c.a = (unsigned char)(alpha * 255);
            float size = p.particles[i].size * (0.5f + alpha * 0.5f);
            DrawCircleV(p.particles[i].pos, size, c);
        }
    }
}

void SpawnPowerUp(PlayerGame &p, Vector2 pos) {
    if (GetRandomValue(0, 100) < 30) {
        PowerUp pu;
        pu.pos = pos;
        pu.speed = { 0, 70 + (float)GetRandomValue(0, 50) };
        pu.active = true;
        pu.timer = 20.0f;
        pu.pulse = 0;
        pu.type = (PowerUpType)GetRandomValue(1, 11);
        
        switch (pu.type) {
            case PowerUpType::WIDE_PADDLE: pu.color = GREEN; break;
            case PowerUpType::FAST_BALL: pu.color = RED; break;
            case PowerUpType::EXTRA_LIFE: pu.color = PINK; break;
            case PowerUpType::SLOW_BALL: pu.color = BLUE; break;
            case PowerUpType::MULTI_BALL: pu.color = YELLOW; break;
            case PowerUpType::FIRE_BALL: pu.color = ORANGE; break;
            case PowerUpType::SHIELD: pu.color = SKYBLUE; break;
            case PowerUpType::LASER: pu.color = LIME; break;
            case PowerUpType::MAGNET: pu.color = MAGENTA; break;
            case PowerUpType::GHOST_BALL: pu.color = VIOLET; break;
            case PowerUpType::BIG_BALL: pu.color = BEIGE; break;
            default: pu.color = WHITE;
        }
        p.powerUps.push_back(pu);
    }
}

void UpdatePowerUps(PlayerGame &p, float dt) {
    for (auto &pu : p.powerUps) {
        if (pu.active) {
            pu.pos.y += pu.speed.y * dt;
            pu.timer -= dt;
            pu.pulse += dt * 5;
            if (pu.timer <= 0 || pu.pos.y > SCREEN_HEIGHT + 20) pu.active = false;
        }
    }
}

void DrawPowerUps(const PlayerGame &p) {
    for (const auto &pu : p.powerUps) {
        if (pu.active) {
            float pulse = sinf(pu.pulse) * 3;
            DrawCircleV(pu.pos, 12 + pulse, pu.color);
            DrawCircleV(pu.pos, 10, BLACK);
            
            const char *symbol = "";
            switch (pu.type) {
                case PowerUpType::WIDE_PADDLE: symbol = "W"; break;
                case PowerUpType::FAST_BALL: symbol = "F"; break;
                case PowerUpType::EXTRA_LIFE: symbol = "+"; break;
                case PowerUpType::SLOW_BALL: symbol = "S"; break;
                case PowerUpType::MULTI_BALL: symbol = "M"; break;
                case PowerUpType::FIRE_BALL: symbol = "*"; break;
                case PowerUpType::SHIELD: symbol = "O"; break;
                case PowerUpType::LASER: symbol = "L"; break;
                case PowerUpType::MAGNET: symbol = "@"; break;
                case PowerUpType::GHOST_BALL: symbol = "G"; break;
                case PowerUpType::BIG_BALL: symbol = "B"; break;
                default: symbol = "?";
            }
            DrawText(symbol, pu.pos.x - 6, pu.pos.y - 8, 16, WHITE);
        }
    }
}

void DrawBrickWithEffect(const Brick &brick, float globalTime) {
    if (!brick.active) return;
    
    Color drawColor = brick.color;
    
    switch (brick.type) {
        case BrickType::ICE:
            drawColor = (Color){ 200, 230, 255, 200 };
            DrawRectangleRec(brick.rect, drawColor);
            DrawRectangleLinesEx(brick.rect, 1, WHITE);
            break;
        case BrickType::FIRE:
            {
                float flicker = sinf(globalTime * 10 + brick.rect.x) * 0.3f + 0.7f;
                drawColor = (Color){ 255, (unsigned char)(100 * flicker), 0, 255 };
                DrawRectangleRec(brick.rect, drawColor);
                DrawCircle(brick.rect.x + brick.rect.width/2, brick.rect.y, 5, ORANGE);
            }
            break;
        case BrickType::POISON:
            {
                float pulse = sinf(globalTime * 3 + brick.rect.x) * 0.3f + 0.7f;
                drawColor = (Color){ (unsigned char)(50 * pulse), (unsigned char)(200 * pulse), 50, 255 };
                DrawRectangleRec(brick.rect, drawColor);
                DrawText("\xE2\x98\xA0", brick.rect.x + brick.rect.width/2 - 7, brick.rect.y + 2, 14, WHITE);
            }
            break;
        case BrickType::HEAL:
            {
                float pulse = sinf(globalTime * 2 + brick.rect.x) * 0.2f + 0.8f;
                drawColor = (Color){ 255, (unsigned char)(150 * pulse), (unsigned char)(200 * pulse), 255 };
                DrawRectangleRec(brick.rect, drawColor);
                DrawText("+", brick.rect.x + brick.rect.width/2 - 5, brick.rect.y + 2, 14, WHITE);
            }
            break;
        case BrickType::MYSTERY:
            {
                float hue = fmod(globalTime * 50 + brick.rect.x * 10, 360);
                drawColor = ColorFromHSV(hue, 0.8f, 0.8f);
                DrawRectangleRec(brick.rect, drawColor);
                DrawText("?", brick.rect.x + brick.rect.width/2 - 5, brick.rect.y + 2, 14, WHITE);
            }
            break;
        case BrickType::SPIKE:
            DrawRectangleRec(brick.rect, DARKPURPLE);
            DrawTriangle(
                (Vector2){ brick.rect.x, brick.rect.y + brick.rect.height },
                (Vector2){ brick.rect.x + brick.rect.width/2, brick.rect.y },
                (Vector2){ brick.rect.x + brick.rect.width, brick.rect.y + brick.rect.height },
                PURPLE
            );
            break;
        case BrickType::HARD:
            DrawRectangleRec(brick.rect, brick.color);
            DrawRectangleLinesEx(brick.rect, 2, DARKGRAY);
            break;
        case BrickType::METAL:
            DrawRectangleRec(brick.rect, GRAY);
            DrawRectangleLinesEx(brick.rect, 3, LIGHTGRAY);
            DrawText("M", brick.rect.x + brick.rect.width/2 - 6, 
                    brick.rect.y + brick.rect.height/2 - 8, 16, WHITE);
            break;
        case BrickType::GOLD_BRICK:
            DrawRectangleRec(brick.rect, GOLD);
            DrawRectangleLinesEx(brick.rect, 2, YELLOW);
            {
                float pulse = sinf(globalTime * 3 + brick.rect.x) * 0.3f + 0.7f;
                DrawRectangleRec(brick.rect, (Color){ 255, 215, 0, (unsigned char)(pulse * 60) });
            }
            break;
        case BrickType::BOMB:
            DrawRectangleRec(brick.rect, RED);
            DrawCircle(brick.rect.x + brick.rect.width/2, brick.rect.y + brick.rect.height/2, 8, DARKGRAY);
            DrawText("!", brick.rect.x + brick.rect.width/2 - 5, 
                    brick.rect.y + brick.rect.height/2 - 8, 16, RED);
            break;
        case BrickType::RAINBOW:
            {
                float hue = fmod(globalTime * 100 + brick.rect.x * 20, 360);
                drawColor = ColorFromHSV(hue, 1, 1);
                DrawRectangleRec(brick.rect, drawColor);
            }
            break;
        default:
            DrawRectangleRec(brick.rect, brick.color);
            break;
    }
    
    if (brick.glowTimer > 0) {
        DrawRectangleRec(brick.rect, (Color){ 255, 255, 255, (unsigned char)(brick.glowTimer * 100) });
    }
    
    if (brick.hitPoints > 1) {
        DrawText(TextFormat("%d", brick.hitPoints), 
                brick.rect.x + brick.rect.width/2 - 5, 
                brick.rect.y + brick.rect.height/2 - 8, 16, WHITE);
    }
}

void DrawBallTrail(const PlayerGame &p) {
    if (!p.ball.active) return;
    for (int i = 0; i < 10; i++) {
        int idx = (p.ball.trailIndex - i + 10) % 10;
        if (p.ball.trail[idx].a > 0) {
            float alpha = (10 - i) / 10.0f * 0.5f;
            Color c = p.ball.trail[idx];
            c.a = (unsigned char)(alpha * 255);
            float size = p.ball.radius * (0.3f + alpha * 0.7f);
            DrawCircleV((Vector2){ (float)p.ball.trail[idx].r, (float)p.ball.trail[idx].g }, size, c);
        }
    }
}

void DrawComboEffect(const PlayerGame &p) {
    if (p.combo > 1 && p.comboDisplayTimer > 0) {
        float scale = 1 + sinf(p.comboDisplayTimer * 20) * 0.1f;
        float alpha = std::min(1.0f, p.comboDisplayTimer * 2);
        
        Color c = WHITE;
        if (p.combo >= 10) c = GOLD;
        else if (p.combo >= 5) c = ORANGE;
        
        DrawText(TextFormat("COMBO x%d!", p.combo), 
                p.paddle.rect.x + HALF_WIDTH/2 - 80, 
                120, (int)(24 * scale), 
                (Color){ c.r, c.g, c.b, (unsigned char)(alpha * 255) });
    }
}

void DrawBackground(const Game &game) {
    for (int i = 0; i < 80; i++) {
        float x = fmod(i * 97.3f + game.bgScroll, SCREEN_WIDTH);
        float y = fmod(i * 53.7f + game.bgScroll2, SCREEN_HEIGHT);
        float brightness = sinf(game.globalTime * 2 + i * 1.7f) * 0.4f + 0.6f;
        float size = 0.5f + sinf(i * 3.1f) * 0.5f;
        DrawCircle(x, y, size, (Color){ (unsigned char)(brightness * 80), 
                                       (unsigned char)(brightness * 80), 
                                       (unsigned char)(brightness * 150), 255 });
    }
    
    for (int i = 0; i < 30; i++) {
        float x = fmod(i * 173.5f + game.bgScroll * 0.3f, SCREEN_WIDTH);
        float y = fmod(i * 89.3f + game.bgScroll2 * 0.5f, SCREEN_HEIGHT);
        float brightness = sinf(game.globalTime * 3 + i * 2.3f) * 0.3f + 0.7f;
        DrawCircle(x, y, 1.5f, (Color){ (unsigned char)(brightness * 150), 
                                       (unsigned char)(brightness * 150), 
                                       (unsigned char)(brightness * 255), 255 });
    }
}

void DrawHUD(const PlayerGame &p, int offsetX, bool isPlayer2) {
    Color pColor = isPlayer2 ? RED : BLUE;
    
    DrawRectangle(offsetX, 0, HALF_WIDTH, 115, (Color){ 0, 0, 0, 150 });
    
    DrawText(TextFormat("PLAYER %d", isPlayer2 ? 2 : 1), offsetX + 10, 5, 24, pColor);
    DrawText(TextFormat("Score: %d", p.paddle.score), offsetX + 10, 32, 18, WHITE);
    
    std::string lives = "";
    for (int i = 0; i < p.paddle.lives; i++) lives += "<3 ";
    DrawText(lives.c_str(), offsetX + 10, 54, 16, PINK);
    
    Color timeColor = WHITE;
    if (p.gameTime < 30) timeColor = RED;
    else if (p.gameTime < 60) timeColor = YELLOW;
    DrawText(TextFormat("Time: %d", (int)p.gameTime), offsetX + 10, 76, 16, timeColor);
    
    DrawText(TextFormat("Level: %d", p.level + 1), offsetX + 10, 96, 14, GRAY);
    
    if (p.combo > 1) {
        DrawComboEffect(p);
    }
    
    if (p.paddle.shielded) {
        float pulse = sinf(GetTime() * 4) * 0.3f + 0.7f;
        DrawCircle(offsetX + HALF_WIDTH/2, SCREEN_HEIGHT - 35, 25, 
                  (Color){ 100, 150, 255, (unsigned char)(pulse * 80) });
        DrawCircle(offsetX + HALF_WIDTH/2, SCREEN_HEIGHT - 35, 20, 
                  (Color){ 100, 150, 255, (unsigned char)(pulse * 40) });
        DrawText("SHIELD", offsetX + HALF_WIDTH/2 - 30, SCREEN_HEIGHT - 50, 14, SKYBLUE);
    }
    
    if (p.paddle.hasLaser) {
        DrawText("LASER", offsetX + HALF_WIDTH/2 - 25, SCREEN_HEIGHT - 20, 14, LIME);
    }
    
    if (!p.ball.active && !p.gameOver) {
        float pulse = sinf(GetTime() * 3) * 0.3f + 0.7f;
        DrawText("Press SPACE/ENTER to serve", 
                offsetX + HALF_WIDTH/2 - 130, SCREEN_HEIGHT/2 - 15, 18, 
                (Color){ 255, 255, 0, (unsigned char)(pulse * 255) });
    }
    
    if (p.gameOver) {
        DrawRectangle(offsetX, SCREEN_HEIGHT/2 - 50, HALF_WIDTH, 100, (Color){ 0, 0, 0, 200 });
        DrawText(p.won ? "YOU WIN!" : "GAME OVER", 
                offsetX + HALF_WIDTH/2 - 70, SCREEN_HEIGHT/2 - 25, 35, p.won ? GREEN : RED);
        
        if (p.won) {
            DrawText(TextFormat("Score: %d | Combo: %d", p.paddle.score, p.maxCombo), 
                    offsetX + HALF_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 20, 16, WHITE);
        }
    }
    
    if (p.totalBricks > 0) {
        float progress = (float)p.bricksDestroyed / p.totalBricks;
        DrawRectangle(offsetX + 10, SCREEN_HEIGHT - 15, HALF_WIDTH - 20, 8, DARKGRAY);
        DrawRectangle(offsetX + 10, SCREEN_HEIGHT - 15, (int)((HALF_WIDTH - 20) * progress), 8, 
                     progress > 0.7f ? GREEN : (progress > 0.3f ? YELLOW : RED));
    }
}

void UpdateEnemies(PlayerGame &p, int offsetX, float dt) {
    for (auto &enemy : p.enemies) {
        if (!enemy.active) continue;
        
        switch (enemy.type) {
            case EnemyType::PATROL:
                enemy.rect.x += enemy.direction.x * enemy.speed * dt;
                if (enemy.rect.x <= offsetX + 20 || enemy.rect.x >= offsetX + HALF_WIDTH - enemy.rect.width - 20) {
                    enemy.direction.x *= -1;
                }
                break;
            case EnemyType::CHASER:
                {
                    Vector2 target;
                    if (p.ball.active) {
                        target = p.ball.pos;
                    } else {
                        target = (Vector2){ p.paddle.rect.x, p.paddle.rect.y };
                    }
                    Vector2 dir = { target.x - enemy.rect.x, target.y - enemy.rect.y };
                    float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (len > 0) {
                        dir.x /= len;
                        dir.y /= len;
                    }
                    enemy.rect.x += dir.x * enemy.speed * dt;
                    enemy.rect.y += dir.y * enemy.speed * dt;
                    enemy.rect.x = std::max((float)offsetX + 20, std::min(enemy.rect.x, (float)offsetX + HALF_WIDTH - enemy.rect.width - 20));
                    enemy.rect.y = std::max(50.0f, std::min(enemy.rect.y, (float)SCREEN_HEIGHT - 150));
                }
                break;
            case EnemyType::SHOOTER:
                enemy.rect.x += enemy.direction.x * enemy.speed * dt;
                if (enemy.rect.x <= offsetX + 20 || enemy.rect.x >= offsetX + HALF_WIDTH - enemy.rect.width - 20) {
                    enemy.direction.x *= -1;
                }
                enemy.shootTimer += dt;
                if (enemy.shootTimer > 2.0f) {
                    enemy.shootTimer = 0;
                    SpawnParticles(p, (Vector2){ enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height }, 
                                 RED, 5, 4);
                }
                break;
            case EnemyType::BOSS:
                enemy.rect.x += enemy.direction.x * enemy.speed * dt;
                if (enemy.rect.x <= offsetX + 20 || enemy.rect.x >= offsetX + HALF_WIDTH - enemy.rect.width - 20) {
                    enemy.direction.x *= -1;
                }
                enemy.shootTimer += dt;
                if (enemy.shootTimer > 1.0f) {
                    enemy.shootTimer = 0;
                    SpawnParticles(p, (Vector2){ enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height }, 
                                 ORANGE, 10, 5);
                }
                break;
            default:
                break;
        }
        
        if (p.ball.active && CheckCollisionCircleRec(p.ball.pos, p.ball.radius, enemy.rect)) {
            enemy.health--;
            if (enemy.health <= 0) {
                enemy.active = false;
                p.paddle.score += 50 * (1 + p.combo);
                SpawnParticles(p, (Vector2){ enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height/2 }, 
                             enemy.color, 30, 5);
                SpawnPowerUp(p, (Vector2){ enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height/2 });
            }
            p.ball.speed.y *= -1;
        }
        
        if (CheckCollisionRecs(enemy.rect, p.paddle.rect)) {
            p.paddle.lives--;
            p.shakeTimer = 0.5f;
            if (p.paddle.lives <= 0) {
                p.gameOver = true;
                p.won = false;
            }
        }
    }
}

void DrawEnemies(const PlayerGame &p) {
    for (const auto &enemy : p.enemies) {
        if (!enemy.active) continue;
        
        Color c = enemy.color;
        float pulse = sinf(GetTime() * 3 + enemy.rect.x) * 0.2f + 0.8f;
        
        switch (enemy.type) {
            case EnemyType::PATROL:
                DrawRectangleRec(enemy.rect, c);
                DrawRectangleLinesEx(enemy.rect, 2, DARKGRAY);
                DrawCircle(enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height/2, 8, 
                          (Color){ 255, 255, 255, (unsigned char)(pulse * 100) });
                break;
            case EnemyType::CHASER:
                DrawRectangleRec(enemy.rect, c);
                DrawTriangle(
                    (Vector2){ enemy.rect.x, enemy.rect.y },
                    (Vector2){ enemy.rect.x + enemy.rect.width, enemy.rect.y },
                    (Vector2){ enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height },
                    (Color){ c.r, c.g, c.b, (unsigned char)(pulse * 200) }
                );
                break;
            case EnemyType::SHOOTER:
                DrawCircle(enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height/2, 
                          enemy.rect.width/2, c);
                DrawCircle(enemy.rect.x + enemy.rect.width/2, enemy.rect.y + enemy.rect.height/2, 
                          enemy.rect.width/4, DARKGRAY);
                break;
            case EnemyType::BOSS:
                {
                    DrawRectangleRec(enemy.rect, c);
                    DrawRectangleLinesEx(enemy.rect, 3, GOLD);
                    DrawText("BOSS", enemy.rect.x + 10, enemy.rect.y + 10, 20, GOLD);
                    float hpPercent = (float)enemy.health / 10.0f;
                    DrawRectangle(enemy.rect.x, enemy.rect.y - 10, enemy.rect.width, 8, DARKGRAY);
                    DrawRectangle(enemy.rect.x, enemy.rect.y - 10, (int)(enemy.rect.width * hpPercent), 8, 
                                 hpPercent > 0.5f ? GREEN : RED);
                }
                break;
            default:
                DrawRectangleRec(enemy.rect, c);
                break;
        }
    }
}

void DrawLaserBeams(const PlayerGame &p) {
    for (const auto &beam : p.paddle.laserBeams) {
        DrawLineEx(beam, (Vector2){ beam.x, beam.y - 30 }, 3, LIME);
        DrawLineEx(beam, (Vector2){ beam.x, beam.y - 30 }, 1, WHITE);
    }
}

void UpdatePlayer(PlayerGame &p, int offsetX, bool isPlayer2, float dt) {
    if (p.gameOver) return;
    
    double startTime = GetTime();
    
    p.gameTime -= dt;
    if (p.gameTime <= 0) {
        p.gameTime = 0;
        p.gameOver = true;
        p.won = false;
        return;
    }
    
    if (p.shakeTimer > 0) {
        p.shakeTimer -= dt;
        p.shakeOffset = { (float)GetRandomValue(-8, 8), (float)GetRandomValue(-8, 8) };
    } else {
        p.shakeOffset = {0, 0};
    }
    
    if (p.comboDisplayTimer > 0) p.comboDisplayTimer -= dt;
    
    float paddleSpeed = 450 * dt;
    if (!isPlayer2) {
        if (IsKeyDown(KEY_W) && p.paddle.rect.y > 0) p.paddle.rect.y -= paddleSpeed;
        if (IsKeyDown(KEY_S) && p.paddle.rect.y + p.paddle.rect.height < SCREEN_HEIGHT) p.paddle.rect.y += paddleSpeed;
        if (IsKeyDown(KEY_A) && p.paddle.rect.x > offsetX + 10) p.paddle.rect.x -= paddleSpeed * 0.5f;
        if (IsKeyDown(KEY_D) && p.paddle.rect.x + p.paddle.rect.width < offsetX + HALF_WIDTH - 10) p.paddle.rect.x += paddleSpeed * 0.5f;
    } else {
        if (IsKeyDown(KEY_UP) && p.paddle.rect.y > 0) p.paddle.rect.y -= paddleSpeed;
        if (IsKeyDown(KEY_DOWN) && p.paddle.rect.y + p.paddle.rect.height < SCREEN_HEIGHT) p.paddle.rect.y += paddleSpeed;
        if (IsKeyDown(KEY_LEFT) && p.paddle.rect.x > offsetX + 10) p.paddle.rect.x -= paddleSpeed * 0.5f;
        if (IsKeyDown(KEY_RIGHT) && p.paddle.rect.x + p.paddle.rect.width < offsetX + HALF_WIDTH - 10) p.paddle.rect.x += paddleSpeed * 0.5f;
    }
    
    if (p.paddle.powerUpTimer > 0) {
        p.paddle.powerUpTimer -= dt;
        if (p.paddle.powerUpTimer <= 0) {
            p.paddle.rect.width = p.paddle.originalWidth;
            p.ball.speedMultiplier = 1.0f;
            p.ball.fire = false;
            p.ball.ghost = false;
            p.ball.radius = BALL_SIZE;
            p.paddle.hasLaser = false;
        }
    }
    
    if (p.paddle.shieldTimer > 0) {
        p.paddle.shieldTimer -= dt;
        if (p.paddle.shieldTimer <= 0) p.paddle.shielded = false;
    }
    
    if (p.paddle.hasLaser && IsKeyPressed(isPlayer2 ? KEY_RIGHT_SHIFT : KEY_LEFT_SHIFT)) {
        p.paddle.laserBeams.push_back((Vector2){ p.paddle.rect.x + p.paddle.rect.width/2, p.paddle.rect.y });
        p.totalShots++;
        
        for (auto &brick : p.bricks) {
            if (!brick.active) continue;
            if (brick.rect.y < p.paddle.rect.y && 
                brick.rect.x < p.paddle.rect.x + p.paddle.rect.width && 
                brick.rect.x + brick.rect.width > p.paddle.rect.x) {
                brick.hitPoints--;
                if (brick.hitPoints <= 0) {
                    brick.active = false;
                    p.paddle.score += 10;
                    p.bricksDestroyed++;
                    SpawnParticles(p, (Vector2){ brick.rect.x + brick.rect.width/2, brick.rect.y + brick.rect.height/2 }, 
                                 brick.color, 10);
                }
                p.hits++;
            }
        }
    }
    
    for (auto &beam : p.paddle.laserBeams) {
        beam.y -= 500 * dt;
    }
    p.paddle.laserBeams.erase(std::remove_if(p.paddle.laserBeams.begin(), p.paddle.laserBeams.end(),
        [](const Vector2 &b) { return b.y < 0; }), p.paddle.laserBeams.end());
    
    if (p.ball.active) {
        p.ball.trail[p.ball.trailIndex] = (Color){ (unsigned char)p.ball.pos.x, (unsigned char)p.ball.pos.y, 0, 255 };
        p.ball.trailIndex = (p.ball.trailIndex + 1) % 10;
        
        p.ball.pos.x += p.ball.speed.x * p.ball.speedMultiplier * 60 * dt;
        p.ball.pos.y += p.ball.speed.y * p.ball.speedMultiplier * 60 * dt;
        
        if (p.ball.pos.y - p.ball.radius <= 0 || p.ball.pos.y + p.ball.radius >= SCREEN_HEIGHT) {
            p.ball.speed.y *= -1;
        }
        
        if (p.ball.pos.x - p.ball.radius <= offsetX) {
            p.ball.speed.x = fabsf(p.ball.speed.x);
        }
        
        if (p.ball.pos.x + p.ball.radius >= offsetX + HALF_WIDTH) {
            if (p.paddle.shielded) {
                p.ball.speed.x = -fabsf(p.ball.speed.x);
                p.paddle.shielded = false;
                p.paddle.shieldTimer = 0;
                SpawnParticles(p, (Vector2){ (float)(offsetX + HALF_WIDTH), p.ball.pos.y }, SKYBLUE, 30, 5);
            } else {
                p.paddle.lives--;
                p.shakeTimer = 0.5f;
                if (p.paddle.lives <= 0) {
                    p.gameOver = true;
                    p.won = false;
                } else {
                    ResetBall(p.ball, offsetX, 5.0f * p.ball.speedMultiplier);
                }
                return;
            }
        }
        
        if (CheckCollisionCircleRec(p.ball.pos, p.ball.radius, p.paddle.rect)) {
            p.ball.speed.x = fabsf(p.ball.speed.x);
            float hitPos = (p.ball.pos.y - p.paddle.rect.y) / p.paddle.rect.height - 0.5f;
            p.ball.speed.y = hitPos * 400;
            p.combo = 0;
            p.hits++;
            SpawnParticles(p, (Vector2){ p.ball.pos.x, p.ball.pos.y + p.ball.radius }, WHITE, 8, 2);
        }
        
        double collisionStart = GetTime();
        int gx = (int)(p.ball.pos.x - offsetX) / CELL_W;
        int gy = (int)(p.ball.pos.y) / CELL_H;
        gx = std::max(0, std::min(gx, GRID_COLS - 1));
        gy = std::max(0, std::min(gy, GRID_ROWS - 1));
        
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = gx + dx, ny = gy + dy;
                if (nx < 0 || nx >= GRID_COLS || ny < 0 || ny >= GRID_ROWS) continue;
                
                for (auto &brick : p.bricks) {
                    if (!brick.active) continue;
                    int bgx = (int)(brick.rect.x - offsetX) / CELL_W;
                    int bgy = (int)(brick.rect.y) / CELL_H;
                    if (bgx == nx && bgy == ny) {
                        if (CheckCollisionCircleRec(p.ball.pos, p.ball.radius, brick.rect)) {
                            if (p.ball.ghost) {
                                brick.hitPoints--;
                                if (brick.hitPoints <= 0) {
                                    brick.active = false;
                                    p.paddle.score += 10;
                                    p.bricksDestroyed++;
                                    SpawnParticles(p, (Vector2){ brick.rect.x + brick.rect.width/2, 
                                                              brick.rect.y + brick.rect.height/2 }, brick.color, 15);
                                }
                                continue;
                            }
                            
                            brick.hitPoints--;
                            brick.glowTimer = 0.2f;
                            
                            if (brick.hitPoints <= 0) {
                                brick.active = false;
                                p.bricksDestroyed++;
                                
                                int points = 10;
                                bool specialEffect = false;
                                
                                switch (brick.type) {
                                    case BrickType::HARD: points = 20; break;
                                    case BrickType::METAL: points = 30; break;
                                    case BrickType::GOLD_BRICK: points = 50; break;
                                    case BrickType::BOMB: 
                                        points = 15;
                                        specialEffect = true;
                                        p.shakeTimer = 0.5f;
                                        SpawnParticles(p, (Vector2){ brick.rect.x + brick.rect.width/2, 
                                                                  brick.rect.y + brick.rect.height/2 }, ORANGE, 60, 6);
                                        for (auto &other : p.bricks) {
                                            if (&other != &brick && other.active) {
                                                float dist = sqrt(pow(other.rect.x - brick.rect.x, 2) + 
                                                                pow(other.rect.y - brick.rect.y, 2));
                                                if (dist < 100) {
                                                    other.active = false;
                                                    p.bricksDestroyed++;
                                                    SpawnParticles(p, (Vector2){ other.rect.x + other.rect.width/2, 
                                                                              other.rect.y + other.rect.height/2 }, 
                                                                 other.color, 20, 4);
                                                }
                                            }
                                        }
                                        break;
                                    case BrickType::RAINBOW: points = 25; break;
                                    case BrickType::ICE:
                                        points = 15;
                                        p.ball.speed.x *= 0.8f;
                                        p.ball.speed.y *= 0.8f;
                                        break;
                                    case BrickType::FIRE:
                                        points = 20;
                                        p.ball.fire = true;
                                        p.paddle.powerUpTimer = 5.0f;
                                        break;
                                    case BrickType::POISON:
                                        points = 15;
                                        p.paddle.lives--;
                                        break;
                                    case BrickType::HEAL:
                                        points = 5;
                                        p.paddle.lives = std::min(p.paddle.lives + 1, 5);
                                        break;
                                    case BrickType::MYSTERY:
                                        points = 10;
                                        SpawnPowerUp(p, (Vector2){ brick.rect.x + brick.rect.width/2, 
                                                                brick.rect.y + brick.rect.height/2 });
                                        break;
                                    case BrickType::SPIKE:
                                        points = 30;
                                        p.ball.speed.y = -fabsf(p.ball.speed.y) * 1.5f;
                                        break;
                                    default: points = 10;
                                }
                                
                                p.paddle.score += points * (1 + p.combo);
                                p.combo++;
                                p.maxCombo = std::max(p.maxCombo, p.combo);
                                p.comboDisplayTimer = 2.0f;
                                
                                if (!specialEffect) {
                                    SpawnParticles(p, (Vector2){ brick.rect.x + brick.rect.width/2, 
                                                              brick.rect.y + brick.rect.height/2 }, brick.color, 20, 4);
                                }
                                
                                SpawnPowerUp(p, (Vector2){ brick.rect.x + brick.rect.width/2, 
                                                        brick.rect.y + brick.rect.height/2 });
                            } else {
                                brick.color.r -= 40;
                                brick.color.g -= 30;
                            }
                            
                            if (p.ball.fire) {
                                p.ball.speed.y *= -1;
                            } else {
                                if (p.ball.pos.x < brick.rect.x || p.ball.pos.x > brick.rect.x + brick.rect.width) {
                                    p.ball.speed.x *= -1;
                                } else {
                                    p.ball.speed.y *= -1;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
        double collisionEnd = GetTime();
        p.collisionTime = (collisionEnd - collisionStart) * 1000;
    } else {
        if (IsKeyPressed(isPlayer2 ? KEY_ENTER : KEY_SPACE)) {
            p.totalShots++;
            ResetBall(p.ball, offsetX, 5.0f * p.ball.speedMultiplier);
        }
    }
    
    for (auto &b : p.extraBalls) {
        if (b.active) {
            b.pos.x += b.speed.x * b.speedMultiplier * 60 * dt;
            b.pos.y += b.speed.y * b.speedMultiplier * 60 * dt;
            if (b.pos.y - b.radius <= 0 || b.pos.y + b.radius >= SCREEN_HEIGHT) b.speed.y *= -1;
            if (b.pos.x - b.radius <= offsetX) b.speed.x = fabsf(b.speed.x);
            if (b.pos.x + b.radius >= offsetX + HALF_WIDTH) b.active = false;
            
            if (CheckCollisionCircleRec(b.pos, b.radius, p.paddle.rect)) {
                b.speed.x = fabsf(b.speed.x);
                float hitPos = (b.pos.y - p.paddle.rect.y) / p.paddle.rect.height - 0.5f;
                b.speed.y = hitPos * 400;
            }
        }
    }
    p.extraBalls.erase(std::remove_if(p.extraBalls.begin(), p.extraBalls.end(), 
        [](const Ball &b) { return !b.active; }), p.extraBalls.end());
    
    UpdatePowerUps(p, dt);
    for (auto &pu : p.powerUps) {
        if (pu.active && CheckCollisionCircleRec(pu.pos, 12, p.paddle.rect)) {
            pu.active = false;
            SpawnParticles(p, pu.pos, pu.color, 20, 5);
            switch (pu.type) {
                case PowerUpType::WIDE_PADDLE:
                    p.paddle.rect.width = PADDLE_WIDTH * 2.5f;
                    p.paddle.powerUpTimer = 15.0f;
                    break;
                case PowerUpType::FAST_BALL:
                    p.ball.speedMultiplier = 1.8f;
                    p.paddle.powerUpTimer = 12.0f;
                    break;
                case PowerUpType::EXTRA_LIFE:
                    p.paddle.lives = std::min(p.paddle.lives + 1, 5);
                    break;
                case PowerUpType::SLOW_BALL:
                    p.ball.speedMultiplier = 0.5f;
                    p.paddle.powerUpTimer = 12.0f;
                    break;
                case PowerUpType::MULTI_BALL:
                    for (int i = 0; i < 4; i++) {
                        Ball nb = p.ball;
                        float angle = (float)GetRandomValue(-75, 75) * DEG2RAD;
                        nb.speed = (Vector2){ p.ball.speed.x * cosf(angle), p.ball.speed.y * sinf(angle) };
                        p.extraBalls.push_back(nb);
                    }
                    break;
                case PowerUpType::FIRE_BALL:
                    p.ball.fire = true;
                    p.paddle.powerUpTimer = 12.0f;
                    break;
                case PowerUpType::SHIELD:
                    p.paddle.shielded = true;
                    p.paddle.shieldTimer = 20.0f;
                    break;
                case PowerUpType::LASER:
                    p.paddle.hasLaser = true;
                    p.paddle.powerUpTimer = 15.0f;
                    break;
                case PowerUpType::MAGNET:
                    if (p.ball.active) {
                        Vector2 dir = { p.paddle.rect.x + p.paddle.rect.width/2 - p.ball.pos.x,
                                       p.paddle.rect.y - p.ball.pos.y };
                        float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                        if (len > 0) {
                            p.ball.speed.x += dir.x / len * 100 * dt;
                            p.ball.speed.y += dir.y / len * 100 * dt;
                        }
                    }
                    break;
                case PowerUpType::GHOST_BALL:
                    p.ball.ghost = true;
                    p.paddle.powerUpTimer = 10.0f;
                    break;
                case PowerUpType::BIG_BALL:
                    p.ball.radius = BALL_SIZE * 2;
                    p.paddle.powerUpTimer = 10.0f;
                    break;
                default: break;
            }
        }
    }
    
    UpdateEnemies(p, offsetX, dt);
    UpdateParticles(p, dt);
    
    for (auto &brick : p.bricks) {
        if (brick.glowTimer > 0) brick.glowTimer -= dt;
    }
    
    bool allDestroyed = true;
    for (const auto &brick : p.bricks) {
        if (brick.active) { allDestroyed = false; break; }
    }
    
    bool allEnemiesDead = true;
    for (const auto &enemy : p.enemies) {
        if (enemy.active) { allEnemiesDead = false; break; }
    }
    
    if (allDestroyed && allEnemiesDead) {
        p.gameOver = true;
        p.won = true;
    }
    
    double endTime = GetTime();
    p.drawTime = (endTime - startTime) * 1000;
}

void UpdateGame(Game &game) {
    game.globalTime += GetFrameTime();
    game.bgScroll += 0.3f;
    game.bgScroll2 += 0.15f;
    if (game.bgScroll > SCREEN_WIDTH) game.bgScroll = 0;
    if (game.bgScroll2 > SCREEN_HEIGHT) game.bgScroll2 = 0;
    
    if (game.state == GameState::MENU) {
        if (IsKeyPressed(KEY_UP)) game.menuSelection = (game.menuSelection - 1 + 6) % 6;
        if (IsKeyPressed(KEY_DOWN)) game.menuSelection = (game.menuSelection + 1) % 6;
        if (IsKeyPressed(KEY_ENTER)) {
            switch (game.menuSelection) {
                case 0:
                    game.state = GameState::PLAYING;
                    InitPlayer(game.player1, 0, game.levelSelection);
                    InitPlayer(game.player2, HALF_WIDTH, game.levelSelection);
                    game.player1.paddle.lives = game.settings.maxLives;
                    game.player2.paddle.lives = game.settings.maxLives;
                    game.player1.gameTime = game.levels[game.levelSelection].timeLimit;
                    game.player2.gameTime = game.levels[game.levelSelection].timeLimit;
                    break;
                case 1: game.state = GameState::LEVEL_SELECT; break;
                case 2: game.state = GameState::SETTINGS; break;
                case 3: game.state = GameState::SHOP; break;
                case 4: game.state = GameState::HIGHSCORES; break;
                case 5: break;
            }
        }
        return;
    }
    
    if (game.state == GameState::LEVEL_SELECT) {
        if (IsKeyPressed(KEY_UP)) game.levelSelection = (game.levelSelection - 1 + MAX_LEVELS) % MAX_LEVELS;
        if (IsKeyPressed(KEY_DOWN)) game.levelSelection = (game.levelSelection + 1) % MAX_LEVELS;
        if (IsKeyPressed(KEY_ENTER) && game.levelSelection < game.unlockedLevel) {
            game.state = GameState::MENU;
            game.menuSelection = 0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) game.state = GameState::MENU;
        return;
    }
    
    if (game.state == GameState::SETTINGS) {
        if (IsKeyPressed(KEY_UP)) game.settingsSelection = (game.settingsSelection - 1 + 9) % 9;
        if (IsKeyPressed(KEY_DOWN)) game.settingsSelection = (game.settingsSelection + 1) % 9;
        if (IsKeyPressed(KEY_LEFT)) {
            switch (game.settingsSelection) {
                case 0: game.settings.difficulty = std::max(1, game.settings.difficulty - 1); break;
                case 1: game.settings.ballSpeed = std::max(3.0f, game.settings.ballSpeed - 0.5f); break;
                case 2: game.settings.brickRows = std::max(3, game.settings.brickRows - 1); break;
                case 3: game.settings.maxLives = std::max(1, game.settings.maxLives - 1); break;
                case 4: game.settings.gameTime = std::max(30, game.settings.gameTime - 10); break;
                case 5: game.settings.soundEnabled = !game.settings.soundEnabled; break;
                case 6: game.settings.particlesEnabled = !game.settings.particlesEnabled; break;
                case 7: game.settings.screenShake = !game.settings.screenShake; break;
                case 8: game.settings.musicVolume = std::max(0.0f, game.settings.musicVolume - 0.1f); break;
            }
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            switch (game.settingsSelection) {
                case 0: game.settings.difficulty = std::min(3, game.settings.difficulty + 1); break;
                case 1: game.settings.ballSpeed = std::min(10.0f, game.settings.ballSpeed + 0.5f); break;
                case 2: game.settings.brickRows = std::min(8, game.settings.brickRows + 1); break;
                case 3: game.settings.maxLives = std::min(10, game.settings.maxLives + 1); break;
                case 4: game.settings.gameTime = std::min(300, game.settings.gameTime + 10); break;
                case 5: game.settings.soundEnabled = !game.settings.soundEnabled; break;
                case 6: game.settings.particlesEnabled = !game.settings.particlesEnabled; break;
                case 7: game.settings.screenShake = !game.settings.screenShake; break;
                case 8: game.settings.musicVolume = std::min(1.0f, game.settings.musicVolume + 0.1f); break;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) game.state = GameState::MENU;
        return;
    }
    
    if (game.state == GameState::SHOP) {
        if (IsKeyPressed(KEY_UP)) game.shopSelection = (game.shopSelection - 1 + 11) % 11;
        if (IsKeyPressed(KEY_DOWN)) game.shopSelection = (game.shopSelection + 1) % 11;
        if (IsKeyPressed(KEY_ENTER)) {
            int costs[] = { 0, 50, 100, 150, 200, 250, 300, 500, 400, 350, 450, 600 };
            if (game.shopSelection < 11 && !game.unlockedItems[game.shopSelection + 1]) {
                if (game.totalCoins >= costs[game.shopSelection + 1]) {
                    game.totalCoins -= costs[game.shopSelection + 1];
                    game.unlockedItems[game.shopSelection + 1] = 1;
                }
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) game.state = GameState::MENU;
        return;
    }
    
    if (game.state == GameState::HIGHSCORES) {
        if (IsKeyPressed(KEY_ESCAPE)) game.state = GameState::MENU;
        return;
    }
    
    if (game.state == GameState::PLAYING) {
        if (IsKeyPressed(KEY_P)) {
            game.state = GameState::PAUSED;
            return;
        }
        
        float dt = GetFrameTime();
        UpdatePlayer(game.player1, 0, false, dt);
        UpdatePlayer(game.player2, HALF_WIDTH, true, dt);
        
        if (game.player1.gameOver && game.player2.gameOver) {
            game.state = GameState::GAMEOVER;
            
            HighScore hs1 = { "Player 1", game.player1.paddle.score, game.player1.level + 1, 
                            game.player1.gameTime, game.player1.maxCombo };
            HighScore hs2 = { "Player 2", game.player2.paddle.score, game.player2.level + 1, 
                            game.player2.gameTime, game.player2.maxCombo };
            game.highScores.push_back(hs1);
            game.highScores.push_back(hs2);
            std::sort(game.highScores.begin(), game.highScores.end(), 
                [](const HighScore &a, const HighScore &b) { return a.score > b.score; });
            if (game.highScores.size() > MAX_HIGHSCORES) game.highScores.resize(MAX_HIGHSCORES);
            SaveHighScores(game);
            
            int coins = (game.player1.paddle.score + game.player2.paddle.score) / 10;
            game.totalCoins += coins;
            
            if (game.player1.won || game.player2.won) {
                game.unlockedLevel = std::max(game.unlockedLevel, game.levelSelection + 2);
            }
        }
        
        game.frameCount++;
        game.fpsAccum += 1.0f / dt;
        if (game.frameCount >= 60) {
            game.avgFps = game.fpsAccum / game.frameCount;
            game.minFps = std::min(game.minFps, game.avgFps);
            game.maxFps = std::max(game.maxFps, game.avgFps);
            game.frameCount = 0;
            game.fpsAccum = 0;
        }
    }
    
    if (game.state == GameState::PAUSED) {
        if (IsKeyPressed(KEY_P)) game.state = GameState::PLAYING;
        return;
    }
    
    if (game.state == GameState::GAMEOVER) {
        if (IsKeyPressed(KEY_ENTER)) game.state = GameState::MENU;
        return;
    }
}

void DrawPlayer(const PlayerGame &p, int offsetX, bool isPlayer2) {
    Vector2 shake = p.shakeOffset;
    
    DrawRectangle(offsetX + (int)shake.x, (int)shake.y, HALF_WIDTH, SCREEN_HEIGHT, (Color){ 5, 5, 25, 255 });
    
    DrawLine(offsetX + HALF_WIDTH, 0, offsetX + HALF_WIDTH, SCREEN_HEIGHT, 
             (Color){ 50, 50, 100, 255 });
    DrawLine(offsetX + HALF_WIDTH - 1, 0, offsetX + HALF_WIDTH - 1, SCREEN_HEIGHT, 
             (Color){ 30, 30, 80, 100 });
    
    DrawBallTrail(p);
    
    for (const auto &brick : p.bricks) {
        DrawBrickWithEffect(brick, GetTime());
    }
    
    DrawEnemies(p);
    DrawLaserBeams(p);
    
    DrawRectangleRec(p.paddle.rect, isPlayer2 ? RED : BLUE);
    if (p.paddle.shielded) {
        float pulse = sinf(GetTime() * 4) * 0.3f + 0.7f;
        DrawRectangleLinesEx(p.paddle.rect, 4, 
                            (Color){ 100, 150, 255, (unsigned char)(pulse * 200) });
        DrawRectangleLinesEx(p.paddle.rect, 2, 
                            (Color){ 150, 200, 255, (unsigned char)(pulse * 100) });
    }
    if (p.paddle.hasLaser) {
        DrawRectangleLinesEx(p.paddle.rect, 2, LIME);
    }
    
    if (p.ball.active) {
        if (p.ball.fire) {
            DrawCircleV(p.ball.pos, p.ball.radius + 5, (Color){ 255, 100, 0, 100 });
            DrawCircleV(p.ball.pos, p.ball.radius + 3, ORANGE);
            DrawCircleV(p.ball.pos, p.ball.radius + 1, YELLOW);
        }
        if (p.ball.ghost) {
            DrawCircleV(p.ball.pos, p.ball.radius, (Color){ 200, 150, 255, 150 });
            DrawCircleV(p.ball.pos, p.ball.radius + 2, (Color){ 200, 150, 255, 50 });
        } else {
            DrawCircleV(p.ball.pos, p.ball.radius, WHITE);
            DrawCircleV(p.ball.pos, p.ball.radius * 0.6f, LIGHTGRAY);
        }
    }
    
    for (const auto &b : p.extraBalls) {
        if (b.active) DrawCircleV(b.pos, b.radius, LIGHTGRAY);
    }
    
    DrawParticles(p);
    DrawPowerUps(p);
    DrawHUD(p, offsetX, isPlayer2);
}

void DrawMenu(const Game &game) {
    ClearBackground(BLACK);
    DrawBackground(game);
    
    DrawText("DOUBLE BREAKOUT", SCREEN_WIDTH/2 - 200, 60, 55, WHITE);
    DrawText("双人打砖块", SCREEN_WIDTH/2 - 80, 120, 25, GRAY);
    
    DrawLine(SCREEN_WIDTH/2 - 250, 160, SCREEN_WIDTH/2 + 250, 160, (Color){ 50, 50, 100, 255 });
    
    const char *options[] = { "Start Game", "Select Level", "Settings", "Shop", "High Scores", "Exit" };
    for (int i = 0; i < 6; i++) {
        Color c = (i == game.menuSelection) ? YELLOW : WHITE;
        if (i == game.menuSelection) {
            DrawRectangle(SCREEN_WIDTH/2 - 120, 190 + i * 50, 240, 40, (Color){ 50, 50, 100, 100 });
            DrawRectangleLines(SCREEN_WIDTH/2 - 120, 190 + i * 50, 240, 40, (Color){ 100, 100, 200, 100 });
        }
        DrawText(options[i], SCREEN_WIDTH/2 - 60, 198 + i * 50, 24, c);
    }
    
    DrawText(TextFormat("Coins: %d", game.totalCoins), SCREEN_WIDTH/2 - 60, 530, 18, GOLD);
    DrawText("Use UP/DOWN to navigate, ENTER to select", SCREEN_WIDTH/2 - 200, 580, 14, GRAY);
}

void DrawLevelSelect(const Game &game) {
    ClearBackground(BLACK);
    DrawBackground(game);
    
    DrawText("SELECT LEVEL", SCREEN_WIDTH/2 - 100, 50, 35, WHITE);
    
    for (int i = 0; i < MAX_LEVELS; i++) {
        int row = i / 5;
        int col = i % 5;
        float x = 100 + col * 220;
        float y = 130 + row * 100;
        
        bool unlocked = i < game.unlockedLevel;
        Color c = (i == game.levelSelection) ? YELLOW : (unlocked ? WHITE : DARKGRAY);
        
        DrawRectangle(x, y, 200, 80, (Color){ 30, 30, 60, 200 });
        DrawRectangleLines(x, y, 200, 80, c);
        
        DrawText(TextFormat("Level %d", i + 1), x + 50, y + 10, 20, c);
        
        if (unlocked) {
            DrawText(TextFormat("%dx%d", game.levels[i].rows, game.levels[i].cols), 
                    x + 60, y + 40, 14, GRAY);
            DrawText(TextFormat("%.0fs", game.levels[i].timeLimit), 
                    x + 120, y + 40, 14, GRAY);
        } else {
            DrawText("LOCKED", x + 60, y + 40, 16, DARKGRAY);
        }
    }
    
    DrawText("Press ENTER to select, ESC to return", SCREEN_WIDTH/2 - 160, 600, 14, GRAY);
}

void DrawSettings(const Game &game) {
    ClearBackground(BLACK);
    DrawBackground(game);
    
    DrawText("SETTINGS", SCREEN_WIDTH/2 - 80, 40, 35, WHITE);
    
    const char *labels[] = { 
        "Difficulty", "Ball Speed", "Brick Rows", "Max Lives", "Game Time", 
        "Sound", "Particles", "Screen Shake", "Music Volume"
    };
    
    for (int i = 0; i < 9; i++) {
        Color c = (i == game.settingsSelection) ? YELLOW : WHITE;
        DrawText(labels[i], SCREEN_WIDTH/2 - 200, 100 + i * 45, 20, c);
        
        const char *val;
        switch (i) {
            case 0: val = TextFormat("%d", game.settings.difficulty); break;
            case 1: val = TextFormat("%.1f", game.settings.ballSpeed); break;
            case 2: val = TextFormat("%d", game.settings.brickRows); break;
            case 3: val = TextFormat("%d", game.settings.maxLives); break;
            case 4: val = TextFormat("%d s", game.settings.gameTime); break;
            case 5: val = game.settings.soundEnabled ? "ON" : "OFF"; break;
            case 6: val = game.settings.particlesEnabled ? "ON" : "OFF"; break;
            case 7: val = game.settings.screenShake ? "ON" : "OFF"; break;
            case 8: val = TextFormat("%.0f%%", game.settings.musicVolume * 100); break;
            default: val = "";
        }
        DrawText(val, SCREEN_WIDTH/2 + 100, 100 + i * 45, 20, c);
    }
    
    DrawText("Use UP/DOWN to select, LEFT/RIGHT to change", SCREEN_WIDTH/2 - 200, 580, 14, GRAY);
    DrawText("ESC to return", SCREEN_WIDTH/2 - 60, 610, 14, GRAY);
}

void DrawShop(const Game &game) {
    ClearBackground(BLACK);
    DrawBackground(game);
    
    DrawText("SHOP", SCREEN_WIDTH/2 - 40, 40, 35, GOLD);
    DrawText(TextFormat("Coins: %d", game.totalCoins), SCREEN_WIDTH/2 - 70, 80, 22, GOLD);
    
    const char *items[] = { 
        "Wide Paddle", "Fast Ball", "Extra Life", "Slow Ball", 
        "Multi Ball", "Fire Ball", "Shield", "Laser", 
        "Magnet", "Ghost Ball", "Big Ball"
    };
    int costs[] = { 50, 100, 150, 200, 250, 300, 500, 400, 350, 450, 600 };
    
    for (int i = 0; i < 11; i++) {
        int row = i / 2;
        int col = i % 2;
        float x = 150 + col * 450;
        float y = 130 + row * 45;
        
        Color c = (i == game.shopSelection) ? YELLOW : WHITE;
        bool owned = game.unlockedItems[i + 1];
        
        DrawText(items[i], x, y, 18, c);
        if (owned) {
            DrawText("OWNED", x + 250, y, 18, GREEN);
        } else {
            DrawText(TextFormat("%d coins", costs[i]), x + 250, y, 18, c);
        }
    }
    
    DrawText("Press ENTER to buy, ESC to return", SCREEN_WIDTH/2 - 160, 580, 14, GRAY);
}

void DrawHighScores(const Game &game) {
    ClearBackground(BLACK);
    DrawBackground(game);
    
    DrawText("HIGH SCORES", SCREEN_WIDTH/2 - 110, 40, 35, WHITE);
    
    DrawText("Rank  Name          Score   Level  Time  Combo", SCREEN_WIDTH/2 - 250, 100, 16, GRAY);
    
    for (size_t i = 0; i < game.highScores.size(); i++) {
        Color c = (i < 3) ? GOLD : WHITE;
        DrawText(TextFormat("%d.", (int)(i + 1)), SCREEN_WIDTH/2 - 250, 130 + i * 30, 16, c);
        DrawText(game.highScores[i].name.c_str(), SCREEN_WIDTH/2 - 210, 130 + i * 30, 16, c);
        DrawText(TextFormat("%d", game.highScores[i].score), SCREEN_WIDTH/2 - 30, 130 + i * 30, 16, c);
        DrawText(TextFormat("%d", game.highScores[i].level), SCREEN_WIDTH/2 + 60, 130 + i * 30, 16, c);
        DrawText(TextFormat("%.0f", game.highScores[i].time), SCREEN_WIDTH/2 + 120, 130 + i * 30, 16, c);
        DrawText(TextFormat("%d", game.highScores[i].combo), SCREEN_WIDTH/2 + 190, 130 + i * 30, 16, c);
    }
    
    DrawText("Press ESC to return", SCREEN_WIDTH/2 - 80, 600, 14, GRAY);
}

void DrawGameOver(const Game &game) {
    ClearBackground(BLACK);
    DrawBackground(game);
    
    DrawText("GAME OVER", SCREEN_WIDTH/2 - 140, 60, 50, RED);
    
    DrawRectangle(100, 150, 450, 200, (Color){ 0, 0, 50, 150 });
    DrawRectangleLines(100, 150, 450, 200, BLUE);
    DrawText("Player 1", 120, 160, 24, BLUE);
    DrawText(TextFormat("Score: %d", game.player1.paddle.score), 120, 195, 18, WHITE);
    DrawText(TextFormat("Level: %d", game.player1.level + 1), 120, 220, 18, WHITE);
    DrawText(TextFormat("Max Combo: %d", game.player1.maxCombo), 120, 245, 18, WHITE);
    DrawText(TextFormat("Bricks: %d/%d", game.player1.bricksDestroyed, game.player1.totalBricks), 120, 270, 18, WHITE);
    DrawText(game.player1.won ? "VICTORY!" : "DEFEATED", 120, 300, 20, game.player1.won ? GREEN : RED);
    
    DrawRectangle(650, 150, 450, 200, (Color){ 50, 0, 0, 150 });
    DrawRectangleLines(650, 150, 450, 200, RED);
    DrawText("Player 2", 670, 160, 24, RED);
    DrawText(TextFormat("Score: %d", game.player2.paddle.score), 670, 195, 18, WHITE);
    DrawText(TextFormat("Level: %d", game.player2.level + 1), 670, 220, 18, WHITE);
    DrawText(TextFormat("Max Combo: %d", game.player2.maxCombo), 670, 245, 18, WHITE);
    DrawText(TextFormat("Bricks: %d/%d", game.player2.bricksDestroyed, game.player2.totalBricks), 670, 270, 18, WHITE);
    DrawText(game.player2.won ? "VICTORY!" : "DEFEATED", 670, 300, 20, game.player2.won ? GREEN : RED);
    
    if (game.player1.paddle.score > game.player2.paddle.score) {
        DrawText("Player 1 Wins!", SCREEN_WIDTH/2 - 100, 400, 30, BLUE);
    } else if (game.player2.paddle.score > game.player1.paddle.score) {
        DrawText("Player 2 Wins!", SCREEN_WIDTH/2 - 100, 400, 30, RED);
    } else {
        DrawText("Draw!", SCREEN_WIDTH/2 - 40, 400, 30, WHITE);
    }
    
    DrawText(TextFormat("Coins earned: %d", (game.player1.paddle.score + game.player2.paddle.score) / 10), 
            SCREEN_WIDTH/2 - 120, 450, 20, GOLD);
    
    DrawText("Press ENTER to return to menu", SCREEN_WIDTH/2 - 150, 550, 16, GRAY);
}

void DrawGame(const Game &game) {
    BeginDrawing();
    
    switch (game.state) {
        case GameState::MENU:
            DrawMenu(game);
            break;
        case GameState::LEVEL_SELECT:
            DrawLevelSelect(game);
            break;
        case GameState::SETTINGS:
            DrawSettings(game);
            break;
        case GameState::SHOP:
            DrawShop(game);
            break;
        case GameState::HIGHSCORES:
            DrawHighScores(game);
            break;
        case GameState::PLAYING:
            ClearBackground(BLACK);
            DrawPlayer(game.player1, 0, false);
            DrawPlayer(game.player2, HALF_WIDTH, true);
            DrawText(TextFormat("FPS: %.0f", game.avgFps), SCREEN_WIDTH - 100, 5, 14, GREEN);
            DrawText(TextFormat("Min: %.0f Max: %.0f", game.minFps, game.maxFps), 
                    SCREEN_WIDTH - 150, 22, 12, GRAY);
            DrawText("P to pause", SCREEN_WIDTH - 90, 40, 12, GRAY);
            break;
        case GameState::PAUSED:
            DrawPlayer(game.player1, 0, false);
            DrawPlayer(game.player2, HALF_WIDTH, true);
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 180 });
            DrawText("PAUSED", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 30, 40, WHITE);
            DrawText("Press P to resume", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 20, GRAY);
            break;
        case GameState::GAMEOVER:
            DrawGameOver(game);
            break;
        default:
            ClearBackground(BLACK);
            DrawText("Loading...", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2, 20, WHITE);
            break;
    }
    
    EndDrawing();
}

void LoadHighScores(Game &game) {
    std::ifstream file("highscores.dat", std::ios::binary);
    if (file.is_open()) {
        HighScore hs;
        while (file.read((char*)&hs, sizeof(HighScore))) {
            game.highScores.push_back(hs);
        }
        file.close();
    }
}

void SaveHighScores(const Game &game) {
    std::ofstream file("highscores.dat", std::ios::binary);
    if (file.is_open()) {
        for (const auto &hs : game.highScores) {
            file.write((const char*)&hs, sizeof(HighScore));
        }
        file.close();
    }
}
