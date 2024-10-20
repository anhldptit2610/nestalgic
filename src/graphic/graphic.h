#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include "ui.h"
#include "sdl.h"

class Graphic {
private:
    SDL sdl;
    UI ui;
public:
    void GetInput(bool *, bool *);
    void Draw(uint32_t *, uint32_t *, uint32_t *);
    /* constructor, destructor */
    Graphic();
    ~Graphic();
};

#endif