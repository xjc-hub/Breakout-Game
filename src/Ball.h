#pragma once

#include "raylib.h"

class Ball {
private:
    Vector2 position;
    Vector2 speed;  // 注意：这里应该是 speed，不是 velocity
    float radius;
    bool launched;

public:
    Ball(Vector2 pos, Vector2 sp, float r);
    
    void Move();
    void BounceEdge(int screenWidth, int screenHeight);
    void BouncePaddle(Rectangle paddleRect);
    void BounceBrick(Rectangle brickRect);
    void Launch(float paddleX, float paddleY);
    void ResetToPaddle(float paddleX, float paddleY);
    void Draw();
    
    Vector2 GetPosition() const { return position; }
    float GetRadius() const { return radius; }
    bool IsLaunched() const { return launched; }
};
