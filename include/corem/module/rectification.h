#ifndef RECTIFICATION_H
#define RECTIFICATION_H

#include "../module.h"

class EXPORT Rectification : public Module {
   public:
    Rectification(std::string id, retina_config_t *conf, double T0,
                  double lambda, double V_th);
    Rectification(const Rectification &copy);
    ~Rectification();

    virtual void update();
    virtual const CImg<double> *getOutput() const;

   protected:
    CImg<double> *image;
    double T0;
    double lambda;
    double V_th;
};

#endif  // RECTIFICATION_H
