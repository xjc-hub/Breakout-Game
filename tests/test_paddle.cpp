#ifdef SIMPLE_TEST
#include "simple_test.hpp"
#else
#include "catch_amalgamated.hpp"
#endif

#include "Paddle.h"

TEST_CASE("Paddle initialization") {
    Paddle paddle(300.0f, 550.0f, 120.0f, 15.0f);
    
    auto rect = paddle.GetRect();
    REQUIRE(rect.x == 300.0f);
    REQUIRE(rect.y == 550.0f);
    REQUIRE(rect.width == 120.0f);
    REQUIRE(rect.height == 15.0f);
}

TEST_CASE("Paddle movement left") {
    Paddle paddle(300.0f, 550.0f, 120.0f, 15.0f);
    
    paddle.MoveLeft(10.0f);
    REQUIRE(paddle.GetRect().x == 290.0f);
    
    // 移动到左边界
    for (int i = 0; i < 30; i++) paddle.MoveLeft(10.0f);
    REQUIRE(paddle.GetRect().x == 0.0f); // 不应该小于0
}

TEST_CASE("Paddle movement right") {
    Paddle paddle(300.0f, 550.0f, 120.0f, 15.0f);
    
    paddle.MoveRight(10.0f);
    REQUIRE(paddle.GetRect().x == 310.0f);
    
    // 移动到右边界（假设屏幕宽度800）
    for (int i = 0; i < 50; i++) paddle.MoveRight(10.0f);
    REQUIRE(paddle.GetRect().x == 800.0f - 120.0f); // 不应该超过屏幕宽度
}

