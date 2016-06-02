#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <SDL.h>
#include <stdexcept>

class Window {
public:
    Window(const char *name, int width, int height)
        : width(width),
        height(height)
    {
        window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  width, height, SDL_WINDOW_SHOWN);
        if (!window)
            throw std::runtime_error("unable to create window");
    }

    SDL_Surface *surface()
    {
        return SDL_GetWindowSurface(window);
    }

    SDL_Window *get()
    {
        return window;
    }

    ~Window()
    {
        SDL_DestroyWindow(window);
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
};

#endif // __WINDOW_H__
