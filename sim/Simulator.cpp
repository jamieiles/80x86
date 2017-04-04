#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>
#include <deque>

#include <SDL_events.h>

#include <boost/program_options.hpp>

#include "CPU.h"
#include "Display.h"
#include "SoftwareCPU.h"
#include "RTLCPU.h"

static std::map<int, char> key_to_scancode_map = {
    { SDLK_a, 0x1e },
    { SDLK_b, 0x30 },
    { SDLK_c, 0x2e },
    { SDLK_d, 0x20 },
    { SDLK_e, 0x12 },
    { SDLK_f, 0x21 },
    { SDLK_g, 0x22 },
    { SDLK_h, 0x23 },
    { SDLK_i, 0x17 },
    { SDLK_j, 0x24 },
    { SDLK_k, 0x25 },
    { SDLK_l, 0x26 },
    { SDLK_m, 0x32 },
    { SDLK_n, 0x31 },
    { SDLK_o, 0x18 },
    { SDLK_p, 0x19 },
    { SDLK_q, 0x10 },
    { SDLK_r, 0x13 },
    { SDLK_s, 0x1f },
    { SDLK_t, 0x1e },
    { SDLK_u, 0x14 },
    { SDLK_v, 0x16 },
    { SDLK_w, 0x2f },
    { SDLK_x, 0x11 },
    { SDLK_y, 0x2d },
    { SDLK_z, 0x2c },
    { SDLK_0, 0x0b },
    { SDLK_1, 0x02 },
    { SDLK_2, 0x03 },
    { SDLK_3, 0x04 },
    { SDLK_4, 0x05 },
    { SDLK_5, 0x06 },
    { SDLK_6, 0x07 },
    { SDLK_7, 0x08 },
    { SDLK_8, 0x09 },
    { SDLK_9, 0x0a },
    { SDLK_LEFTBRACKET, 0x1a },
    { SDLK_BACKSLASH, 0x2b },
    { SDLK_RIGHTBRACKET, 0x1b },
    { SDLK_BACKQUOTE, 0x29 },
    { SDLK_PERIOD, 0x34 },
    { SDLK_SLASH, 0x35 },
    { SDLK_RETURN, 0x1c },
    { SDLK_ESCAPE, 0x01 },
    { SDLK_TAB, 0x0f },
    { SDLK_SPACE, 0x39 },
    { SDLK_QUOTE, 0x28 },
    { SDLK_MINUS, 0x0c },
    { SDLK_EQUALS, 0x0d },
    { SDLK_COMMA, 0x33 },
    { SDLK_RIGHT, 0x4d },
    { SDLK_LEFT, 0x4b },
    { SDLK_DOWN, 0x50 },
    { SDLK_UP, 0x48 },
    { SDLK_SEMICOLON, 0x27 },
};

template <typename T>
class Simulator {
public:
    Simulator<T>(const std::string &bios_path,
                 const std::string &disk_image_path);
    void run();
private:
    void load_bios(const std::string &bios_path);
    void process_events();
    void set_stack_flags(uint16_t mask, uint16_t new_flags);
    void handle_vectors();
    int get_bios_interrupt();

    // BIOS Interrupt handlers
    void equipment_check();
    void video_services();
    void video_teletype_output();
    void video_current_state();
    void video_set_cursor_position();
    void video_scroll();
    void report_memory_size();
    void disk_services();
    void disk_reset();
    void disk_read();
    void disk_get_parameters();
    void disk_get_type();
    void disk_status();
    void system_services();
    void system_configuration_parameters();
    void system_extended_memory_size();
    void unsupported_bios_interrupt();
    void time_services();
    void time_system_clock_counter();
    void time_read_rtc_time();
    void time_read_rtc_date();
    void pci_installation_check();
    void keyboard_services();
    void keyboard_wait();
    void keyboard_status();
    void keyboard_shift_status();
    bool keypress_is_active(const SDL_Event &e);
    void clear_keypress(const SDL_Event &e);
    void return_keypress(bool block);

    T cpu;
    Display display;
    std::ifstream disk_image;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::deque<SDL_Event> active_keypresses;
};

template <typename T>
Simulator<T>::Simulator(const std::string &bios_path,
                        const std::string &disk_image_path)
    : cpu("simulator"),
    disk_image(disk_image_path, std::ios::binary),
    start_time(std::chrono::system_clock::now())
{
    cpu.reset();
    load_bios(bios_path);
}

