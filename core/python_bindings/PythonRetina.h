#include "../DisplayManager.h"
#include "../Retina.h"
#include "../StaticNonLinearity.h"
#include "../ShortTermPlasticity.h"
#include <boost/python.hpp>
#include "../constants.h"
#include "../InterfaceNEST.h"

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

class InterfaceNESTWrapper {
private:
    InterfaceNEST  *iface_;
    CImg<double>* generateImage(int cols, int rows, const py::list& img) {
        CImg<double>* retval = new CImg<double>(cols, rows, 1, 3);
        for (int i = 0; i < retval->width(); i++) {
            for (int j = 0; j < retval->height(); j++) {
                (*retval)(i, j, 0, 0) = py::extract<double>(img[3*(j*cols+i)]);
                (*retval)(i, j, 0, 1) = py::extract<double>(img[3*(j*cols+i)+1]);
                (*retval)(i, j, 0, 2) = py::extract<double>(img[3*(j*cols+i)+2]);
            }
        }
        // *retval = retval->resize_doubleXY();
        return retval;
    }
public:
    InterfaceNESTWrapper(std::string config_path) {
        iface_ = new InterfaceNEST();
        iface_->allocateValues(config_path.c_str(), constants::resultID, constants::outputfactor, 0);
    }
    // InterfaceNESTWrapper(const InterfaceNESTWrapper& inw): iface_(inw.iface_) {};

    void update(int cols, int rows, const py::list& img) { // (CImg<double>* img) {
        CImg<double> *actual_img = generateImage(cols, rows, img);
        CImg<double> *input = iface_->retina.feedInput(actual_img);
        iface_->retina.update();
        iface_->displayMg.updateDisplay(input,
            iface_->retina,
            iface_->SimTime,
            iface_->totalSimTime,
            iface_->CurrentTrial,
            iface_->totalNumberTrials);
        iface_->SimTime += iface_->step;
        // delete input;
        // delete actual_img;
    };

    double getValue(int row, int col) {
        return iface_->getValue(row*iface_->sizeY+col);
    };
};
