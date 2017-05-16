#ifndef COREM_GAUSSFILTER_H
#define COREM_GAUSSFILTER_H

/* BeginDocumentation
 * Name: GaussFilter
 *
 * Description: Gaussian convolution to reproduce spatial integration in
 *chemical
 * and gap-junction synapses. It implements a recursive
 *infinite-impulse-response
 * (IIR) filter based on the Deriche's algorithm. Based on [1,2]. OpenMP is used
 * for multithreading
 *
 * [1] Triggs, Bill, and Michaël Sdika. "Boundary conditions for Young-van Vliet
 * recursive filtering." Signal Processing, IEEE Transactions on 54.6 (2006):
 * 2365-2367.
 *
 * [2] Tan, Sovira, Jason L. Dale, and Alan Johnston. "Performance of three
 * recursive algorithms for fast space-variant Gaussian filtering." Real-Time
 * Imaging 9.3 (2003): 215-228.
 *
 * Source code adapted from RawTherapee library. <http://rawtherapee.com/>
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso:
 */

#include <CImg.h>
#include <omp.h>
#include "../constants.h"
#include "../module.h"

class EXPORT GaussFilter : public Module {
   public:
    GaussFilter(std::string id, retina_config_t *conf, double sigma);
    GaussFilter(const GaussFilter &copy);
    ~GaussFilter();

    virtual void update();
    virtual CImg<double> *getOutput();

    // Fast filtering with constant sigma
    void gaussHorizontal(CImg<double> &src);
    void gaussVertical(CImg<double> &src);

   protected:
    // Filter parameters
    double sigma_;
    double *buffer_;

    // Deriche's coefficients
    double q_;
    double b0_;
    double b1_;
    double b2_;
    double b3_;
    double B_;

    // Matrices
    double M_[3][3];

    CImg<double> *inputImage_;
    CImg<double> *outputImage_;
};

#endif  // COREM_GAUSSFILTER_H
