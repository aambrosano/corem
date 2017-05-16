#ifndef COREM_TEMPORAL_MULTIMETER_H
#define COREM_TEMPORAL_MULTIMETER_H

#include "COREM/core/multimeter.h"

#include <algorithm>  // For std::min_element, std::max_element

class TemporalMultimeter : public Multimeter {
   public:
    TemporalMultimeter(unsigned int columns, unsigned int rows,
                       retina_config_t *conf, std::string id,
                       unsigned int timeToShow, unsigned int x, unsigned int y,
                       unsigned int sim_step);

    virtual void show(int waitTime);
    virtual void showGP(unsigned int start_from = 0);
    virtual void dump(std::string filename = "");
    virtual void record(/* CImg<double>* image */);

   protected:
    unsigned int x;
    unsigned int y;
    unsigned int sim_step;
};

#endif
