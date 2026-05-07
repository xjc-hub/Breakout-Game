// ===== ???????????? =====
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
enum class LoadState { IDLE, LOADING, DONE };
LoadState loadState = LoadState::IDLE;
std::future<void> loadFuture;
std::mutex loadMutex;
Color brickColor = RED;
void SimulateLoading() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
// ? Update() ??????
void UpdateAsyncLoading() {
    if (IsKeyPressed(KEY_L) && loadState == LoadState::IDLE) {
        loadState = LoadState::LOADING;
        loadFuture = std::async(std::launch::async, SimulateLoading);
    }
    if (loadState == LoadState::LOADING &&
        loadFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        loadFuture.get();
        std::lock_guard<std::mutex> lock(loadMutex);
        loadState = LoadState::DONE;
        brickColor = GREEN; // ?????????
    }
}
// ? Draw() ??????
void DrawAsyncLoading() {
    if (loadState == LoadState::LOADING) {
        DrawText("Loading...", screenWidth/2 - 100, screenHeight/2, 40, WHITE);
    } else if (loadState == LoadState::DONE) {
        DrawText("Loaded! Press L again to reload.", screenWidth/2 - 180, screenHeight/2 + 50, 20, GREEN);
    }
}
