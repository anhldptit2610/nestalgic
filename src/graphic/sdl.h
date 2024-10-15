#ifndef SDL_H_
#define SDL_H_

#include <SDL2/SDL.h>

#define PATTERN_TABLE_WIDTH         (16 * 8 * 2)
#define PATTERN_TABLE_HEIGHT        (16 * 8)

struct SDL {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *patternTable = nullptr;

    /* methods */

    /* constructor, destructor */
    SDL();
    ~SDL();
};

#endif