#ifndef SDL_H_
#define SDL_H_

#include <SDL2/SDL.h>
#include "common.h"


struct SDL {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *patternTable = nullptr;

    /* methods */
    void UpdateTexture(uint32_t *);
    /* constructor, destructor */
    SDL();
    ~SDL();
};

#endif