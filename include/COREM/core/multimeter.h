#ifndef MULTIMETER_H
#define MULTIMETER_H

/* BeginDocumentation
 * Name: multimeter
 *
 * Description: A multimeter records spatial or temporal evolution of connected
 * neural layers. It is also possible to define a linear-nonlinear (LN) analysis
 *[1]
 *
 * FFT code from the book "Numerical Recipes in C".
 *
 * [1] Baccus, Stephen A., and Markus Meister. "Fast and slow contrast
 *adaptation
 * in retinal circuitry." Neuron 36.5 (2002): 909-919.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 */

#include <CImg.h>

#include <stdio.h>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include "COREM/core/module.h"

#include "constants.h"

#include <gnuplot-iostream.h>

// FFT
// #define _USE_MATH_DEFINES

using namespace cimg_library;

namespace fs = boost::filesystem;

class Multimeter {
   public:
    Multimeter(unsigned int columns, unsigned int rows, retina_config_t *conf,
               std::string id, unsigned int timeToShow);

    virtual void show(int waitTime) = 0;
    virtual void showGP(
        unsigned int start_from = 0) = 0;  // Plots using gnuplot
    virtual void record(/* CImg<double> *image */) = 0;
    virtual void dump(std::string filename = "") = 0;
    virtual void setShowable(bool showable);
    virtual void setSourceModule(Module *m);
    virtual void setSourceImage(CImg<double> *img);
    virtual unsigned int showTime();

    static void saveArray(std::vector<double> array, std::string path);

   protected:
    retina_config_t *config;
    static const fs::path getExecutablePath();

    unsigned int columns;
    unsigned int rows;
    unsigned int timeToShow;
    std::string id;
    Module *source_module;
    CImg<double> *source_image;
    bool showable;
    std::vector<double> data;
};

#endif  // MULTIMETER_H
