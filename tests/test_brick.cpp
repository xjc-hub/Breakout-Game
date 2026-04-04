#ifdef SIMPLE_TEST
#include "simple_test.hpp"
#else
#include "catch_amalgamated.hpp"
#endif

#include "Brick.h"

TEST_CASE("Brick initialization") {
    Brick brick(100.0f, 200.0f, 80.0f, 25.0f, RED);
    
    auto rect = brick.GetRect();
    REQUIRE(rect.x == 100.0f);
    REQUIRE(rect.y == 200.0f);
    REQUIRE(rect.width == 80.0f);
    REQUIRE(rect.height == 25.0f);
    REQUIRE(brick.IsActive());
}

TEST_CASE("Brick deactivation") {
    Brick brick(100.0f, 200.0f, 80.0f, 25.0f, RED);
    
    REQUIRE(brick.IsActive());
    
    brick.SetActive(false);
    REQUIRE_FALSE(brick.IsActive());
    
    brick.SetActive(true);
    REQUIRE(brick.IsActive());
}

