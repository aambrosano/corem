#ifndef RETINA_H
#define RETINA_H

/* BeginDocumentation
 * Name: Retina
 *
 * Description: the class Retina implements a vector of modules, manages their
 * connections
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

#include <numpy/arrayobject.h>

#include <corem/constants.h>
#include <corem/displayManager.h>

#include <corem/module.h>
#include <corem/module/customNonLinearity.h>
#include <corem/module/gaussFilter.h>
#include <corem/module/highpass.h>
#include <corem/module/linearFilter.h>
#include <corem/module/parrot.h>
#include <corem/module/rectification.h>
#include <corem/module/shortTermPlasticity.h>
#include <corem/module/singleCompartment.h>
#include <corem/module/spaceVariantGaussFilter.h>
#include <corem/module/staticNonLinearity.h>

#include <corem/spikes.h>
#include <corem/spikes/lif.h>

#include <corem/input.h>
#include <corem/input/fixationalMovGrating.h>
#include <corem/input/gratingGenerator.h>
#include <corem/input/image.h>
#include <corem/input/impulse.h>
#include <corem/input/localizedImpulse.h>
#include <corem/input/sequence.h>
#include <corem/input/whiteNoise.h>

#include <corem/multimeter.h>
#include <corem/multimeter/LNMultimeter.h>
#include <corem/multimeter/spatialMultimeter.h>
#include <corem/multimeter/temporalMultimeter.h>

using namespace cimg_library;
namespace py = boost::python;
namespace fs = boost::filesystem;

class EXPORT Retina {
   public:
    DisplayManager *displayMg;

    Retina();
    ~Retina();

    void initialize();

    void set_step(unsigned int temporal_step);

    // feedInput accepting a custom image
    const CImg<double> *step(const CImg<double> *img);
    // New input and update of equations
    const CImg<double> *step();

    void run();

    // New module
    void addModule(Module *m);
    // Get module
    Module *getModule(int ID);
    Module *getModule(const std::string s);
    // get number of modules
    int getNumberModules();
    // Connect modules
    void connect(std::vector<std::string> from, std::string to,
                 std::vector<Operation> operations, SynapseType type_synapse);

    retina_config_t *getConfig();
    // Load the python configuration describing the circuit
    void loadCircuit(std::string retinaPath);
    double getValue(int cell, std::string layer = "Output");
    const CImg<double> *getOutput(std::string layer = "Output");

    void stats();
    const CImg<double> *getData(std::string id);

    // Python interface
    void Py_TempStep(int);
    void Py_SimTime(int);
    void Py_NumTrials(int);
    void Py_PixelsPerDegree(double);
    void Py_NRepetitions(double);
    void Py_DisplayDelay(int);
    void Py_DisplayZoom(int);
    void Py_DisplayWindows(int);

    void Py_Input(std::string, py::dict);
    void Py_Create(std::string, std::string, py::dict);
    void Py_Connect(py::object, std::string, std::string);
    void Py_Show(std::string, bool, py::dict);
    void Py_Multimeter(std::string, std::string, std::string, py::dict, bool);

    void Py_Tweak(const std::string module, const py::list E);

    void Py_Step(const py::object &);
    void Py_Step();
    void Py_Run();
    double Py_GetValue(int row, int col, std::string layer = "Output");
    py::object Py_GetOutput(std::string module);
    py::object Py_GetSpikes(std::string module);

    void Py_Stats();

   protected:
    // Retina output and accumulator of intermediate images
    CImg<double> *output;
    CImg<double> *accumulator;
    // retina input channels (for color conversion)
    CImg<double> *RGBred;
    CImg<double> *RGBgreen;
    CImg<double> *RGBblue;
    CImg<double> *L_cones;
    CImg<double> *M_cones;
    CImg<double> *S_cones;
    CImg<double> *rods;

   public:
    vector<SpikeSource *> spike_sources;
    vector<Module *> modules;
    vector<Multimeter *> multimeters;
    double t_RGB_to_LMS, t_module_ports, t_modules, t_multimeters,
        t_update_display;

   protected:
    Input *inputModule;
    const CImg<double> *input;  // Used only for multimeters!

   private:
    retina_config_t *config;
    simulation_state_t *simState;

    CImg<double> *convertImage(const boost::python::api::object &img);
    CImg<double> *convertndarray(const boost::python::api::object &img);
    py::object convertToPythonImage(const CImg<double> *img);
};

EXPORT void define_retina_python();

#endif  // RETINA_H
