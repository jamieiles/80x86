#pragma once

#include <SDL.h>
#include <stdexcept>

class Window {
public:
    const float x_scale_factor = 3;
    const float y_scale_factor = 6;

    Window(const char *name, int width, int height)
        : width(width),
        height(height)
    {
        auto ret = SDL_CreateWindowAndRenderer(width * x_scale_factor,
                                               height * y_scale_factor,
                                               SDL_WINDOWPOS_UNDEFINED,
                                               &window, &renderer);
        if (ret)
            throw std::runtime_error("unable to create window");

        SDL_SetWindowTitle(window, name);
        SDL_RenderSetScale(renderer, x_scale_factor, y_scale_factor);
    }

    ~Window()
    {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }

    SDL_Surface *surface()
    {
        return SDL_GetWindowSurface(window);
    }

    SDL_Window *get()
    {
        return window;
    }

    void clear()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    void set_pixel(int row, int col, unsigned char r, unsigned char g,
                   unsigned char b)
    {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, row, col);
    }

    void redraw()
    {
        SDL_RenderPresent(renderer);
    }

    int get_width() const
    {
        return width;
    }

    int get_height() const
    {
        return height;
    }
private:
    int width;
    int height;
    SDL_Window *window;
    SDL_Renderer *renderer;
};
