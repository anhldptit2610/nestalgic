#include "sdl.h"


void SDL::UpdateTexture(uint32_t *screenFrameBuffer, uint32_t *ptFrameBuffer)
{
    SDL_UpdateTexture(patternTable, nullptr, ptFrameBuffer, PATTERN_TABLE_WIDTH * 4);
    SDL_UpdateTexture(screen, nullptr, screenFrameBuffer, SCREEN_WIDTH * 4);
}

SDL::SDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("Error: %s\n", SDL_GetError());
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    window = SDL_CreateWindow("hello world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        SDL_Log("Error: SDL_CreateWindow() - %s\n", SDL_GetError());
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Error: SDL_CreateRenderer() - %s\n", SDL_GetError());
    }
    patternTable = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                        PATTERN_TABLE_WIDTH, PATTERN_TABLE_HEIGHT);
    if (!patternTable) {
        SDL_Log("Error: SDL_CreateTexture() - %s\n", SDL_GetError());
    }
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                        SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!screen) {
        SDL_Log("Error: SDL_CreateTexture() - %s\n", SDL_GetError());
    }
}

SDL::~SDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}