template <typename T>
void Simulator<T>::load_bios(const std::string &bios_path)
{
    cpu.write_reg(CS, 0xff00);
    cpu.write_reg(IP, 0x0000);

    std::ifstream bios(bios_path, std::ios::binary);
    for (unsigned offs = 0; !bios.eof(); ++offs) {
        char v;
        bios.read(&v, 1);
        cpu.write_mem8(get_phys_addr(0xff00, offs), v);
    }

    cpu.write_mem8(get_phys_addr(0xf000, 0xfffe), 0xff);
    cpu.write_mem8(get_phys_addr(0xf000, 0x0002), 0xff);
    cpu.write_mem8(get_phys_addr(0xf000, 0x0000), 8);
}

template <typename T>
void Simulator<T>::run()
{
    for (unsigned cycle_count = 0; ; ++cycle_count) {
        if (cycle_count % 10000 == 0) {
            display.refresh();
            process_events();
        }
        handle_vectors();
        cpu.step();
    }
}

template <typename T>
int Simulator<T>::get_bios_interrupt()
{
    auto cs = cpu.read_reg(CS);
    auto ip = cpu.read_reg(IP);

    if (!(cs == 0xff00 && ip >= 0x400 && ip < 0x500))
        return 0;

    return ip - 0x400;
}

template <typename T>
void Simulator<T>::handle_vectors()
{
    auto vector = get_bios_interrupt();
    if (vector == 0)
        return;

    set_stack_flags(CF, 0);

    switch (vector) {
    case 0x15:
        system_services();
        break;
    case 0x1a:
        time_services();
        break;
    case 0x16:
        keyboard_services();
        break;
    case 0x17: // fallthrough
    case 0x14:
        //std::cerr << std::hex << "warning: ignoring int " <<
        //    static_cast<unsigned>(vector) << "h, ah=" <<
        //    static_cast<unsigned>(ah) << std::endl;
        break;
    case 0x11:
        equipment_check();
        break;
    case 0x10:
        video_services();
        break;
    case 0x12:
        report_memory_size();
        break;
    case 0x13:
        disk_services();
        break;
    default:
        unsupported_bios_interrupt();
    }
}

template <typename T>
void Simulator<T>::equipment_check()
{
    cpu.write_reg(AX, (3 << 4) | (1 << 0));
}

template <typename T>
void Simulator<T>::video_services()
{
    auto ah = cpu.read_reg(AH);

    switch (ah) {
    case 0xe:
        video_teletype_output();
        break;
    case 0xf:
        video_current_state();
        break;
    case 0x2:
        video_set_cursor_position();
        break;
    case 0x6:
        video_scroll();
        break;
    case 0x1:
        break;
    default:
        unsupported_bios_interrupt();
    }
}

template <typename T>
void Simulator<T>::video_teletype_output()
{
    display.write_char(cpu.read_reg(AL));
}

template <typename T>
void Simulator<T>::video_current_state()
{
    cpu.write_reg(AL, 0x07); // MDA
    cpu.write_reg(AH, 80); // 80 columns
    cpu.write_reg(BH, 0); // Page 0
}

template <typename T>
void Simulator<T>::video_set_cursor_position()
{
    display.set_cursor(cpu.read_reg(DH), cpu.read_reg(DL));
}

template <typename T>
void Simulator<T>::video_scroll()
{
    display.scroll(cpu.read_reg(AL));
}

template <typename T>
void Simulator<T>::report_memory_size()
{
    cpu.write_reg(AX, 640);
}

template <typename T>
void Simulator<T>::disk_services()
{
    auto ah = cpu.read_reg(AH);
    switch (ah) {
    case 0x0:
        disk_reset();
        break;
    case 0x1:
        disk_status();
        break;
    case 0x2:
        disk_read();
        break;
    case 0x8:
        disk_get_parameters();
        break;
    case 0x15:
        disk_get_type();
        break;
    case 0x41:
        set_stack_flags(CF, CF);
        break;
    default:
        unsupported_bios_interrupt();
    }
}

template <typename T>
void Simulator<T>::disk_reset()
{
    if (cpu.read_reg(DL) == 0)
        cpu.write_reg(AH, 0);
    else
        set_stack_flags(CF, CF);
}

template <typename T>
void Simulator<T>::disk_status()
{
    if (cpu.read_reg(DL) == 0)
        cpu.write_reg(AH, 0);
    else
        set_stack_flags(CF, CF);
}

