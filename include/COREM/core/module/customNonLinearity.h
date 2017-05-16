#ifndef CUSTOMNONLINEARITY_H
#define CUSTOMNONLINEARITY_H

#include <CImg.h>

#include "../constants.h"
#include "../module.h"

using namespace cimg_library;

class EXPORT CustomNonLinearity : public Module {
   public:
    CustomNonLinearity(std::string id, retina_config_t *conf,
                       std::vector<double> slope, std::vector<double> offset,
                       std::vector<double> exponent, std::vector<double> start,
                       std::vector<double> end);
    CustomNonLinearity(const CustomNonLinearity &copy);
    ~CustomNonLinearity();

    virtual void update();
    virtual CImg<double> *getOutput();

   protected:
    // nonlinearity parameters
    std::vector<double> slope_;
    std::vector<double> offset_;
    std::vector<double> exponent_;

    // for the piecewise function
    std::vector<double> start_;
    std::vector<double> end_;

    // buffers
    CImg<double> *inputImage_;
    CImg<double> *outputImage_;
    CImg<double> *markers_;
};

#endif  // CUSTOMNONLINEARITY_H
