#ifndef COREM_STATICNONLINEARITY_H
#define COREM_STATICNONLINEARITY_H

/* BeginDocumentation
 * Name: StaticNonLinearity
 *
 * Description: Static nonlinearity
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: module
 */

#include <CImg.h>
#include <limits>

#include "../constants.h"
#include "../module.h"

using namespace cimg_library;

class EXPORT StaticNonLinearity : public Module {
   public:
    StaticNonLinearity(std::string id, retina_config_t *conf, unsigned int type,
                       double slope, double offset, double exponent,
                       double threshold);
    StaticNonLinearity(const StaticNonLinearity &copy);
    ~StaticNonLinearity();

    virtual void update();
    virtual const CImg<double> *getOutput() const;

    template <typename T>
    int sgn(T val);

   protected:
    // type of nonlinearity
    int type_;

    // nonlinearity parameters
    double slope_;
    double offset_;
    double exponent_;
    double threshold_;

    // buffers
    CImg<double> *inputImage_;
    CImg<double> *outputImage_;
    CImg<double> *markers_;
};

#endif  // COREM_STATICNONLINEARITY_H
