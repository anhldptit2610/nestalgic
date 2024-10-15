#ifndef SDL_H_
#define SDL_H_

#include <SDL2/SDL.h>

struct SDL {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    /* methods */

    /* constructor, destructor */
    SDL();
    ~SDL();
};

#endif