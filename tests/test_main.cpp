#ifdef SIMPLE_TEST
#include "simple_test.hpp"
#else
#include "catch_amalgamated.hpp"
#endif

// 声明测试函数
void test_Ball_initialization();
void test_Ball_movement();
void test_Ball_bounce_from_edges();
void test_Ball_reset_to_paddle();

void test_Paddle_initialization();
void test_Paddle_movement_left();
void test_Paddle_movement_right();

void test_Brick_initialization();
void test_Brick_deactivation();

void test_Leaderboard_basic_operations();
void test_Leaderboard_max_entries();

// 如果没有使用 Catch2，创建简单的主函数
#ifdef SIMPLE_TEST
int main() {
    std::cout << "Running tests..." << std::endl;
    
    // 运行 Ball 测试
    test_Ball_initialization();
    test_Ball_movement();
    test_Ball_bounce_from_edges();
    test_Ball_reset_to_paddle();
    
    // 运行 Paddle 测试
    test_Paddle_initialization();
    test_Paddle_movement_left();
    test_Paddle_movement_right();
    
    // 运行 Brick 测试
    test_Brick_initialization();
    test_Brick_deactivation();
    
    // 运行 Leaderboard 测试
    test_Leaderboard_basic_operations();
    test_Leaderboard_max_entries();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
#endif

// 如果使用 Catch2
#ifndef SIMPLE_TEST
#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#endif
