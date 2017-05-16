#ifndef CONSTANTS_H
#define CONSTANTS_H

/* BeginDocumentation
 * Name: constants
 *
 * Description: declaration of constants used in the project and default path
 * to the retina script
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// #include <unistd.h>

// maximum path length
#define PATH_MAX 4096

// numerical constants
#define PI M_PI
#define TWOPI (2.0 * PI)

#undef DBL_EPSILON
const double DBL_EPSILON = 1.0e-9;
const double DBL_INF = 1.0e9;

#ifdef _WIN32
#ifdef EXPORTING_RETINA
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#elif __linux__
#define EXPORT
#endif

const bool verbose = true;

namespace constants {
extern EXPORT std::string retinaFolder;
extern EXPORT std::string retinaScript;
extern EXPORT const std::string resultID;
extern EXPORT const double outputfactor;
}

typedef union param_val_t {
    double d;
    std::string *s;
    std::vector<double> *vd;
    uint64_t u;
    int64_t i;
    bool b;
} param_val_t;

param_val_t getParam(std::string name,
                     std::map<std::string, param_val_t> params,
                     std::map<std::string, param_val_t> defaults);

typedef struct retina_config_t {
   public:
    retina_config_t()
        : simulationTime(0),
          simulationStep(0),
          numTrials(0),
          pixelsPerDegree(0),
          displayDelay(0),
          displayZoom(0),
          columns(0),
          rows(0) {}

    retina_config_t(retina_config_t &copy)
        : simulationTime(copy.simulationTime),
          simulationStep(copy.simulationStep),
          numTrials(copy.numTrials),
          pixelsPerDegree(copy.pixelsPerDegree),
          displayDelay(copy.displayDelay),
          displayZoom(copy.displayZoom),
          columns(copy.columns),
          rows(copy.rows) {}

    // These have been set to be signed integers because it often happens to
    // have them sign reversed during the simulation
    int simulationTime;  // milliseconds
    int simulationStep;  // milliseconds
    int numTrials;
    int pixelsPerDegree;
    int displayDelay;  // milliseconds
    int displayZoom;
    unsigned int columns;
    unsigned int rows;
} retina_config_t;

typedef struct simulation_state_t {
    unsigned int currentTime;
    unsigned int currentTrial;
} simulation_state_t;

void error(std::string msg);
void debug(std::string msg);

#endif  // CONSTANTS_H
