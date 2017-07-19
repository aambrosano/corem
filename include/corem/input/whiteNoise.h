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

#include "../constants.h"
#include "../input.h"

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

#include <map>
#include <string>

using namespace cimg_library;
using namespace std;

class WhiteNoise : public Input {
   private:
    // Normal distributions
    boost::random::normal_distribution<double> distribution1;
    boost::random::linear_congruential<unsigned long, 16807, 0, 2147483647>
        generator1;
    boost::random::normal_distribution<double> distribution2;
    boost::random::linear_congruential<unsigned long, 16807, 0, 2147483647>
        generator2;

    // time to switch
    double switchTime;

    // new value
    double GaussianPeriod;

    // Output image
    CImg<double> *output;

   public:
    WhiteNoise(int columns, int rows,
               std::map<std::string, param_val_t> params);

    virtual const CImg<double> *getData(int t);

    virtual void getSize(int &columns, int &rows) const;

    // initialize distributions
    void initializeDist(unsigned seed);

    // get time to switch
    unsigned int getSwitchTime() { return switchTime; }

   private:
    static const std::map<std::string, param_val_t> defaultParams;
    static std::map<std::string, param_val_t> createDefaults();
};

#endif  // WHITENOISE_H
