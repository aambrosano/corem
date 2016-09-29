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

#include <stdio.h>
#include <iostream>
#include <cmath>
// #include <unistd.h>

// maximum path length
#define PATH_MAX 4096

// numerical constants
#define PI M_PI
#define TWOPI (2.0*PI)

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

namespace constants {
    extern EXPORT std::string retinaFolder;
    extern EXPORT std::string retinaScript;
    extern EXPORT const std::string resultID;
    extern EXPORT const double outputfactor;
}

#endif // CONSTANTS_H
