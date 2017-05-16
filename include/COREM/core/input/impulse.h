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
  // Constructor, copy, destructor.
  // impulse();
  // impulse(double startParam, double stopParam, double amplitudeParam, double
  // offsetParam,int X, int Y);
  Impulse(unsigned int columns, unsigned int rows,
          std::map<std::string, param_val_t> params);

  // update
  // CImg<double>* update(double t);
  virtual CImg<double> *getData(unsigned int t);
  virtual void getSize(unsigned int &columns, unsigned int &rows);

private:
  static const std::map<std::string, param_val_t> defaultParams;
  static std::map<std::string, param_val_t> createDefaults();
};

#endif // IMPULSE_H
