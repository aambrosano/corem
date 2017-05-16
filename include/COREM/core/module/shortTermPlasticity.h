#ifndef COREM_SHORTTERMPLASTICITY_H
#define COREM_SHORTTERMPLASTICITY_H

/* BeginDocumentation
 * Name: ShortTermPlasticity
 *
 * Description: dynamic nonlinearity that includes a STP term
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: module
 */

#include "../module.h"

#include <limits>

#define DBL_EPSILON_STP 1.0e-2

class EXPORT ShortTermPlasticity : public Module {
   public:
    // TODO: move Am, Vm, Em, th and isTh to parameters
    ShortTermPlasticity(std::string id, retina_config_t *conf, double slope,
                        double offset, double exponent, double kf, double kd,
                        double tau, double vinf, double threshold);
    ShortTermPlasticity(const ShortTermPlasticity &copy);
    ~ShortTermPlasticity();

    virtual void update();
    virtual CImg<double> *getOutput();

   protected:
    // nonlinearity parameters
    double slope_;
    double offset_;
    double exponent_;
    double threshold_;
    // STP parameters
    double kf_;
    double kd_;
    double tau_;
    double VInf_;
    // Buffers
    CImg<double> **inputImage_;
    CImg<double> *outputImage_;
};

#endif  // COREM_SHORTTERMPLASTICITY_H