template <typename T>
void Simulator<T>::disk_read()
{
    auto count = cpu.read_reg(AL);
    auto ch = cpu.read_reg(CH);
    auto cl = cpu.read_reg(CL);

    auto cylinder = ch | ((cl & 0xc0) << 2);
    auto sector = cl & 0x3f;
    auto head = cpu.read_reg(DH);
    auto drive = cpu.read_reg(DL);
    auto es = cpu.read_reg(ES);
    auto bx = cpu.read_reg(BX);

    if (drive != 0) {
        set_stack_flags(CF, CF);
        cpu.write_reg(AL, 0x80);
    } else {
        auto lba = (cylinder * 2 + head) * 0x12 + (sector - 1);
        if (lba < 0)
            lba = 0;
        disk_image.seekg(lba * 512, std::ios::beg);
        for (unsigned offset = 0; offset < count * 512; ++offset) {
            char v;
            disk_image.read(&v, 1);
            cpu.write_mem8(get_phys_addr(es, (bx + offset) & 0xffff), v);
        }
        cpu.write_reg(AH, 0);
    }
}

template <typename T>
void Simulator<T>::disk_get_parameters()
{
    cpu.write_reg(AH, 0);
    cpu.write_reg(BL, 4);
    cpu.write_reg(DH, 0);
    cpu.write_reg(DL, 1);
}

template <typename T>
void Simulator<T>::disk_get_type()
{
    if (cpu.read_reg(DL) == 0)
        cpu.write_reg(AH, 1);
    else
        set_stack_flags(CF, CF);
}

template <typename T>
void Simulator<T>::system_services()
{
    auto ah = cpu.read_reg(AH);

    switch (ah) {
    case 0xc0:
        system_configuration_parameters();
        break;
    case 0x41:
        // wait for external event
        break;
    case 0x88:
        system_extended_memory_size();
        break;
    case 0x24:
        // A20 gate
        set_stack_flags(CF, CF);
        break;
    default:
        unsupported_bios_interrupt();
    }
}

template <typename T>
void Simulator<T>::system_configuration_parameters()
{
    cpu.write_reg(AH, 0x80);
    cpu.write_reg(ES, 0xf000);
    cpu.write_reg(BX, 0);
}

template <typename T>
void Simulator<T>::system_extended_memory_size()
{
    cpu.write_reg(AX, 0);
}

template <typename T>
void Simulator<T>::time_services()
{
    auto ah = cpu.read_reg(AH);

    // Maintain the counter of times that int 1a'h was called.
    cpu.write_mem32(0x46c, cpu.read_mem32(0x46c) + 1);
    switch (ah) {
    case 0x0:
        time_system_clock_counter();
        break;
    case 0x1:
        // Set system clock counter
        break;
    case 0x3:
        // Set RTC time
        break;
    case 0x5:
        // Set RTC date
        break;
    case 0x2:
        time_read_rtc_time();
        break;
    case 0x4:
        time_read_rtc_date();
        break;
    case 0xb1:
        pci_installation_check();
        break;
    default:
        unsupported_bios_interrupt();
    }
}

template <typename T>
void Simulator<T>::time_system_clock_counter()
{
    auto now = std::chrono::system_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

    cpu.write_reg(DX, elapsed_seconds & 0xffff);
    cpu.write_reg(CX, (elapsed_seconds >> 16) & 0xffff);
}

template <typename T>
void Simulator<T>::time_read_rtc_time()
{
    auto time = std::time(nullptr);
    auto local_time = std::localtime(&time);

    cpu.write_reg(CH, local_time->tm_hour);
    cpu.write_reg(CL, local_time->tm_min);
    cpu.write_reg(DH, local_time->tm_sec);
}

template <typename T>
void Simulator<T>::time_read_rtc_date()
{
    auto time = std::time(nullptr);
    auto local_time = std::localtime(&time);

    cpu.write_reg(CH, local_time->tm_year % 100);
    cpu.write_reg(CL, local_time->tm_mon);
    cpu.write_reg(DH, local_time->tm_mday);
}

template <typename T>
void Simulator<T>::pci_installation_check()
{
    set_stack_flags(CF, CF);
    cpu.write_reg(AH, 0x86);
}

template <typename T>
void Simulator<T>::keyboard_services()
{
    auto ah = cpu.read_reg(AH);

    switch (ah) {
    case 0x0:
        keyboard_wait();
        break;
    case 0x1:
        keyboard_status();
        break;
    case 0x2:
        keyboard_shift_status();
        break;
    default:
        unsupported_bios_interrupt();
    }
}

