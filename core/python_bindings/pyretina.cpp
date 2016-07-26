#include <boost/python.hpp>
#include "PythonRetina.h"
#include "InterfaceNESTWrapper.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(pyretina)
{
    py::class_<PythonRetina>("Retina", py::init<Retina&, DisplayManager&>())
        .def("TempStep", &PythonRetina::TempStep)
        .def("SimTime", &PythonRetina::SimTime)
        .def("NumTrials", &PythonRetina::NumTrials)
        .def("PixelsPerDegree", &PythonRetina::PixelsPerDegree)
        .def("NRepetitions", &PythonRetina::NRepetitions)
        .def("DisplayDelay", &PythonRetina::DisplayDelay)
        .def("DisplayZoom", &PythonRetina::DisplayZoom)
        .def("DisplayWindows", &PythonRetina::DisplayWindows)
        .def("Input", &PythonRetina::Input)
        .def("Create", &PythonRetina::Create)
        .def("Connect", &PythonRetina::Connect)
        .def("Show", &PythonRetina::Show)
        .def("Multimeter", &PythonRetina::Multimeter);
    py::class_<InterfaceNESTWrapper>("InterfaceNEST", py::init<std::string>())
        .def("update", &InterfaceNESTWrapper::update)
        .def("getValue", &InterfaceNESTWrapper::getValue);
}
