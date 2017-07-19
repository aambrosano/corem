#ifndef HIGHPASS_H
#define HIGHPASS_H

#include "../module.h"

class EXPORT HighPass : public Module {
   public:
    HighPass(std::string id, retina_config_t *conf, double w, double tau);
    HighPass(const HighPass &copy);
    ~HighPass();

    virtual void update();
    virtual const CImg<double> *getOutput() const;

   protected:
    int N;
    double w;
    double tau;

    CImg<double> **last_inputs;  // Input memory
    CImg<double> *output;
    double *a;  // Filter coefficients
};

#endif  // HIGHPASS_H
