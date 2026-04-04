#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

struct ScoreEntry {
    char name[32];
    int score;
    time_t timestamp;
};

class Leaderboard {
private:
    static const int MAX_ENTRIES = 10;
    ScoreEntry entries[MAX_ENTRIES];
    int count;
    const char* filename;
    
public:
    Leaderboard(const char* file);
    void Load();
    void Save();
    int AddScore(const char* name, int score);
    bool GetEntry(int rank, ScoreEntry& entry);
    int GetCount();
    bool CanEnter(int score);
};
