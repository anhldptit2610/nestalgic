#ifndef UI_H_
#define UI_H_

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "sdl.h"

class UI {
private:
    ImGuiIO *io = nullptr;
    ImVec4 bgColor;
public:
    void Frame(SDL *);
    void Render(SDL_Renderer *);
    /* constructor, destructor */
    UI(SDL_Window *, SDL_Renderer *) ;
    ~UI();
};

#endif