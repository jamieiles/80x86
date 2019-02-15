// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

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
                                               0, &window,
                                               &renderer);
        if (ret)
            throw std::runtime_error("unable to create window" + std::string(SDL_GetError()));

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
