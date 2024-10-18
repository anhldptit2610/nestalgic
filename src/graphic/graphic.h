#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include "ui.h"
#include "sdl.h"

class Graphic {
private:
    SDL sdl;
    UI ui;
    uint8_t *patternTable = nullptr;
public:
    void GetInput(bool *);
    void Draw(uint32_t *);
    /* constructor, destructor */
    Graphic();
    ~Graphic();
};

#endif