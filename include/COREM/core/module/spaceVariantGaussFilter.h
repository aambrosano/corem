#ifndef SPACEVARIANTGAUSSFILTER_H
#define SPACEVARIANTGAUSSFILTER_H

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

#include <CImg.h>
#include <omp.h>
#include "../constants.h"
#include "../module.h"

class EXPORT SpaceVariantGaussFilter : public Module {
   public:
    SpaceVariantGaussFilter(std::string id, retina_config_t *conf, double sigma,
                            double K, double R0);
    SpaceVariantGaussFilter(const SpaceVariantGaussFilter &copy);
    ~SpaceVariantGaussFilter();

    virtual void update();
    virtual CImg<double> *getOutput();

    // Fast filtering with space-variant sigma
    void gaussHorizontal(CImg<double> &src);
    void gaussVertical(CImg<double> &src);

    double density(double r);

   protected:
    // Filter parameters
    double sigma_;
    double *buffer_;

    // Deriche's coefficients
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

#endif  // SPACEVARIANTGAUSSFILTER_H
