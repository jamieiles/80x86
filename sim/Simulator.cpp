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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>

#include <boost/program_options.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "CGA.h"
#include "CPU.h"
#include "Display.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "SoftwareCPU.h"
#include "RTLCPU.h"
#include "PIC.h"
#include "UART.h"
#include "SPI.h"
#include "Timer.h"

bool trace = false;

namespace tty
{
const std::string green = "\x1b[32m";
const std::string normal = "\x1b[39;49;0m";
const std::string bold = "\x1b[1m";
};

class SDRAMConfigRegister : public IOPorts
{
public:
    SDRAMConfigRegister() : IOPorts(0xfffc, 1)
    {
    }

    virtual ~SDRAMConfigRegister()
    {
    }

    void write8(uint16_t __unused port_num,
                unsigned __unused offs,
                uint8_t __unused v)
    {
    }
    uint8_t read8(uint16_t __unused port_num, unsigned offs)
    {
        return offs == 0 ? 0x1 : 0;
    }
};

template <typename T>
class Simulator
{
public:
    Simulator<T>(const std::string &bios_path,
                 const std::string &disk_image_path,
                 bool detached);
    void run();

private:
    void load_bios(const std::string &bios_path);
    void process_io();
    void trace_insn(uint16_t cs, uint16_t ip, size_t instr_len);
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & cpu;
        ar & uart;
        ar & spi;
        ar & timer;
        ar & cga;
        ar & pic;
        // clang-format on
    }

    T cpu;
    PIC pic;
    SDRAMConfigRegister sdram_config_register;
    UART uart;
    SPI spi;
    TimerTick timer;
    CGA cga;
    Keyboard kbd;
    Mouse mouse;
    bool got_exit;
    bool detached;
};

template <typename T>
Simulator<T>::Simulator(const std::string &bios_path,
                        const std::string &disk_image_path,
                        bool detached)
    : cpu("simulator"),
      pic(&this->cpu),
      spi(disk_image_path),
      timer(&this->pic),
      cga(this->cpu.get_memory()),
      kbd(&this->pic),
      mouse(&this->pic),
      got_exit(false),
      detached(detached)
{
    cpu.add_ioport(&sdram_config_register);
    cpu.add_ioport(&uart);
    cpu.add_ioport(&spi);
    cpu.add_ioport(&timer);
    cpu.add_ioport(&cga);
    cpu.add_ioport(&kbd);
    cpu.add_ioport(&pic);
    cpu.add_ioport(&mouse);
    cpu.reset();
    load_bios(bios_path);
}

template <typename T>
void Simulator<T>::load_bios(const std::string &bios_path)
{
    std::cout << tty::bold << tty::green << "Loading bios: " << bios_path
              << tty::normal << "\r\n";

    std::ifstream bios(bios_path, std::ios::binary);
    for (unsigned offs = 0; !bios.eof(); ++offs) {
        char v;
        bios.read(&v, 1);
        cpu.write_mem8(0xfc00, offs, v);
    }

    cpu.write_mem8(0xf000, 0xfffe, 0xff);
    cpu.write_mem8(0xf000, 0x0002, 0xff);
    cpu.write_mem8(0xf000, 0x0000, 8);

    cpu.write_reg(CS, 0xffff);
    cpu.write_reg(IP, 0x0000);
}

template <typename T>
void Simulator<T>::process_io()
{
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 0x1d)
            got_exit = true;
        else
            uart.add_char(c);
    }

    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            got_exit = true;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
            kbd.process_event(e);
        else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
            mouse.process_event(e);
    }

    mouse.update();

    auto kbd_state = SDL_GetKeyboardState(NULL);
    if (kbd_state[SDL_SCANCODE_LCTRL] && kbd_state[SDL_SCANCODE_LALT] &&
        kbd_state[SDL_SCANCODE_RIGHTBRACKET])
        got_exit = true;

    if (kbd_state[SDL_SCANCODE_LCTRL] && kbd_state[SDL_SCANCODE_LALT] &&
        kbd_state[SDL_SCANCODE_M])
        mouse.toggle_capture();
}

