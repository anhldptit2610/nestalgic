#include "graphic.h"
#include "controller.h"

void Graphic::GetInput(bool *quit, bool *key)
{
    const uint8_t *keyState = nullptr;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            *quit = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdl.window))
            *quit = true;
    }

    keyState = SDL_GetKeyboardState(nullptr);
    key[BTN_A] = keyState[SDL_SCANCODE_X];
    key[BTN_B] = keyState[SDL_SCANCODE_Z];
    key[BTN_SELECT] = keyState[SDL_SCANCODE_SPACE];
    key[BTN_START] = keyState[SDL_SCANCODE_RETURN];
    key[BTN_UP] = keyState[SDL_SCANCODE_UP];
    key[BTN_DOWN] = keyState[SDL_SCANCODE_DOWN];
    key[BTN_LEFT] = keyState[SDL_SCANCODE_LEFT];
    key[BTN_RIGHT] = keyState[SDL_SCANCODE_RIGHT];
}

void Graphic::Draw(uint32_t *screenFrameBuffer, uint32_t *ptFrameBuffer)
{
    sdl.UpdateTexture(screenFrameBuffer, ptFrameBuffer);
    ui.Frame(&sdl);
    ui.Render(sdl.renderer);
}

Graphic::Graphic() : sdl(), ui(sdl.window, sdl.renderer)
{

}

Graphic::~Graphic()
{

}