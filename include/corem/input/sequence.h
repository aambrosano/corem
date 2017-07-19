#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "../constants.h"
#include "../input.h"

#include <CImg.h>

#include <map>
#include <string>

using namespace cimg_library;

class Sequence : public Input {
   protected:
   public:
    Sequence(int columns, int rows, std::map<std::string, param_val_t> params);

    virtual const CImg<double> *getData(int t);

    virtual void getSize(int &columns, int &rows) const;

   private:
    static const std::map<std::string, param_val_t> defaultParams;
    static std::map<std::string, param_val_t> createDefaults();

    CImg<double> **inputSeq;
    int time_per_image;
    int numberImages;
};

#endif  // SEQUENCE_H
