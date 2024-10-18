#include "graphic.h"

void Graphic::GetInput(bool *quit)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            *quit = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdl.window))
            *quit = true;
    }
}

void Graphic::Draw(uint32_t *ptFrameBuffer)
{
    sdl.UpdateTexture(ptFrameBuffer);
    ui.Frame(&sdl);
    ui.Render(sdl.renderer);
}

Graphic::Graphic() : sdl(), ui(sdl.window, sdl.renderer)
{

}

Graphic::~Graphic()
{

}