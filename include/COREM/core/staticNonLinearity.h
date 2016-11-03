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

#include "module.h"
#include "constants.h"

using namespace cimg_library;

class EXPORT StaticNonLinearity : public Module {
public:
    StaticNonLinearity(std::string id, unsigned int columns, unsigned int rows,
                       double temporalStep, std::map<std::string, double> parameters,
                       int type);
    StaticNonLinearity(const StaticNonLinearity &copy);
    ~StaticNonLinearity();

    virtual void update();
    virtual CImg<double>* getOutput();

    template <typename T> int sgn(T val);

protected:
    // type of nonlinearity
    int type_;

    // nonlinearity parameters
    std::vector<double> slope_;
    std::vector<double> offset_;
    std::vector<double> exponent_;
    std::vector<double> threshold_;

    // for the piecewise function
    std::vector<double> start_;
    std::vector<double> end_;

    bool isThreshold_;

    // buffers
    CImg<double> *inputImage_;
    CImg<double> *outputImage_;
    CImg<double> *markers_;
};

#endif // COREM_STATICNONLINEARITY_H
