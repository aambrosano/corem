#ifndef COREM_TEMPORAL_MULTIMETER_H
#define COREM_TEMPORAL_MULTIMETER_H

#include <corem/multimeter.h>

#include <algorithm>  // For std::min_element, std::max_element

class TemporalMultimeter : public Multimeter {
   public:
    TemporalMultimeter(int columns, int rows, retina_config_t *conf,
                       std::string id, int timeToShow, int x, int y,
                       int sim_step);

    virtual void show(int waitTime);
    virtual void showGP(int start_from = 0);
    virtual void dump(std::string filename = "");
    virtual void record(/* CImg<double>* image */);

   protected:
    int x;
    int y;
    int sim_step;
};

#endif
