// 简单测试框架
#ifndef SIMPLE_TEST_HPP
#define SIMPLE_TEST_HPP

#include <iostream>
#include <cassert>

// 移除 SIMPLE_TEST 定义，避免重复
#undef SIMPLE_TEST

// 使用不同的宏名称避免冲突
#define SIMPLE_TEST_CASE(name) void test_##name()
#define SIMPLE_REQUIRE(expr) if (!(expr)) { std::cout << "FAIL: " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; exit(1); }
#define SIMPLE_REQUIRE_FALSE(expr) SIMPLE_REQUIRE(!(expr))

#endif
