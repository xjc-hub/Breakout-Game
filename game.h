#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;
const int HALF_WIDTH = SCREEN_WIDTH / 2;

const int PADDLE_WIDTH = 12;
const int PADDLE_HEIGHT = 90;
const int BALL_SIZE = 8;
const int BRICK_WIDTH = 50;
const int BRICK_HEIGHT = 18;
const int MAX_PARTICLES = 500;
const int MAX_HIGHSCORES = 10;
const int MAX_LEVELS = 20;

const int GRID_COLS = 12;
const int GRID_ROWS = 8;
const int CELL_W = HALF_WIDTH / GRID_COLS;
const int CELL_H = SCREEN_HEIGHT / GRID_ROWS;

enum class PowerUpType {
    NONE, WIDE_PADDLE, FAST_BALL, EXTRA_LIFE, 
    SLOW_BALL, MULTI_BALL, FIRE_BALL, SHIELD,
    LASER, MAGNET, GHOST_BALL, BIG_BALL
};

enum class BrickType {
    NORMAL, HARD, METAL, GOLD_BRICK, BOMB, RAINBOW,
    ICE, FIRE, POISON, HEAL, MYSTERY, SPIKE
};

enum class EnemyType {
    NONE, PATROL, CHASER, SHOOTER, BOSS
};

struct Particle {
    Vector2 pos, vel;
    Color color;
    float life, maxLife;
    bool active;
    float size;
};

struct PowerUp {
    Vector2 pos, speed;
    PowerUpType type;
    bool active;
    float timer;
    Color color;
    float pulse;
};

struct Paddle {
    Rectangle rect;
    int score, lives;
    float originalWidth, powerUpTimer;
    bool shielded, hasLaser;
    float shieldTimer, laserTimer;
    std::vector<Vector2> laserBeams;
};

struct Ball {
    Vector2 pos, speed;
    float radius;
    bool active, fire, ghost;
    float speedMultiplier;
    Color trail[10];
    int trailIndex;
};

struct Brick {
    Rectangle rect;
    bool active;
    Color color;
    int hitPoints, maxHitPoints;
    BrickType type;
    float animTimer, glowTimer;
    Vector2 originalPos;
};

struct Enemy {
    Rectangle rect;
    EnemyType type;
    bool active;
    int health;
    float speed, shootTimer;
    Color color;
    Vector2 direction;
};

struct PlayerGame {
    Paddle paddle;
    Ball ball;
    std::vector<Brick> bricks;
    std::vector<Ball> extraBalls;
    Particle particles[MAX_PARTICLES];
    std::vector<PowerUp> powerUps;
    std::vector<Enemy> enemies;
    float gameTime, collisionTime, drawTime;
    bool gameOver, won;
    int combo, level, maxCombo;
    float lastHitTime, shakeTimer, comboDisplayTimer;
    Vector2 shakeOffset;
    int bricksDestroyed, totalBricks;
    float accuracy;
    int totalShots, hits;
};

enum class GameState {
    MENU, SETTINGS, PLAYING, PAUSED, GAMEOVER, SHOP, HIGHSCORES, LEVEL_SELECT, TUTORIAL
};

struct Settings {
    int difficulty;
    float ballSpeed;
    int brickRows, brickCols, maxLives, gameTime;
    bool soundEnabled, particlesEnabled, screenShake, vsync;
    float musicVolume, sfxVolume;
};

struct HighScore {
    std::string name;
    int score;
    int level;
    float time;
    int combo;
};

struct LevelData {
    int rows, cols;
    int hardCount, metalCount, goldCount;
    bool hasBombs, hasRainbow, hasEnemies;
    float timeLimit;
    std::string name;
};

struct Game {
    PlayerGame player1, player2;
    GameState state;
    Settings settings;
    float avgFps, minFps, maxFps;
    int frameCount;
    float fpsAccum;
    int menuSelection, settingsSelection, shopSelection, levelSelection;
    std::vector<HighScore> highScores;
    int totalCoins;
    int unlockedItems[12];
    float bgScroll, bgScroll2;
    float globalTime;
    bool showTutorial;
    int tutorialStep;
    LevelData levels[MAX_LEVELS];
    int unlockedLevel;
};

void InitGame(Game &game);
void InitPlayer(PlayerGame &p, int offsetX, int level);
void UpdateGame(Game &game);
void UpdatePlayer(PlayerGame &p, int offsetX, bool isPlayer2, float dt);
void DrawGame(const Game &game);
void DrawPlayer(const PlayerGame &p, int offsetX, bool isPlayer2);
void DrawMenu(const Game &game);
void DrawSettings(const Game &game);
void DrawGameOver(const Game &game);
void DrawShop(const Game &game);
void DrawHighScores(const Game &game);
void DrawLevelSelect(const Game &game);
void DrawTutorial(const Game &game);
void SpawnParticles(PlayerGame &p, Vector2 pos, Color color, int count, float size = 3);
void UpdateParticles(PlayerGame &p, float dt);
void DrawParticles(const PlayerGame &p);
void SpawnPowerUp(PlayerGame &p, Vector2 pos);
void UpdatePowerUps(PlayerGame &p, float dt);
void DrawPowerUps(const PlayerGame &p);
void ResetBall(Ball &ball, int offsetX, float speed);
void LoadHighScores(Game &game);
void SaveHighScores(const Game &game);
void GenerateLevel(PlayerGame &p, int offsetX, int level);
void DrawBackground(const Game &game);
void DrawHUD(const PlayerGame &p, int offsetX, bool isPlayer2);
void DrawBrickWithEffect(const Brick &brick, float globalTime);
void UpdateEnemies(PlayerGame &p, int offsetX, float dt);
void DrawEnemies(const PlayerGame &p);
void DrawLaserBeams(const PlayerGame &p);
void DrawBallTrail(const PlayerGame &p);
void DrawComboEffect(const PlayerGame &p);
void DrawLevelTransition(const Game &game);
void SpawnEnemy(PlayerGame &p, int offsetX, EnemyType type, float y);

#endif
