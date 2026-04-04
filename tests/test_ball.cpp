#ifdef SIMPLE_TEST
#include "simple_test.hpp"
#else
#include "catch_amalgamated.hpp"
#endif

#include "Ball.h"

TEST_CASE("Ball initialization") {
    Ball ball({100.0f, 200.0f}, {5.0f, -5.0f}, 10.0f);
    
    REQUIRE(ball.GetPosition().x == 100.0f);
    REQUIRE(ball.GetPosition().y == 200.0f);
    REQUIRE(ball.GetRadius() == 10.0f);
    REQUIRE_FALSE(ball.IsLaunched());
}

TEST_CASE("Ball movement") {
    Ball ball({100.0f, 200.0f}, {3.0f, 4.0f}, 10.0f);
    ball.Launch(100.0f, 50.0f);
    
    REQUIRE(ball.IsLaunched());
    
    ball.Move();
    REQUIRE(ball.GetPosition().x == 103.0f);
    REQUIRE(ball.GetPosition().y == 204.0f);
}

TEST_CASE("Ball bounce from edges") {
    Ball ball({790.0f, 300.0f}, {5.0f, 0.0f}, 10.0f);
    ball.Launch(790.0f, 300.0f);
    
    // 应该从右边缘反弹
    ball.BounceEdge(800, 600);
    ball.Move();
    
    // X 方向应该反转
    REQUIRE(ball.GetPosition().x == 785.0f); // 790 - 5
}

TEST_CASE("Ball reset to paddle") {
    Ball ball({400.0f, 300.0f}, {5.0f, 5.0f}, 10.0f);
    ball.Launch(400.0f, 300.0f);
    
    REQUIRE(ball.IsLaunched());
    
    ball.ResetToPaddle(350.0f, 550.0f);
    REQUIRE_FALSE(ball.IsLaunched());
    REQUIRE(ball.GetPosition().x == 350.0f);
    REQUIRE(ball.GetPosition().y == 550.0f - 10.0f); // 减去半径
}

