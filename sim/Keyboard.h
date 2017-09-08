#pragma once

#include <stdint.h>
#include "Simulator.h"

#include <SDL_events.h>

#include <map>
#include <deque>

#include "SoftwareCPU.h"
#include "../bios/bda.h"

static std::map<int, std::vector<unsigned char>> sdl_to_keyboard = {
    {SDLK_a, {0x1c}},
    {SDLK_b, {0x32}},
    {SDLK_c, {0x21}},
    {SDLK_d, {0x23}},
    {SDLK_e, {0x24}},
    {SDLK_f, {0x2b}},
    {SDLK_g, {0x34}},
    {SDLK_h, {0x33}},
    {SDLK_i, {0x43}},
    {SDLK_j, {0x3b}},
    {SDLK_k, {0x42}},
    {SDLK_l, {0x4b}},
    {SDLK_m, {0x3a}},
    {SDLK_n, {0x31}},
    {SDLK_o, {0x44}},
    {SDLK_p, {0x4d}},
    {SDLK_q, {0x15}},
    {SDLK_r, {0x2d}},
    {SDLK_s, {0x1b}},
    {SDLK_t, {0x2c}},
    {SDLK_u, {0x3c}},
    {SDLK_v, {0x2a}},
    {SDLK_w, {0x1d}},
    {SDLK_x, {0x22}},
    {SDLK_y, {0x35}},
    {SDLK_z, {0x1a}},
    {SDLK_1, {0x16}},
    {SDLK_2, {0x1e}},
    {SDLK_3, {0x26}},
    {SDLK_4, {0x25}},
    {SDLK_5, {0x2e}},
    {SDLK_6, {0x36}},
    {SDLK_7, {0x3d}},
    {SDLK_8, {0x3e}},
    {SDLK_9, {0x46}},
    {SDLK_0, {0x45}},
    {SDLK_MINUS, {0x4e}},
    {SDLK_EQUALS, {0x55}},
    {SDLK_LEFTBRACKET, {0x54}},
    {SDLK_RIGHTBRACKET, {0x5b}},
    {SDLK_SEMICOLON, {0x4c}},
    {SDLK_QUOTE, {0x52}},
    {SDLK_BACKQUOTE, {0x0e}},
    {SDLK_BACKSLASH, {0x5d}},
    {SDLK_COMMA, {0x41}},
    {SDLK_PERIOD, {0x49}},
    {SDLK_SLASH, {0x4a}},
    {SDLK_F1, {0x05}},
    {SDLK_F2, {0x06}},
    {SDLK_F3, {0x04}},
    {SDLK_F4, {0x0c}},
    {SDLK_F5, {0x03}},
    {SDLK_F6, {0x0b}},
    {SDLK_F7, {0x83}},
    {SDLK_F8, {0x0a}},
    {SDLK_F9, {0x01}},
    {SDLK_F10, {0x09}},
    {SDLK_F11, {0x78}},
    {SDLK_F12, {0x07}},
    {SDLK_BACKSPACE, {0x66}},
    {SDLK_DELETE, {0xe0, 0x71}},
    {SDLK_DOWN, {0xe0, 0x72}},
    {SDLK_END, {0xe0, 0x69}},
    {SDLK_RETURN, {0x5a}},
    {SDLK_ESCAPE, {0x76}},
    {SDLK_HOME, {0xe0, 0x6c}},
    {SDLK_INSERT, {0xe0, 0x70}},
    {SDLK_LEFT, {0xe0, 0x6b}},
    {SDLK_PAGEDOWN, {0xe0, 0x7a}},
    {SDLK_PAGEUP, {0xe0, 0x7d}},
    {SDLK_PRINTSCREEN, {0xe0, 0x12, 0xe0, 0x7c}},
    {SDLK_RIGHT, {0xe0, 0x74}},
    {SDLK_SPACE, {0x29}},
    {SDLK_TAB, {0x0d}},
    {SDLK_UP, {0xe0, 0x75}},
    {SDLK_LCTRL, {0x14}},
    {SDLK_LALT, {0x11}},
    {SDLK_LSHIFT, {0x12}},
};

template <typename CPU_t>
class Keyboard : public IOPorts
{
public:
    Keyboard(CPU_t *cpu) : IOPorts(0x0060, 1), cpu(cpu) {}

    void write8(uint16_t __unused port_num,
                unsigned __unused offs,
                uint8_t __unused v)
    {
        if (pending.size())
            pending.pop_front();
        if (pending.size() == 0)
            cpu->clear_irq(1);
    }

    uint8_t read8(uint16_t __unused port_num, unsigned __unused offs)
    {
        if (offs)
            return 0;

        return pending.size() ? pending.front() : 0;
    }

    void set_scancode(uint8_t v)
    {
        cpu->raise_irq(1);

        pending.push_back(v);
    }

    void process_event(SDL_Event e)
    {
        if (sdl_to_keyboard.count(e.key.keysym.sym) == 0)
            return;

        for (auto b : sdl_to_keyboard[e.key.keysym.sym]) {
            if (e.type == SDL_KEYUP)
                set_scancode(0xf0);
            set_scancode(b);
        }
    }

private:
    CPU_t *cpu;
    std::deque<uint8_t> pending;
};
