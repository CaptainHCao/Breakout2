// Menu.cpp
#include "Menu.h"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "Breakout.h"

void Menu::render(SDL_Renderer *renderer,
                  bool soundOn, bool musicOn, int highscore,
                  bool &startGame, bool &quit,
                  int logicalWidth, int logicalHeight)
{
    ImGuiIO &io = ImGui::GetIO();

    // ---- WINDOW POSITION & SIZE (CENTERED) ----
    const float menuWidth = 420.0f;
    const float menuHeight = 260.0f;

    // Top-left coordinate so that the window is centered
    ImVec2 pos(
        (logicalWidth - menuWidth) * 0.5f,
        (logicalHeight - menuHeight) * 0.5f);

    ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);

    // ---- STYLE OVERRIDES FOR THIS WINDOW ONLY ----
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.02f, 0.06f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.90f, 0.30f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.00f, 0.45f, 0.45f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.95f, 0.8f, 1.0f));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Breakout", nullptr, flags))
    {
        ImGui::Spacing();
        ImGui::Spacing();

        // Title
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("BREAKOUT").x) * 0.5f);
        ImGui::Text("BREAKOUT");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Highscore: %d", highscore);
        ImGui::Spacing();

        // Big buttons
        ImVec2 buttonSize(220.0f, 40.0f);

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - buttonSize.x) * 0.5f);
        if (ImGui::Button("Start Game", buttonSize))
        {
            startGame = true;
        }

        ImGui::Spacing();

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - buttonSize.x) * 0.5f);
        if (ImGui::Button("Quit Game", buttonSize))
        {
            quit = true;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Options:");
        ImGui::BulletText("Sound:  %s", soundOn ? "ON" : "OFF");
        ImGui::BulletText("Music:  %s", musicOn ? "ON" : "OFF");
    }
    ImGui::End();

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(3);
}

void Menu::handleEvent(const SDL_Event &event,
                       bool &soundOn, bool &musicOn, int &highscore,
                       bool &startGame, bool &quit)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        switch (event.key.key)
        {
        case SDLK_ESCAPE:
            quit = true;
            break;
        case SDLK_O:
            soundOn = !soundOn;
            break;
        case SDLK_M:
            musicOn = !musicOn;
            break;
        case SDLK_H: /* show highscore */
            break;
        case SDLK_RETURN:
            startGame = true;
            break;
        }
    }
}