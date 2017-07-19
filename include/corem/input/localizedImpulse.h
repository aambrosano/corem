#ifndef LOCALIZEDIMPULSE_H
#define LOCALIZEDIMPULSE_H

#include "../constants.h"
#include "../input.h"

#include <CImg.h>

#include <map>
#include <string>

using namespace cimg_library;
using namespace std;

class LocalizedImpulse : public Input {
   private:
    // parameters
    std::vector<int> start;
    std::vector<int> stop;
    std::vector<double> amplitude;
    double offset;
    std::vector<int> startX;
    std::vector<int> startY;
    std::vector<int> sizeX;
    std::vector<int> sizeY;

    // Output image
    CImg<double> *output;

   public:
    LocalizedImpulse(int columns, int rows,
                     std::map<std::string, param_val_t> params);

    virtual const CImg<double> *getData(int t);

    virtual void getSize(int &columns, int &rows) const;

   private:
    static const std::map<std::string, param_val_t> defaultParams;
    static std::map<std::string, param_val_t> createDefaults();
};

#endif  // LOCALIZEDIMPULSE_H
