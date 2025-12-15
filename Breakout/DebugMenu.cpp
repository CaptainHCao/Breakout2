#include "DebugMenu.h"
#include "imgui.h"

void renderDebugMenu(
    bool& showDebugMenu,
    Paddle& paddle,
    Ball& ball
) {
    if (!showDebugMenu) return;

    ImGui::SetNextWindowPos(ImVec2(10, 90), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGui::Begin("DEBUG Upgrades", &showDebugMenu,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings);

    const SDL_FRect pRect = paddle.getRect();
    const SDL_FRect bRect = ball.getRect();

    ImGui::Text("Paddle width: %.1f", pRect.w);
    ImGui::Text("Ball size: %.1f", bRect.w);
    ImGui::Text("Ball strong: %s", ball.isStrong() ? "YES" : "no");

    ImGui::Separator();
    ImGui::Text("Activate upgrades");

    if (ImGui::Button("Bigger Paddle"))
        paddle.makeBigger(1.25f);

    if (ImGui::Button("Faster Paddle"))
        paddle.makeFaster(1.20f);

    if (ImGui::Button("Bigger Ball"))
        ball.makeBigger(1.25f);

    if (ImGui::Button("Strong Ball (5 hits)"))
        ball.makeStrong(5);

    ImGui::Separator();

    if (ImGui::Button("Apply ALL"))
    {
        paddle.makeBigger(1.25f);
        paddle.makeFaster(1.20f);
        ball.makeBigger(1.25f);
        ball.makeStrong(10);
    }

    ImGui::End();
}
