#ifndef COREM_INPUT_H
#define COREM_INPUT_H

#include "constants.h"

#include <CImg.h>
#include <COREM/core/displayWithBar.h>
#include <map>
#include <string>

using namespace cimg_library;

class Input {
   public:
    virtual CImg<double> *getData(
        unsigned int t) = 0;  // Gets input value at time t
    virtual void getSize(unsigned int &columns, unsigned int &rows) = 0;

    const std::string input_type;
    unsigned int columns;
    unsigned int rows;

   protected:
    Input(unsigned int columns, unsigned int rows,
          std::map<std::string, param_val_t> params,
          std::string input_type = "");
};

#endif