template <typename T>
void Simulator<T>::run()
{
    auto start_time = std::chrono::system_clock::now();

    if (detached)
        cpu.debug_detach();

    unsigned long last_cycle = 0;
    while (!got_exit) {
        auto io_callback = [&](unsigned long cycle_num) {
            if (cycle_num % 1000000 == 0)
                cga.update();
            if (cycle_num % 1000 == 0)
                process_io();
            timer.tick((cycle_num - last_cycle) + 1);
            last_cycle = cycle_num;
        };

        if (detached)
            cpu.cycle_cpu_with_io(io_callback);
        else {
            auto cs = cpu.read_reg(CS);
            auto ip = cpu.read_reg(IP);
            auto instr_len = cpu.step_with_io(io_callback);

            if (trace)
                trace_insn(cs, ip, instr_len);
        }
    }

    auto end_time = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end_time - start_time;

    std::cout << tty::bold << tty::green << "\r\nOperating frequency: "
              << (cpu.cycle_count() / 1000000.0) / elapsed_seconds.count()
              << "MHz\r\n"
              << tty::normal;
}

template <typename T>
void Simulator<T>::trace_insn(uint16_t cs, uint16_t ip, size_t instr_len)
{
    std::cout << "[" << std::hex << cs << ":" << ip << "] ";
    auto bytes = cpu.read_vector8(cs, ip, instr_len);

    auto flags = std::cout.flags();

    for (auto b : bytes)
        std::cout << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<unsigned>(b) << " ";

    std::cout.flags(flags);
    std::cout << "\r\n";
}

template <typename T>
static void run_sim(const std::string &bios_image,
                    const std::string &disk_image,
                    const bool detached,
                    const std::string &save,
                    const std::string &restore)
{
    T sim(bios_image, disk_image, detached);

    if (restore != "") {
        std::ifstream ifs(restore);
        boost::archive::text_iarchive ia(ifs);
        ia >> sim;
    }

    sim.run();

    if (save != "") {
        std::ofstream ofs(save);
        {
            boost::archive::text_oarchive oa(ofs);
            oa << sim;
        }
    }
}

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    std::string bios_image;
    std::string disk_image;
    std::string restore;
    std::string save;
    std::string backend = "SoftwareCPU";
    bool detached;

    po::options_description desc("Options");
    // clang-format off
    desc.add_options()
        ("help,h", "print this usage information and exit")
        ("backend,b", po::value<std::string>(&backend),
         "the simulator backend module to use, either SoftwareCPU or RTLCPU, default SoftwareCPU")
        ("restore,r", po::value<std::string>(&restore),
         "restore file to load from")
        ("save,s", po::value<std::string>(&save),
         "save file to write from")
        ("detached,d",
         "run the simulation in free-running mode")
        ("trace,t",
         "trace all instructions as they are executed")
        ("bios", po::value<std::string>(&bios_image)->required(),
         "the bios image to use")
        ("diskimage", po::value<std::string>(&disk_image)->required(),
         "the boot disk image");
    // clang-format on

    po::positional_options_description positional;
    positional.add("bios", 1);
    positional.add("diskimage", 1);

    po::variables_map variables_map;
    try {
        po::store(po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(positional)
                      .run(),
                  variables_map);
        if (variables_map.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        detached = variables_map.count("detached");
        trace = variables_map.count("trace");

        po::notify(variables_map);
    } catch (boost::program_options::required_option &e) {
        std::cerr << "Error: missing arguments " << e.what() << std::endl;
        return 1;
    } catch (boost::program_options::error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }

    if (backend == "SoftwareCPU") {
        run_sim<Simulator<SoftwareCPU>>(bios_image, disk_image, detached, save,
                                        restore);
    } else if (backend == "RTLCPU") {
        run_sim<Simulator<RTLCPU<verilator_debug_enabled>>>(
            bios_image, disk_image, detached, save, restore);
    } else {
        std::cerr << "Error: invalid simulation backend \"" << backend << "\""
                  << std::endl;
        return 3;
    }

    return 0;
}
