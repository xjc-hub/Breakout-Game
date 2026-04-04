#ifdef SIMPLE_TEST
#include "simple_test.hpp"
#else
#include "catch_amalgamated.hpp"
#endif

#include "Leaderboard.h"
#include <cstdio>

TEST_CASE("Leaderboard basic operations") {
    // 使用临时文件
    const char* tempFile = "test_scores.txt";
    
    // 删除可能存在的旧文件
    remove(tempFile);
    
    Leaderboard leaderboard(tempFile);
    
    REQUIRE(leaderboard.GetCount() == 0);
    REQUIRE(leaderboard.CanEnter(100) == true);
    
    // 添加分数
    int rank = leaderboard.AddScore("Player1", 100);
    REQUIRE(rank == 1);
    REQUIRE(leaderboard.GetCount() == 1);
    
    // 添加更高分数应该排在前面
    rank = leaderboard.AddScore("Player2", 200);
    REQUIRE(rank == 1);
    REQUIRE(leaderboard.GetCount() == 2);
    
    // 添加更低分数应该排在后面
    rank = leaderboard.AddScore("Player3", 50);
    REQUIRE(rank == 3);
    REQUIRE(leaderboard.GetCount() == 3);
    
    // 测试获取条目
    ScoreEntry entry;
    REQUIRE(leaderboard.GetEntry(1, entry) == true);
    REQUIRE(strcmp(entry.name, "Player2") == 0);
    REQUIRE(entry.score == 200);
    
    REQUIRE(leaderboard.GetEntry(2, entry) == true);
    REQUIRE(strcmp(entry.name, "Player1") == 0);
    REQUIRE(entry.score == 100);
    
    // 清理临时文件
    remove(tempFile);
}

TEST_CASE("Leaderboard max entries") {
    const char* tempFile = "test_scores2.txt";
    remove(tempFile);
    
    Leaderboard leaderboard(tempFile);
    
    // 添加10个分数
    for (int i = 1; i <= 10; i++) {
        leaderboard.AddScore("Player", i * 100);
    }
    
    REQUIRE(leaderboard.GetCount() == 10);
    
    // 第11个分数（50）太低，不应该进入排行榜
    int rank = leaderboard.AddScore("Player11", 50);
    REQUIRE(rank == 0);
    REQUIRE(leaderboard.GetCount() == 10);
    
    // 第11个分数（550）够高，应该进入排行榜
    rank = leaderboard.AddScore("Player11", 550);
    REQUIRE(rank == 1);
    REQUIRE(leaderboard.GetCount() == 10);
    
    // 检查最低分是否被移除
    ScoreEntry entry;
    REQUIRE(leaderboard.GetEntry(10, entry) == true);
    REQUIRE(entry.score == 200); // 原来的100应该被移除
    
    remove(tempFile);
}

