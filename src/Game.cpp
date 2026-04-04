#include "Game.h"
#include "raylib.h"
#include <iostream>

Game::Game() 
    : ball({0,0}, {0,0}, 0),
      paddle(0, 0, 0, 0),
      currentState(MENU),
      score(0),
      lives(3),
      winCount(0),
      gameTime(0.0f) {
    
    // ????
    ConfigManager::LoadConfig(configFile);
    
    // ?????
    FontManager::GetInstance().LoadChineseFont();
    
    // ??????
    leaderboard = Leaderboard(ConfigManager::GetString("scoreFileName", "scores.txt").c_str());
}

void Game::Initialize() {
    // ???????
    ResetBallAndPaddle();
    
    // ?????
    InitializeBricks();
    
    // ???????
    currentState = MENU;
}

void Game::Update() {
    // ???????????????
    switch (currentState) {
        case MENU:
            UpdateMenu();
            break;
        case PLAYING:
            UpdatePlaying();
            break;
        case PAUSED:
            UpdatePaused();
            break;
        case GAMEOVER:
            UpdateGameOver();
            break;
        case VICTORY:
            UpdateVictory();
            break;
        case LEADERBOARD:
            UpdateLeaderboard();
            break;
    }
}

void Game::Draw() {
    // ???????????????
    switch (currentState) {
        case MENU:
            DrawMenu();
            break;
        case PLAYING:
            DrawPlaying();
            break;
        case PAUSED:
            DrawPaused();
            break;
        case GAMEOVER:
            DrawGameOver();
            break;
        case VICTORY:
            DrawVictory();
            break;
        case LEADERBOARD:
            DrawLeaderboard();
            break;
    }
}

void Game::ProcessInput() {
    // ???????????????
    if (IsKeyPressed(KEY_R)) {
        if (currentState == GAMEOVER || currentState == VICTORY) {
            Reset();
            ChangeState(MENU);
        }
    }
    
    if (IsKeyPressed(KEY_L)) {
        if (currentState == MENU || currentState == PLAYING || currentState == PAUSED) {
            ChangeState(LEADERBOARD);
        } else if (currentState == LEADERBOARD) {
            ChangeState(MENU);
        }
    }
    
    // ?????????
    switch (currentState) {
        case MENU:
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                Reset();
                ChangeState(PLAYING);
            }
            break;
            
        case PLAYING:
            // ????
            float currentSpeed = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) 
                ? ConfigManager::GetFloat("boostSpeed", 28.0f) 
                : ConfigManager::GetFloat("normalSpeed", 18.0f);
            
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                paddle.MoveLeft(currentSpeed);
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                paddle.MoveRight(currentSpeed);
            }
            
            // ???
            if (IsKeyPressed(KEY_SPACE) && !ball.IsLaunched()) {
                ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
            }
            
            // ??
            if (IsKeyPressed(KEY_P)) {
                ChangeState(PAUSED);
            }
            break;
            
        case PAUSED:
            if (IsKeyPressed(KEY_P)) {
                ChangeState(PLAYING);
            }
            break;
            
        case GAMEOVER:
        case VICTORY:
            // ?R????????
            break;
            
        case LEADERBOARD:
            // ?L????????
            break;
    }
}

void Game::ChangeState(GameState newState) {
    // ????????????????
    if (currentState == LEADERBOARD && newState != LEADERBOARD) {
        // ???????????
    }
    
    // ???????????
    std::cout << "State changed from " << currentState << " to " << newState << std::endl;
    
    // ????
    currentState = newState;
}

void Game::Reset() {
    score = 0;
    lives = ConfigManager::GetInt("initialLives", 3);
    winCount = 0;
    gameTime = 0.0f;
    
    // ??????
    ResetBallAndPaddle();
    
    // ????
    InitializeBricks();
}

// ==================== ?????? ====================

void Game::UpdateMenu() {
    // ?????????????
}

