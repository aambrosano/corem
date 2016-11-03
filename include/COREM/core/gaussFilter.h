#ifndef COREM_GAUSSFILTER_H
#define COREM_GAUSSFILTER_H

/* BeginDocumentation
 * Name: GaussFilter
 *
 * Description: Gaussian convolution to reproduce spatial integration in chemical
 * and gap-junction synapses. It implements a recursive infinite-impulse-response
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

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

#include <CImg.h>
#include <omp.h>
#include "module.h"
#include "constants.h"

class EXPORT GaussFilter: public Module {
public:
    GaussFilter(std::string id, unsigned int columns, unsigned int rows, double temporalStep,
                std::map<std::string, double> parameters, double pixelsPerDegree);
    GaussFilter(const GaussFilter &copy);
    ~GaussFilter();

    virtual void update();
    virtual CImg<double> *getOutput();

    // Fast filtering with constant sigma
    void gaussHorizontal(CImg<double> &src);
    void gaussVertical(CImg <double> &src);
    void gaussFiltering(CImg<double> &src);

    // Fast filtering with space-variant sigma
    void spaceVariantGaussHorizontal(CImg<double> &src);
    void spaceVariantGaussVertical(CImg<double> &src);
    void spaceVariantGaussFiltering(CImg<double> &src);

    double density(double r);

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
    CImg<double> q_m_;
    CImg<double> b0_m_;
    CImg<double> b1_m_;
    CImg<double> b2_m_;
    CImg<double> b3_m_;
    CImg<double> B_m_;
    CImg<double> M_m_;

    // spaceVariantSigma
    bool spaceVariantSigma_;
    double R0_;
    double K_;

    CImg<double> *inputImage_;
    CImg<double> *outputImage_;
};

#endif // COREM_GAUSSFILTER_H
