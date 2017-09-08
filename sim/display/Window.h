#pragma once

#include <SDL.h>
#include <stdexcept>

class Window
{
public:
    Window(const char *name, int width, int height)
        : width(width), height(height)
    {
        compute_scale();

        auto ret = SDL_CreateWindowAndRenderer(window_width, window_height,
                                               SDL_WINDOWPOS_UNDEFINED, &window,
                                               &renderer);
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

    void set_pixel(int row,
                   int col,
                   unsigned char r,
                   unsigned char g,
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
    void compute_scale()
    {
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);

        window_width = display_mode.w / 2;
        window_height = ((float)height / (float)width) * window_width * 2;

        x_scale_factor = window_width / (float)width;
        y_scale_factor = window_height / (float)height;
    }

    int width;
    int height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    float x_scale_factor;
    float y_scale_factor;
    int window_width;
    int window_height;
};
