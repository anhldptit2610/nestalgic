#ifndef SDL_H_
#define SDL_H_

#include <SDL2/SDL.h>
#include "common.h"


struct SDL {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *patternTable = nullptr;
    SDL_Texture *screen = nullptr;
    SDL_Texture *objTable = nullptr;
    SDL_Texture *tileMap = nullptr;

    /* methods */
    void UpdateTexture(uint32_t *, uint32_t *, uint32_t *, uint32_t *);
    /* constructor, destructor */
    SDL();
    ~SDL();
};

#endif