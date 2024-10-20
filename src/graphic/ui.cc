#include "ui.h"

void UI::Frame(SDL *pSDL)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("pattern table");
    ImGui::Image(reinterpret_cast<ImTextureID>(pSDL->patternTable),
                 ImVec2(PATTERN_TABLE_WIDTH * 2, PATTERN_TABLE_HEIGHT * 2));
    ImGui::End();

    ImGui::Begin("screen");
    ImGui::Image(reinterpret_cast<ImTextureID>(pSDL->screen),
                ImVec2(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2));
    ImGui::End();

    ImGui::Begin("sprite table");
    ImGui::Image(reinterpret_cast<ImTextureID>(pSDL->objTable),
                ImVec2(OBJ_TABLE_WIDTH * 4, OBJ_TABLE_HEIGHT * 4));
    ImGui::End();
}

void UI::Render(SDL_Renderer *pRenderer)
{
    ImGui::Render();
    SDL_RenderSetScale(pRenderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(pRenderer, (uint8_t)(bgColor.x * 255), (uint8_t)(bgColor.y * 255), (uint8_t)(bgColor.z * 255), (uint8_t)(bgColor.w * 255));
    SDL_RenderClear(pRenderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), pRenderer);
    SDL_RenderPresent(pRenderer);
}

UI::UI(SDL_Window *pWindow, SDL_Renderer *pRenderer) 
{ 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& rIO = ImGui::GetIO();
    io = &rIO;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(pWindow, pRenderer);
    ImGui_ImplSDLRenderer2_Init(pRenderer);
    
    bgColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

UI::~UI() { }