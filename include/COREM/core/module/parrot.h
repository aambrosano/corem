#ifndef PARROT_H
#define PARROT_H
#include <CImg.h>

#include "../constants.h"
#include "../module.h"

using namespace cimg_library;

class EXPORT Parrot : public Module {
   public:
    Parrot(std::string id, retina_config_t *conf);
    Parrot(const Parrot &copy);
    ~Parrot();

    virtual void update();
    virtual CImg<double> *getOutput();

   protected:
    CImg<double> *image;
};

#endif  // PARROT_H
