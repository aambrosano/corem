#ifndef RETINA_H
#define RETINA_H

/* BeginDocumentation
 * Name: Retina
 *
 * Description: the class Retina implements a vector of modules, manages their
 *connections
 * and feed them with new input data every simulation step.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: module, GratingGenerator, fixationalMovGrating, impulse, whiteNoise
 */

#include <algorithm>
#include <fstream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/python.hpp>

#include "COREM/core/constants.h"
#include "COREM/core/displayManager.h"

#include "COREM/core/module.h"
#include "COREM/core/module/customNonLinearity.h"
#include "COREM/core/module/gaussFilter.h"
#include "COREM/core/module/linearFilter.h"
#include "COREM/core/module/parrot.h"
#include "COREM/core/module/shortTermPlasticity.h"
#include "COREM/core/module/singleCompartment.h"
#include "COREM/core/module/spaceVariantGaussFilter.h"
#include "COREM/core/module/staticNonLinearity.h"

#include "COREM/core/input.h"
#include "COREM/core/input/fixationalMovGrating.h"
#include "COREM/core/input/gratingGenerator.h"
#include "COREM/core/input/image.h"
#include "COREM/core/input/impulse.h"
#include "COREM/core/input/sequence.h"
#include "COREM/core/input/whiteNoise.h"

#include "COREM/core/multimeter.h"
#include "COREM/core/multimeter/LNMultimeter.h"
#include "COREM/core/multimeter/spatialMultimeter.h"
#include "COREM/core/multimeter/temporalMultimeter.h"

using namespace cimg_library;
namespace py = boost::python;
namespace fs = boost::filesystem;

class EXPORT Retina {
   public:
    DisplayManager displayMg;

    Retina();
    //    Retina(const Retina &copy);

    void initialize();

    void set_step(unsigned int temporal_step);

    // feedInput accepting a custom image
    CImg<double> *step(CImg<double> *img);
    // New input and update of equations
    CImg<double> *step();
    // void update();

    void run();

    // New module
    void addModule(Module *m);
    // Get module
    Module *getModule(int ID);
    // get number of modules
    int getNumberModules();
    // Connect modules
    void connect(std::vector<std::string> from, std::string to,
                 std::vector<Operation> operations, SynapseType type_synapse);

    retina_config_t *getConfig();
    // Load the python configuration describing the circuit
    void loadCircuit(std::string retinaPath);
    //
    double getValue(int cell, std::string layer = "Output");

    // Python interface
    void Py_TempStep(int);
    void Py_SimTime(int);
    void Py_NumTrials(int);
    void Py_PixelsPerDegree(int);
    void Py_NRepetitions(double);
    void Py_DisplayDelay(int);
    void Py_DisplayZoom(int);
    void Py_DisplayWindows(int);

    void Py_Input(std::string, py::dict);
    void Py_Create(std::string, std::string, py::dict);
    void Py_Connect(py::object, std::string, std::string);
    void Py_Show(std::string, bool, py::dict);
    void Py_Multimeter(std::string, std::string, std::string, py::dict, bool);

    void Py_Step(const py::object &);
    double Py_GetValue(int row, int col, string layer = "Output");

   protected:
    // Retina output and accumulator of intermediate images
    CImg<double> *output;
    CImg<double> accumulator;
    // retina input channels (for color conversion)
    CImg<double> RGBred, RGBgreen, RGBblue, ch1, ch2, ch3, rods, X_mat, Y_mat,
        Z_mat;
    // vector of retina modules
   public:
    vector<Module *> modules;
    vector<Multimeter *> multimeters;
    // Type of input
   protected:
    Input *inputModule;
    CImg<double> input;  // Used only for multimeters!

   private:
    retina_config_t *config;
    simulation_state_t *simState;

    CImg<double> *convertImage(const boost::python::api::object &img);
    CImg<double> *convertndarray(const boost::python::api::object &img);
};

EXPORT void define_retina_python();

#endif  // RETINA_H