void Game::UpdatePlaying() {
    gameTime += GetFrameTime();
    
    // ??????
    if (ball.IsLaunched()) {
        ball.Move();
        ball.BounceEdge(ConfigManager::GetInt("screenWidth", 800), 
                       ConfigManager::GetInt("screenHeight", 600));
    } else {
        ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, 
                          paddle.GetRect().y);
    }
    
    // ???????
    if (ball.GetPosition().y > ConfigManager::GetInt("screenHeight", 600)) {
        LoseLife();
    }
    
    // ????
    CheckCollisions();
    
    // ??????
    if (winCount >= ConfigManager::GetInt("brickRows", 5) * ConfigManager::GetInt("brickCols", 8)) {
        ChangeState(VICTORY);
        if (leaderboard.CanEnter(score)) {
            leaderboard.AddScore("Player", score);
        }
    }
    
    // ????????
    if (lives <= 0) {
        ChangeState(GAMEOVER);
        if (leaderboard.CanEnter(score)) {
            leaderboard.AddScore("Player", score);
        }
    }
}

void Game::UpdatePaused() {
    // ?????????????
}

void Game::UpdateGameOver() {
    // ???????????????
}

void Game::UpdateVictory() {
    // ?????????????
}

void Game::UpdateLeaderboard() {
    // ??????????????
}

// ==================== ?????? ====================

void Game::DrawMenu() {
    ClearBackground(BLACK);
    
    FontManager& fontMgr = FontManager::GetInstance();
    
    // ????
    fontMgr.DrawChineseTextCentered("?????", 150, 60, YELLOW);
    
    // ????
    fontMgr.DrawChineseTextCentered("??????????", 300, 30, WHITE);
    fontMgr.DrawChineseTextCentered("?L?????", 350, 30, WHITE);
    fontMgr.DrawChineseTextCentered("?ESC??", 400, 30, WHITE);
    
    // ??????
    fontMgr.DrawChineseText("????:", 50, 450, 24, GREEN);
    fontMgr.DrawChineseText("左右箭头/A/D: 移动挡板", 70, 480, 20, WHITE);
    fontMgr.DrawChineseText("Shift: ????", 70, 510, 20, WHITE);
    fontMgr.DrawChineseText("空格: 发射球/开始游戏", 70, 540, 20, WHITE);
    fontMgr.DrawChineseText("P: 暂停/继续", 70, 570, 20, WHITE);
}

void Game::DrawPlaying() {
    ClearBackground(BLACK);
    
    // ????
    for (auto& brick : bricks) {
        if (brick.IsActive()) {
            brick.Draw();
        }
    }
    
    // ??????
    ball.Draw();
    paddle.Draw();
    
    // ??UI
    FontManager& fontMgr = FontManager::GetInstance();
    
    fontMgr.DrawChineseText(("??: " + std::to_string(score)).c_str(), 10, 10, 20, WHITE);
    fontMgr.DrawChineseText(("??: " + std::to_string(lives)).c_str(), 10, 40, 20, WHITE);
    fontMgr.DrawChineseText(("??: " + std::to_string((int)gameTime) + "?").c_str(), 10, 70, 20, WHITE);
    
    // ????
    if (!ball.IsLaunched()) {
        fontMgr.DrawChineseTextCentered("?????", 450, 24, WHITE);
    }
}

void Game::DrawPaused() {
    // ???????
    DrawPlaying();
    
    // ????????
    DrawRectangle(0, 0, ConfigManager::GetInt("screenWidth", 800), 
                 ConfigManager::GetInt("screenHeight", 600), {0, 0, 0, 150});
    
    FontManager& fontMgr = FontManager::GetInstance();
    fontMgr.DrawChineseTextCentered("????", 250, 48, YELLOW);
    fontMgr.DrawChineseTextCentered("?P????", 320, 24, WHITE);
    fontMgr.DrawChineseTextCentered("?L?????", 370, 24, WHITE);
}

void Game::DrawGameOver() {
    // ???????
    DrawPlaying();
    
    // ????????
    DrawRectangle(0, 0, ConfigManager::GetInt("screenWidth", 800), 
                 ConfigManager::GetInt("screenHeight", 600), {0, 0, 0, 150});
    
    FontManager& fontMgr = FontManager::GetInstance();
    fontMgr.DrawChineseTextCentered("????", 250, 48, RED);
    fontMgr.DrawChineseTextCentered(("????: " + std::to_string(score)).c_str(), 320, 36, WHITE);
    fontMgr.DrawChineseTextCentered("?R?????", 370, 24, WHITE);
}

