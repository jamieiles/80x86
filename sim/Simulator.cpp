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

#include "CPU.h"
#include "SoftwareCPU.h"
#include "RTLCPU.h"
#include "UART.h"
#include "SPI.h"
#include "Timer.h"

class SDRAMConfigRegister : public IOPorts {
public:
    SDRAMConfigRegister()
        : IOPorts(0xfffc, 1)
    {}

    void write8(uint16_t __unused port_num, unsigned __unused offs,
                uint8_t __unused v) {}
    void write16(uint16_t __unused port_num, uint16_t __unused v) {}
    uint8_t read8(uint16_t __unused port_num, unsigned offs)
    {
        return offs == 0 ? 0x1 : 0;
    }
    uint16_t read16(uint16_t __unused port_num)
    {
        return 1;
    }
};

template <typename T>
class Simulator {
public:
    Simulator<T>(const std::string &bios_path,
                 const std::string &disk_image_path,
                 bool detached);
    void run();
private:
    void load_bios(const std::string &bios_path);
    void process_io();

    T cpu;
    SDRAMConfigRegister sdram_config_register;
    UART uart;
    SPI spi;
    TimerTick timer;
    bool got_exit;
    bool detached;
};

template <typename T>
Simulator<T>::Simulator(const std::string &bios_path,
                        const std::string &disk_image_path,
                        bool detached)
    : cpu("simulator"),
    spi(disk_image_path),
    timer(&this->cpu),
    got_exit(false),
    detached(detached)
{
    cpu.add_ioport(&sdram_config_register);
    cpu.add_ioport(&uart);
    cpu.add_ioport(&spi);
    cpu.add_ioport(&timer);
    cpu.reset();
    load_bios(bios_path);
}

template <typename T>
void Simulator<T>::load_bios(const std::string &bios_path)
{
    std::ifstream bios(bios_path, std::ios::binary);
    for (unsigned offs = 0; !bios.eof(); ++offs) {
        char v;
        bios.read(&v, 1);
        cpu.write_mem8(0xfe00, offs, v);
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
}

template <typename T>
void Simulator<T>::run()
{
    unsigned cycle_count = 0;

    auto start_time = std::chrono::system_clock::now();

    if (detached)
        cpu.debug_detach();

    while (!got_exit) {
        auto prev_count = cpu.cycle_count();

        if (++cycle_count % 1000)
            process_io();
        if (detached)
            cpu.cycle_cpu();
        else
            cpu.step();

        auto new_count = cpu.cycle_count();

        timer.tick((new_count - prev_count) * 2);
    }

    auto end_time = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end_time - start_time;

    std::cout << "Operating frequency: " << (cpu.cycle_count() / 1000000.0) / elapsed_seconds.count() << "MHz" << std::endl;
}

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    std::string bios_image;
    std::string disk_image;
    std::string backend = "SoftwareCPU";
    bool detached;

    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "print this usage information and exit")
        ("backend,b", po::value<std::string>(&backend),
         "the simulator backend module to use, either SoftwareCPU or RTLCPU, default SoftwareCPU")
        ("detached,d",
         "run the simulation in free-running mode")
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

        detached = variables_map.count("detached");

        po::notify(variables_map);
    } catch (boost::program_options::required_option &e) {
        std::cerr << "Error: missing arguments " << e.what() << std::endl;
        return 1;
    } catch (boost::program_options::error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }

    if (backend == "SoftwareCPU") {
        Simulator<SoftwareCPU> sim(bios_image, disk_image, detached);
        sim.run();
    } else if (backend == "RTLCPU") {
        Simulator<RTLCPU<verilator_debug_enabled>> sim(bios_image, disk_image, detached);
        sim.run();
    } else {
        std::cerr << "Error: invalid simulation backend \"" << backend << "\"" << std::endl;
        return 3;
    }

    return 0;
}
