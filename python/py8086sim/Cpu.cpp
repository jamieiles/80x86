#include <boost/python.hpp>
#include <boost/version.hpp>

#include "RegisterFile.h"
#include "RTLCPU.h"
#include "SoftwareCPU.h"
#include "JTAGCPU.h"

using namespace boost::python;

typedef RTLCPU<verilator_debug_enabled> rtlcpu;

template <typename T>
void write_vector8(T *c,
                   uint16_t segment,
                   uint16_t addr,
                   boost::python::list vals)
{
    std::vector<uint8_t> vec;

    for (auto i = 0; i < boost::python::len(vals); ++i)
        vec.push_back(boost::python::extract<uint8_t>(vals[i]));

    c->write_vector8(segment, addr, vec);
}

template <typename T>
void write_vector16(T *c,
                    uint16_t segment,
                    uint16_t addr,
                    boost::python::list vals)
{
    std::vector<uint16_t> vec;

    for (auto i = 0; i < boost::python::len(vals); ++i)
        vec.push_back(boost::python::extract<uint16_t>(vals[i]));

    c->write_vector16(segment, addr, vec);
}

BOOST_PYTHON_MODULE(Cpu)
{
    class_<SoftwareCPU, boost::noncopyable>("Sim", init<const std::string &>())
        .def("reset", &SoftwareCPU::reset)
        .def("write_reg", &SoftwareCPU::write_reg)
        .def("read_reg", &SoftwareCPU::read_reg)
        .def("write_mem8", &SoftwareCPU::write_mem8)
        .def("write_mem16", &SoftwareCPU::write_mem16)
        .def("write_mem32", &SoftwareCPU::write_mem32)
        .def("read_mem8", &SoftwareCPU::read_mem8)
        .def("read_mem16", &SoftwareCPU::read_mem16)
        .def("read_mem32", &SoftwareCPU::read_mem32)
        .def("write_io8", &SoftwareCPU::write_io8)
        .def("write_io16", &SoftwareCPU::write_io16)
        .def("write_vector8", &write_vector8<SoftwareCPU>)
        .def("write_vector16", &write_vector16<SoftwareCPU>)
        .def("read_io8", &SoftwareCPU::read_io8)
        .def("read_io16", &SoftwareCPU::read_io16)
        .def("read_flags", &SoftwareCPU::read_flags)
        .def("write_flags", &SoftwareCPU::write_flags)
        .def("step", &SoftwareCPU::step)
        .def("has_trapped", &SoftwareCPU::has_trapped);
    class_<rtlcpu, boost::noncopyable>("RTLCPU", init<const std::string &>())
        .def("reset", &rtlcpu::reset)
        .def("write_reg", &rtlcpu::write_reg)
        .def("read_reg", &rtlcpu::read_reg)
        .def("write_mem8", &rtlcpu::write_mem8)
        .def("write_mem16", &rtlcpu::write_mem16)
        .def("write_mem32", &rtlcpu::write_mem32)
        .def("read_mem8", &rtlcpu::read_mem8)
        .def("read_mem16", &rtlcpu::read_mem16)
        .def("read_mem32", &rtlcpu::read_mem32)
        .def("write_vector8", &write_vector8<rtlcpu>)
        .def("write_vector16", &write_vector16<rtlcpu>)
        .def("write_io8", &rtlcpu::write_io8)
        .def("write_io16", &rtlcpu::write_io16)
        .def("read_io8", &rtlcpu::read_io8)
        .def("read_io16", &rtlcpu::read_io16)
        .def("read_flags", &rtlcpu::read_flags)
        .def("write_flags", &rtlcpu::write_flags)
        .def("step", &rtlcpu::step)
        .def("idle", &rtlcpu::idle)
        .def("time_step", &rtlcpu::time_step)
        .def("has_trapped", &rtlcpu::has_trapped);
    class_<JTAGCPU, boost::noncopyable>("JTAGCPU", init<const std::string &>())
        .def("reset", &JTAGCPU::reset)
        .def("write_reg", &JTAGCPU::write_reg)
        .def("read_reg", &JTAGCPU::read_reg)
        .def("write_mem8", &JTAGCPU::write_mem8)
        .def("write_mem16", &JTAGCPU::write_mem16)
        .def("write_mem32", &JTAGCPU::write_mem32)
        .def("read_mem8", &JTAGCPU::read_mem8)
        .def("read_mem16", &JTAGCPU::read_mem16)
        .def("read_mem32", &JTAGCPU::read_mem32)
        .def("write_vector8", &write_vector8<JTAGCPU>)
        .def("write_vector16", &write_vector16<JTAGCPU>)
        .def("write_io8", &JTAGCPU::write_io8)
        .def("write_io16", &JTAGCPU::write_io16)
        .def("read_io8", &JTAGCPU::read_io8)
        .def("read_io16", &JTAGCPU::read_io16)
        .def("read_flags", &JTAGCPU::read_flags)
        .def("write_flags", &JTAGCPU::write_flags)
        .def("step", &JTAGCPU::step)
        .def("has_trapped", &JTAGCPU::has_trapped);
    enum_<GPR>("GPR")
        .value("AX", AX)
        .value("CX", CX)
        .value("DX", DX)
        .value("BX", BX)
        .value("SP", SP)
        .value("BP", BP)
        .value("SI", SI)
        .value("DI", DI)
        .value("IP", IP)
        .value("ES", ES)
        .value("CS", CS)
        .value("SS", SS)
        .value("DS", DS)
        .value("AL", AL)
        .value("CL", CL)
        .value("DL", DL)
        .value("BL", BL)
        .value("AH", AH)
        .value("CH", CH)
        .value("DH", DH)
        .value("BH", BH);
    enum_<Flag>("Flag")
        .value("CF", CF)
        .value("PF", PF)
        .value("AF", AF)
        .value("ZF", ZF)
        .value("SF", SF)
        .value("TF", TF)
        .value("IF", IF)
        .value("DF", DF)
        .value("OF", OF);
}