void Game::DrawVictory() {
    // ???????
    DrawPlaying();
    
    // ????????
    DrawRectangle(0, 0, ConfigManager::GetInt("screenWidth", 800), 
                 ConfigManager::GetInt("screenHeight", 600), {0, 0, 0, 150});
    
    FontManager& fontMgr = FontManager::GetInstance();
    fontMgr.DrawChineseTextCentered("???", 250, 48, GREEN);
    fontMgr.DrawChineseTextCentered(("????: " + std::to_string(score)).c_str(), 320, 36, WHITE);
    fontMgr.DrawChineseTextCentered("???????????", 370, 24, WHITE);
    fontMgr.DrawChineseTextCentered("?R?????", 420, 24, WHITE);
}

void Game::DrawLeaderboard() {
    ClearBackground(BLACK);
    
    FontManager& fontMgr = FontManager::GetInstance();
    
    // ????
    fontMgr.DrawChineseTextCentered("???", 100, 48, YELLOW);
    
    // ???????
    ScoreEntry entry;
    int yPos = 150;
    for (int i = 1; i <= leaderboard.GetCount(); i++) {
        if (leaderboard.GetEntry(i, entry)) {
            std::string text = std::to_string(i) + ". " + entry.name + " - " + std::to_string(entry.score);
            fontMgr.DrawChineseTextCentered(text.c_str(), yPos, 30, WHITE);
            yPos += 40;
        }
    }
    
    // ????
    fontMgr.DrawChineseTextCentered("?L??", 500, 24, WHITE);
}

// ==================== ???? ====================

void Game::InitializeBricks() {
    bricks.clear();
    for (int row = 0; row < ConfigManager::GetInt("brickRows", 5); row++) {
        for (int col = 0; col < ConfigManager::GetInt("brickCols", 8); col++) {
            bricks.emplace_back(
                ConfigManager::GetInt("brickStartX", 50) + col * ConfigManager::GetInt("brickSpacingX", 95),
                ConfigManager::GetInt("brickStartY", 80) + row * ConfigManager::GetInt("brickSpacingY", 35),
                ConfigManager::GetInt("brickWidth", 85),
                ConfigManager::GetInt("brickHeight", 25),
                brickColors[row]
            );
        }
    }
    winCount = 0;
}

void Game::ResetBallAndPaddle() {
    // ???
    ball = Ball(
        {ConfigManager::GetFloat("ballStartX", 400.0f), 
         ConfigManager::GetFloat("ballStartY", 530.0f)},
        {0.0f, 0.0f},
        ConfigManager::GetFloat("ballRadius", 10.0f)
    );
    
    // ????
    paddle = Paddle(
        ConfigManager::GetFloat("paddleStartX", 340.0f),
        ConfigManager::GetFloat("paddleStartY", 550.0f),
        ConfigManager::GetFloat("paddleWidth", 120.0f),
        ConfigManager::GetFloat("paddleHeight", 15.0f)
    );
}

void Game::CheckCollisions() {
    // ??????
    if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), paddle.GetRect())) {
        ball.BouncePaddle(paddle.GetRect());
    }
    
    // ??????
    for (auto& brick : bricks) {
        if (brick.IsActive() && CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), brick.GetRect())) {
            brick.SetActive(false);
            ball.BounceBrick(brick.GetRect());
            AddScore(ConfigManager::GetInt("baseScorePerBrick", 10));
            winCount++;
            break;
        }
    }
}

void Game::AddScore(int baseScore) {
    score += CalculateScore(baseScore, gameTime);
}

void Game::LoseLife() {
    lives--;
    score -= ConfigManager::GetInt("lifePenalty", 50);
    ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
}

int Game::CalculateScore(int baseScore, float time) {
    float multiplier = ConfigManager::GetFloat("scoreMultiplier", 5.0f);
    float penalty = ConfigManager::GetFloat("timePenalty", 0.05f);
    return static_cast<int>(baseScore * multiplier - time * penalty);
}