template <typename T>
void Simulator<T>::return_keypress(bool block)
{
    auto k = active_keypresses[0];
    auto scancode = key_to_scancode_map[static_cast<int>(k.key.keysym.sym)];
    auto character = k.key.keysym.sym;

    if (k.key.keysym.mod & KMOD_SHIFT)
        character ^= 0x20;
    if (k.key.keysym.mod & KMOD_CTRL)
        character ^= 0x40;
    if (k.key.keysym.mod & KMOD_ALT)
        character ^= 0xff;
    cpu.write_reg(AH, scancode);
    cpu.write_reg(AL, character);
    set_stack_flags(ZF, 0);

    if (block)
        active_keypresses.erase(active_keypresses.begin());
}

template <typename T>
void Simulator<T>::keyboard_wait()
{
    while (!active_keypresses.size())
        process_events();

    return_keypress(true);
}

template <typename T>
void Simulator<T>::keyboard_status()
{
    process_events();

    if (!active_keypresses.size()) {
        set_stack_flags(ZF, ZF);
        cpu.write_reg(AX, 0);
        return;
    }

    return_keypress(false);
}

template <typename T>
void Simulator<T>::keyboard_shift_status()
{
    cpu.write_reg(AL, 0x1);
}

template <typename T>
bool Simulator<T>::keypress_is_active(const SDL_Event &e)
{
    for (auto i = active_keypresses.begin(); i != active_keypresses.end(); ++i)
        if ((*i).key.keysym.scancode == e.key.keysym.scancode &&
            (*i).key.keysym.sym == e.key.keysym.sym &&
            (*i).key.keysym.mod == e.key.keysym.mod)
            return true;

    return false;
}

template <typename T>
void Simulator<T>::clear_keypress(const SDL_Event &e)
{
    if (active_keypresses.size() < 100)
        return;

    for (auto i = active_keypresses.begin(); i != active_keypresses.end(); ++i) {
        if ((*i).key.keysym.scancode == e.key.keysym.scancode &&
            (*i).key.keysym.sym == e.key.keysym.sym &&
            (*i).key.keysym.mod == e.key.keysym.mod) {
            active_keypresses.erase(i);
            break;
        }
    }
}

template <typename T>
void Simulator<T>::process_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            exit(0);
        else if (e.type == SDL_KEYDOWN &&
                 !keypress_is_active(e) &&
                 key_to_scancode_map.find(e.key.keysym.sym) != key_to_scancode_map.end())
            active_keypresses.push_back(e);
        else if (e.type == SDL_KEYUP)
            clear_keypress(e);
    }
}

template <typename T>
void Simulator<T>::set_stack_flags(uint16_t mask, uint16_t new_flags)
{
    // Interrupt frame
    auto flags_addr = get_phys_addr(cpu.read_reg(SS), cpu.read_reg(SP) + 4);
    auto flags = cpu.read_mem16(flags_addr);
    flags &= ~mask;
    flags |= (mask & new_flags);
    cpu.write_mem16(flags_addr, flags);
}

template <typename T>
void Simulator<T>::unsupported_bios_interrupt()
{
    auto vector = get_bios_interrupt();
    auto ah = cpu.read_reg(AH);
    std::stringstream ss;

    ss << "Invalid BIOS interrupt " << std::hex << static_cast<unsigned>(vector) <<
        "h, ah=" << static_cast<unsigned>(ah) << "h";

    throw std::runtime_error(ss.str());
}

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    std::string bios_image;
    std::string disk_image;
    std::string backend = "SoftwareCPU";

    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "print this usage information and exit")
        ("backend,b", po::value<std::string>(&backend),
         "the simulator backend module to use, either SoftwareCPU or RTLCPU, default SoftwareCPU")
        ("bios", po::value<std::string>(&bios_image)->required(),
         "the bios image to use")
        ("diskimage", po::value<std::string>(&disk_image)->required(),
         "the boot disk image");

    po::positional_options_description positional;
    positional.add("bios", 1);
    positional.add("diskimage", 1);

    po::variables_map variables_map;
    try {
        po::store(po::command_line_parser(argc, argv)
                    .options(desc).positional(positional).run(),
                  variables_map);
        if (variables_map.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        po::notify(variables_map);
    } catch (boost::program_options::required_option &e) {
        std::cerr << "Error: missing arguments " << e.what() << std::endl;
        return 1;
    } catch (boost::program_options::error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }

    if (backend == "SoftwareCPU") {
        Simulator<SoftwareCPU> sim(bios_image, disk_image);
        sim.run();
    } else if (backend == "RTLCPU") {
        Simulator<RTLCPU<verilator_debug_enabled>> sim(bios_image, disk_image);
        sim.run();
    } else {
        std::cerr << "Error: invalid simulation backend \"" << backend << "\"" << std::endl;
        return 3;
    }

    return 0;
}
