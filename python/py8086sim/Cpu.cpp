#include <boost/python.hpp>
#include <boost/version.hpp>

#include "RegisterFile.h"
#include "RTLCPU.h"
#include "SoftwareCPU.h"
#include "JTAGCPU.h"

using namespace boost::python;

typedef RTLCPU<verilator_debug_enabled> rtlcpu;

BOOST_PYTHON_MODULE(Cpu)
{
    class_<SoftwareCPU, boost::noncopyable>("Sim", init<const std::string &>())
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
        .def("read_io8", &SoftwareCPU::read_io8)
        .def("read_io16", &SoftwareCPU::read_io16)
        .def("read_flags", &SoftwareCPU::read_flags)
        .def("write_flags", &SoftwareCPU::write_flags)
        .def("step", &SoftwareCPU::step)
        .def("has_trapped", &SoftwareCPU::has_trapped);
    class_<rtlcpu, boost::noncopyable>("RTLCPU", init<const std::string &>())
        .def("write_reg", &rtlcpu::write_reg)
        .def("read_reg", &rtlcpu::read_reg)
        .def("write_mem8", &rtlcpu::write_mem8)
        .def("write_mem16", &rtlcpu::write_mem16)
        .def("write_mem32", &rtlcpu::write_mem32)
        .def("read_mem8", &rtlcpu::read_mem8)
        .def("read_mem16", &rtlcpu::read_mem16)
        .def("read_mem32", &rtlcpu::read_mem32)
        .def("write_io8", &rtlcpu::write_io8)
        .def("write_io16", &rtlcpu::write_io16)
        .def("read_io8", &rtlcpu::read_io8)
        .def("read_io16", &rtlcpu::read_io16)
        .def("read_flags", &rtlcpu::read_flags)
        .def("write_flags", &rtlcpu::write_flags)
        .def("step", &rtlcpu::step)
        .def("has_trapped", &rtlcpu::has_trapped);
    class_<JTAGCPU, boost::noncopyable>("JTAGCPU", init<const std::string &>())
        .def("write_reg", &rtlcpu::write_reg)
        .def("read_reg", &rtlcpu::read_reg)
        .def("write_mem8", &rtlcpu::write_mem8)
        .def("write_mem16", &rtlcpu::write_mem16)
        .def("write_mem32", &rtlcpu::write_mem32)
        .def("read_mem8", &rtlcpu::read_mem8)
        .def("read_mem16", &rtlcpu::read_mem16)
        .def("read_mem32", &rtlcpu::read_mem32)
        .def("write_io8", &rtlcpu::write_io8)
        .def("write_io16", &rtlcpu::write_io16)
        .def("read_io8", &rtlcpu::read_io8)
        .def("read_io16", &rtlcpu::read_io16)
        .def("read_flags", &rtlcpu::read_flags)
        .def("write_flags", &rtlcpu::write_flags)
        .def("step", &rtlcpu::step)
        .def("has_trapped", &rtlcpu::has_trapped);
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
