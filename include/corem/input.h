#ifndef COREM_INPUT_H
#define COREM_INPUT_H

#include <corem/constants.h>

#include <CImg.h>
#include <corem/displayWithBar.h>
#include <map>
#include <string>

using namespace cimg_library;

class Input {
   public:
    virtual ~Input() = 0;

    virtual const CImg<double> *getData(
        int t) = 0;  // Gets input value at time t

    virtual void getSize(int &columns, int &rows) const = 0;

    int columns;
    int rows;

   protected:
    Input(int columns, int rows, std::map<std::string, param_val_t> params);
};

#endif
