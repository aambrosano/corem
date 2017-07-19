#ifndef PARROT_H
#define PARROT_H

#include "../module.h"

class EXPORT Parrot : public Module {
   public:
    Parrot(std::string id, retina_config_t *conf, double gain);
    Parrot(const Parrot &copy);
    ~Parrot();

    virtual void update();
    virtual const CImg<double> *getOutput() const;

   protected:
    CImg<double> *image;
    double gain;
};

#endif  // PARROT_H
