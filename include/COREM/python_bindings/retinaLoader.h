#ifndef RETINA_LOADER_H
#define RETINA_LOADER_H

#include <COREM/core/retina.h>
#include <COREM/core/staticNonLinearity.h>
#include <COREM/core/shortTermPlasticity.h>
#include <COREM/core/constants.h>

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>

namespace py = boost::python;
namespace fs = boost::filesystem;

class Retina;
class RetinaLoader {
private:
  Retina *retina_;

public:
    RetinaLoader(Retina& cpp_retina) : retina_(&cpp_retina) {}

    void TempStep(double);
    void SimTime(int);
    void NumTrials(double);
    void PixelsPerDegree(double);
    void NRepetitions(double);
    void DisplayDelay(int);
    void DisplayZoom(double);
    void DisplayWindows(int);

    void Input(std::string, py::dict);
    void Create(std::string, std::string, py::dict);
    void Connect(py::object, std::string, std::string);
    void Show(std::string, bool, py::dict);
    void Multimeter(std::string, std::string, std::string, py::dict, bool);
};

#endif
