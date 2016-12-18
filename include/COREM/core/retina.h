#ifndef RETINA_H
#define RETINA_H

/* BeginDocumentation
 * Name: Retina
 *
 * Description: the class Retina implements a vector of modules, manages their connections
 * and feed them with new input data every simulation step.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: module, GratingGenerator, fixationalMovGrating, impulse, whiteNoise
 */


#include <algorithm>
#include <fstream>

#include "module.h"
#include "linearFilter.h"
#include "singleCompartment.h"
#include "gaussFilter.h"
#include "gratingGenerator.h"
#include "fixationalMovGrating.h"
#include "whiteNoise.h"
#include "staticNonLinearity.h"
#include "shortTermPlasticity.h"
#include "impulse.h"
#include "displayManager.h"
#include "constants.h"

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

using namespace cimg_library;
using namespace std;
namespace py = boost::python;
namespace fs = boost::filesystem;

class EXPORT Retina {
protected:
    // Image size
    int columns_, rows_;
    // simulation step and ppd
    // double step;
    double pixelsPerDegree;

    // Retina output and accumulator of intermediate images
    CImg <double> *output;
    CImg <double> accumulator;
    // retina input channels (for color conversion)
    CImg<double> RGBred,RGBgreen,RGBblue,ch1,ch2,ch3,rods,X_mat,Y_mat,Z_mat;
    // vector of retina modules
    vector <module*> modules;
    // Type of input
    int inputType;

    // Inputs
    vector < CImg<double> > sequence;
    GratingGenerator *g;
    fixationalMovGrating *fg;
    whiteNoise *WN;
    impulse *imp;
    // input seq
    CImg<double>** inputSeq;
    // Number of images in the input seq
    int numberImages;
    // Number of repetitions of each image
    int repetitions;

    // Simulation time
    int simCurrTime, simDuration;
    // Current and total number of trials
    double currentTrial, totalNumberTrials;

    // Display comments
    bool verbose;
    CImg<double> lastFedInput;

public:
    double simStep;
    DisplayManager displayMg;

    // Constructor, copy, destructor.
    Retina(int columns=1, int rows=1, double temporal_step=1.0);
    Retina(const Retina& copy) : Retina(copy.columns_, copy.rows_, copy.simStep) {}

    void reset(int columns=1, int rows=1, double temporal_step=1.0);

    // Allocate values and set protected parameters
    void allocateValues();
    void setColumns(int columns);
    void setRows(int rows);
    void set_step(double temporal_step);
    int getColumns();
    int getRows();
    double getSimStep();
    void setSimCurrentTrial(double r);
    void setSimTotalTrials(double r);
    void setSimDuration(int t);
    void setSimCurrTime(int t);
    double getSimCurrentTrial();
    double getSimTotalTrials();
    int getSimDuration();
    int getSimCurrTime();

    // set and get pixelsPerDegree
    void setPixelsPerDegree(double ppd);
    double getPixelsPerDegree();
    // feedInput accepting a custom image
    CImg<double> *feedInput(CImg<double>* img);
    // New input and update of equations
    CImg<double> *feedInput(int simStep);
    void update();

    // New module
    void addModule(module* m, string ID);
    // Get module
    module* getModule(int ID);
    // get number of modules
    int getNumberModules();
    // Connect modules
    bool connect(vector<string> from, string to, vector<int> operations, string type_synapse);

    // Grating generator
    bool generateGrating(int type,double simStep,double lengthB,double length,double length2,int X,int Y,double freq,double T,double Lum,double Cont,double phi,double phi_t,double theta,double red, double green, double blue,double red_phi, double green_phi,double blue_phi);
    CImg<double> *updateGrating(double t);
    // Grating for fixational Movements
    bool generateFixationalMovGrating(int X,int Y,double radius,double jitter,double period,double simStep,double luminance,double contrast,double orientation,double red_weight,double green_weigh, double blue_weight, int type1, int type2, int ts);
    CImg<double> *updateFixGrating(double t);
    // White noise
    bool generateWhiteNoise(double mean, double contrast1,double contrast2, double period, double switchT,int X, int Y);
    CImg<double> *updateNoise(double t);
    whiteNoise* getWhiteNoise();
    // Impulse
    bool generateImpulse(double start, double stop, double amplitude, double offset, int X, int Y);
    CImg<double> *updateImpulse(double t);
    // Input Sequence
    bool setInputSeq(string s);
    // get number of images
    int getNumberImages();
    // Set repetitions
    void setRepetitions(int r);
    // Load the python configuration describing the circuit
    void loadCircuit(std::string retinaPath);
    //
    double getValue(int cell, std::string layer="Output");

    // Python interface
    void Py_TempStep(double);
    void Py_SimTime(int);
    void Py_NumTrials(double);
    void Py_PixelsPerDegree(double);
    void Py_NRepetitions(double);
    void Py_DisplayDelay(int);
    void Py_DisplayZoom(double);
    void Py_DisplayWindows(int);

    void Py_Input(std::string, py::dict);
    void Py_Create(std::string, std::string, py::dict);
    void Py_Connect(py::object, std::string, std::string);
    void Py_Show(std::string, bool, py::dict);
    void Py_Multimeter(std::string, std::string, std::string, py::dict, bool);

    void Py_Update(const py::object&);
    double Py_GetValue(int row, int col, string layer="Output");

private:
    CImg<double>* convertImage(const boost::python::api::object& img);
    CImg<double>* convertndarray(const boost::python::api::object& img);
};

EXPORT void define_retina_python();

#endif // RETINA_H
