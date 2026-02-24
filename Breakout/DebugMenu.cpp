#include "DebugMenu.h"
#include "imgui.h"

void renderDebugMenu(
    bool& showDebugMenu,
    SDL_Renderer* renderer,   
    bool& vsyncEnabled,
    Paddle& paddle,
    Ball& ball,
    std::vector<UpgradePickup>& pickups
) {
    if (!showDebugMenu) return;

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGui::Begin("DEBUG Upgrades", &showDebugMenu,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings);

    const SDL_FRect pRect = paddle.getRect();
    const SDL_FRect bRect = ball.getRect();

    ImGui::Text("Paddle width: %.1f", pRect.w);
    ImGui::Text("Paddle speed: %.1f", paddle.getSpeed());
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

    ImGui::Separator();
    ImGui::Text("Falling pickups: %d", (int)pickups.size());
    for (int i = 0; i < (int)pickups.size(); ++i) {
        const auto& p = pickups[i];
        ImGui::BulletText("#%d type=%d y=%.1f alive=%d",
            i, (int)p.type, p.rect.y, (int)p.alive);
    }

    ImGui::Separator();
    ImGui::Text("Rendering");

    if (ImGui::Checkbox("VSync Enabled", &vsyncEnabled))
    {
        SDL_SetRenderVSync(renderer, vsyncEnabled ? 1 : 0);
    }

    ImGui::End();
}
