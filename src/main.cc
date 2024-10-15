#include "nestalgic.h" 
// #include "graphic/graphic.h"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

int main(int argc, char *argv[])
{
    Nestalgic nestalgic(argv[1]);

    while (!nestalgic.Quit()) {
        nestalgic.RunFrame();
    }
//     // setup SDL
//     if (SDL_Init(SDL_INIT_EVERYTHING)) {
//         printf("Error: %s\n", SDL_GetError());
//         return -1;
//     }

// #ifdef SDL_HINT_IME_SHOW_UI
//     SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
// #endif

//     SDL_Window *window = SDL_CreateWindow("hello world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
//     if (window == nullptr) {
//         printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
//         return -1;
//     }
//     SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
//     if (renderer == nullptr) {
//         printf("Error creating SDL_Renderer!");
//         return -1;
//     }

//     // setup Dear ImGui context
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO(); (void)io;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

//     ImGui::StyleColorsDark();

//     ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
//     ImGui_ImplSDLRenderer2_Init(renderer);

//     bool showDemoWindow = true;
//     bool showAnotherWindow = false;
//     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

//     bool done = false;
//     while (!done) {
//         SDL_Event event;
//         while (SDL_PollEvent(&event)) {
//             ImGui_ImplSDL2_ProcessEvent(&event);
//             if (event.type == SDL_QUIT)
//                 done = true;
//             if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
//                 done = true;
//         }
//         if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
//             SDL_Delay(10);
//             continue;
//         }

//         ImGui_ImplSDLRenderer2_NewFrame();
//         ImGui_ImplSDL2_NewFrame();
//         ImGui::NewFrame();

//         if (showDemoWindow)
//             ImGui::ShowDemoWindow(&showDemoWindow);

//         {
//             ImGui::Begin("Hello, world!");
//             ImGui::Text("This is it");
//             ImGui::End();
//         }

//         // Rendering
//         ImGui::Render();
//         SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
//         SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
//         SDL_RenderClear(renderer);
//         ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
//         SDL_RenderPresent(renderer);
//     }

//     // Cleanup
//     ImGui_ImplSDLRenderer2_Shutdown();
//     ImGui_ImplSDL2_Shutdown();
//     ImGui::DestroyContext();

//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     SDL_Quit();
    return 0;
}