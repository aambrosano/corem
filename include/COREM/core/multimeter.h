#ifndef MULTIMETER_H
#define MULTIMETER_H

/* BeginDocumentation
 * Name: multimeter
 *
 * Description: A multimeter records spatial or temporal evolution of connected
 * neural layers. It is also possible to define a linear-nonlinear (LN) analysis [1]
 *
 * FFT code from the book "Numerical Recipes in C".
 *
 * [1] Baccus, Stephen A., and Markus Meister. "Fast and slow contrast adaptation
 * in retinal circuitry." Neuron 36.5 (2002): 909-919.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 */

#include <CImg.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include <boost/filesystem.hpp>

#include "constants.h"

// FFT
// #define _USE_MATH_DEFINES


using namespace cimg_library;
using namespace std;

namespace fs = boost::filesystem;

class multimeter{
protected:
    // Image size
    int sizeX, sizeY;
    // temporal registers
    vector <double> temporal;
    vector <double> input;
    // LN analysis
    std::ifstream fin;
    std::ofstream fout;
    std::fstream finout;
    double switchTime;

    // simulation step
    double simStep;

public:

    // Constructor
    multimeter(int x=1,int y=1);

    // Spatial multimeter
    void showSpatialProfile(CImg<double> img,bool rowCol,int number, const string &title,int col,int row,double waitTime);

    // Temporal multimeter
    void recordValue(double value);
    void showTemporalProfile(const string &title, int col, int row, double waitTime, const string &TempFile);

    // LN analysis
    void recordInput(double value);
    void showLNAnalysis(const string &title, int col, int row, double waitTime, double segment,double interval, double start,
                        double stop, double numberTrials, const string &LNFile);
    void showLNAnalysisAvg(int col, int row,double waitTime, double segment, double start, double stop, double numberTrials,
                           const string &LNFile, double ampl);
    void saveArray(double* array, unsigned int arraySize, string fileID);
    void getSwitchTime(double t){switchTime=t;}
    fs::path getExecutablePath();
    void removeResultsFile(const string &filename);

    vector<double> readSeq(const string &LNFile);
    void saveSeq(vector<double> newSeq, const string &LNFile, double maxSize);

    //Sim step
    void setSimStep(double value);

    //FFT
    void fft(double data[], int nn, int isign);
    void conj(double data[], double copy[], int NFFT);

};

enum MultimeterType { Temporal = 0, Spatial = 1, LinearNonLinear = 2 };

#endif // MULTIMETER_H
