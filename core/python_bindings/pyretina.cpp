#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>
#include "RetinaLoader.h"
#include "RetinaWrapper.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(pyretina)
{
    py::class_<RetinaLoader>("RetinaLoader", py::init<Retina&>())
        .def("TempStep", &RetinaLoader::TempStep)
        .def("SimTime", &RetinaLoader::SimTime)
        .def("NumTrials", &RetinaLoader::NumTrials)
        .def("PixelsPerDegree", &RetinaLoader::PixelsPerDegree)
        .def("NRepetitions", &RetinaLoader::NRepetitions)
        .def("DisplayDelay", &RetinaLoader::DisplayDelay)
        .def("DisplayZoom", &RetinaLoader::DisplayZoom)
        .def("DisplayWindows", &RetinaLoader::DisplayWindows)
        .def("Input", &RetinaLoader::Input)
        .def("Create", &RetinaLoader::Create)
        .def("Connect", &RetinaLoader::Connect)
        .def("Show", &RetinaLoader::Show)
        .def("Multimeter", &RetinaLoader::Multimeter);
    py::class_<RetinaWrapper>("Retina", py::init<std::string>())
        .def("update", &RetinaWrapper::update)
        .def("getValue", &RetinaWrapper::getValue);
}
