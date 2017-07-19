#ifndef IMPULSE_H
#define IMPULSE_H

/* BeginDocumentation
 * Name: impulse
 *
 * Description: impulse generator.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: GratingGenerator, fixationalMovGrating, whiteNoise
 */

#include "../constants.h"
#include "../input.h"

#include <CImg.h>

#include <map>
#include <string>

using namespace cimg_library;
using namespace std;

class Impulse : public Input {
   private:
    // parameters
    double start;
    double stop;
    double amplitude;
    double offset;

    // Output image
    CImg<double> *output;

   public:
    Impulse(int columns, int rows, std::map<std::string, param_val_t> params);

    virtual const CImg<double> *getData(int t);

    virtual void getSize(int &columns, int &rows) const;

   private:
    static const std::map<std::string, param_val_t> defaultParams;
    static std::map<std::string, param_val_t> createDefaults();
};

#endif  // IMPULSE_H
