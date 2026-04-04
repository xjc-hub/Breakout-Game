#include "Ball.h"
#include "raylib.h"
#include "ConfigManager.h"
#include <iostream>
#include <algorithm>

Ball::Ball(Vector2 pos, Vector2 sp, float r) 
    : position(pos), velocity(sp), radius(r), launched(false) {
}

void Ball::Move() {
    position.x += velocity.x;
    position.y += velocity.y;
}

void Ball::BounceEdge(int screenWidth, int screenHeight) {
    // 左右边界反弹
    if (position.x <= radius) {
        position.x = radius;
        velocity.x = -velocity.x;
    } else if (position.x >= screenWidth - radius) {
        position.x = screenWidth - radius;
        velocity.x = -velocity.x;
    }
    
    // 上边界反弹
    if (position.y <= radius) {
        position.y = radius;
        velocity.y = -velocity.y;
    }
}

void Ball::BouncePaddle(Rectangle paddleRect) {
    // 计算球击中挡板的相对位置
    float hitPoint = (position.x - paddleRect.x) / paddleRect.width;
    hitPoint = hitPoint * 2.0f - 1.0f; // 转换为 -1 到 1 的范围
    
    // 限制 hitPoint 在合理范围内
    hitPoint = std::clamp(hitPoint, -1.0f, 1.0f);
    
    // 根据击中位置调整反弹角度
    velocity.x = hitPoint * ConfigManager::GetFloat("ballSpeedX", 8.0f);
    velocity.y = -std::abs(velocity.y); // 总是向上反弹
    
    // 限制最大速度
    float maxSpeed = ConfigManager::GetFloat("ballMaxSpeed", 15.0f);
    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
    if (velocity.y > maxSpeed) velocity.y = maxSpeed;
    if (velocity.y < -maxSpeed) velocity.y = -maxSpeed;
}

void Ball::BounceBrick(Rectangle brickRect) {
    // 简单地从砖块反弹（与挡板反弹类似）
    velocity.y = -velocity.y;
    
    // 稍微调整X方向速度，增加随机性
    velocity.x += (GetRandomValue(-10, 10) / 100.0f);
    
    // 限制最大速度
    float maxSpeed = ConfigManager::GetFloat("ballMaxSpeed", 15.0f);
    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
    if (velocity.y > maxSpeed) velocity.y = maxSpeed;
    if (velocity.y < -maxSpeed) velocity.y = -maxSpeed;
}

void Ball::Launch(float paddleX, float paddleY) {
    position.x = paddleX;
    position.y = paddleY - radius;
    velocity.x = 0.0f;
    velocity.y = -ConfigManager::GetFloat("ballLaunchSpeed", 10.0f);
    launched = true;
}

void Ball::ResetToPaddle(float paddleX, float paddleY) {
    position.x = paddleX;
    position.y = paddleY - radius;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    launched = false;
}

void Ball::Draw() {
    DrawCircle(position.x, position.y, radius, WHITE);
}

Vector2 Ball::GetPosition() const {
    return position;
}

float Ball::GetRadius() const {
    return radius;
}

bool Ball::IsLaunched() const {
    return launched;
}
