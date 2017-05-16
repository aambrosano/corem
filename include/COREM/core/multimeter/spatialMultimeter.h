#ifndef COREM_SPATIAL_MULTIMETER_H
#define COREM_SPATIAL_MULTIMETER_H

#include "COREM/core/multimeter.h"

#include <algorithm>  // For std::min_element, std::max_element

enum Orientation { HORIZONTAL, VERTICAL };

class SpatialMultimeter : public Multimeter {
   public:
    SpatialMultimeter(unsigned int columns, unsigned int rows,
                      retina_config_t *conf, std::string id,
                      unsigned int timeToShow, Orientation orientation,
                      unsigned int row_or_column);

    virtual void show(int waitTime);
    virtual void showGP(unsigned int start_from = 0);
    virtual void dump(std::string filename = "");
    virtual void record(/* CImg<double>* image */);

   protected:
    Orientation or_;  // Orientation of the multimeter, either it takes a column
                      // or a row from an image
    unsigned int line;  // The id of the column or the row to sample
    int waitTime;
};

#endif
