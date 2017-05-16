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
  Sequence(unsigned int columns, unsigned int rows,
           std::map<std::string, param_val_t> params);

  virtual CImg<double> *getData(unsigned int t);
  virtual void getSize(unsigned int &columns, unsigned int &rows);

private:
  static const std::map<std::string, param_val_t> defaultParams;
  static std::map<std::string, param_val_t> createDefaults();

  CImg<double> **inputSeq;
  unsigned int time_per_image;
  unsigned int numberImages;
};

#endif // SEQUENCE_H
