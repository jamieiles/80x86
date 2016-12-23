#pragma once

#include <SDL_ttf.h>

static const int font_size = 19;

class Font {
public:
    Font(const std::string &path)
    {
        font = TTF_OpenFont(path.c_str(), font_size);
        if (!font)
            throw std::runtime_error("unable to open font");
    }

    ~Font()
    {
        TTF_CloseFont(font);
    }

    TTF_Font *get()
    {
        return font;
    }

    int get_font_size() const
    {
        return font_size;
    }

    size_t glyph_width() const
    {
        int maxx;

        if (TTF_GlyphMetrics(font, 'x', nullptr, &maxx, nullptr, nullptr,
                             nullptr))
            throw std::runtime_error("unable to get font metrics");

        return static_cast<size_t>(maxx);
    }
private:
    TTF_Font *font;
};
