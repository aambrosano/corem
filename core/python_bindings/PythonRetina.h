#include "../DisplayManager.h"
#include "../Retina.h"
#include "../StaticNonLinearity.h"
#include "../ShortTermPlasticity.h"
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include "../constants.h"
#include "../InterfaceNEST.h"

namespace py = boost::python;
namespace fs = boost::filesystem;

class PythonRetina {
private:
  Retina *retina_;
  DisplayManager *display_manager_;

public:
    PythonRetina(Retina& cpp_retina, DisplayManager& cpp_dm)
        : retina_(&cpp_retina), display_manager_(&cpp_dm) {}

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

class InterfaceNESTWrapper {

public:
    InterfaceNESTWrapper(std::string config_path);

    void update(const py::object& img);
    double getValue(int row, int col, string layer);

private:
    InterfaceNEST  *iface_;
    CImg<double> inputImg;
    int rows, cols;

    // main conversion function, dispatches to the ones below
    // at the end, the converted image can be found in inputImg
    void convertImage(const py::object& img);
    void convertndarray(const py::object& img);
};
