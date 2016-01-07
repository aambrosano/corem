#include "../DisplayManager.h"
#include "../Retina.h"
#include "../StaticNonLinearity.h"
#include "../ShortTermPlasticity.h"
#include <boost/python.hpp>

namespace py = boost::python;

class PythonRetina {
private:
  Retina *retina_;
  DisplayManager *display_manager_;

public:
    PythonRetina(Retina& cpp_retina, DisplayManager& cpp_dm)
        : retina_(&cpp_retina), display_manager_(&cpp_dm) {};

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
