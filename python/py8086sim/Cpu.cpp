#include <boost/python.hpp>
#include <boost/version.hpp>

#include "CPU.h"
#include "RegisterFile.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(Cpu)
{
    class_<CPU, boost::noncopyable>("Sim", init<>())
        .def("write_reg", &CPU::write_reg)
        .def("read_reg", &CPU::read_reg)
        .def("write_mem8", &CPU::write_mem<uint8_t>)
        .def("write_mem16", &CPU::write_mem<uint16_t>)
        .def("write_mem32", &CPU::write_mem<uint32_t>)
        .def("read_mem8", &CPU::read_mem<uint8_t>)
        .def("read_mem16", &CPU::read_mem<uint16_t>)
        .def("read_mem32", &CPU::read_mem<uint32_t>)
        .def("write_io8", &CPU::write_io<uint8_t>)
        .def("write_io16", &CPU::write_io<uint16_t>)
        .def("read_io8", &CPU::read_io<uint8_t>)
        .def("read_io16", &CPU::read_io<uint16_t>)
        .def("read_flags", &CPU::read_flags)
        .def("write_flags", &CPU::write_flags)
        .def("cycle", &CPU::cycle)
        .def("has_trapped", &CPU::has_trapped);
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
