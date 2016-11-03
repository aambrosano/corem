#ifndef WHITENOISE_H
#define WHITENOISE_H

/* BeginDocumentation
 * Name: whiteNoise
 *
 * Description: White Noise generator.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: GratingGenerator, fixationalMovGrating, impulse
 */

#include <CImg.h>

#include <boost/random.hpp>

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

using namespace cimg_library;
using namespace std;

class whiteNoise {
private:

    // Normal distributions
    boost::random::normal_distribution<double> distribution1;
    boost::random::linear_congruential<unsigned long, 16807, 0, 2147483647> generator1;
    boost::random::normal_distribution<double> distribution2;
    boost::random::linear_congruential<unsigned long, 16807, 0, 2147483647> generator2;

    // time to switch
    double switchTime;

    // new value
    double GaussianPeriod;

    // Output image
    CImg <double> *output;

public:
    // Constructor
    whiteNoise();
    whiteNoise(double mean, double contrast1, double contrast2, double period, double switchT,int X, int Y);

    // update
    CImg<double>* update(double t);

    // initialize distributions
    void initializeDist(unsigned seed);

    // get time to switch
    double getSwitchTime() { return switchTime; }
};

#endif // WHITENOISE_H
