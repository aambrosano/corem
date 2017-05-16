#include <COREM/core/input/impulse.h>

const std::map<std::string, param_val_t> Impulse::defaultParams =
    Impulse::createDefaults();

std::map<std::string, param_val_t> Impulse::createDefaults() {
  std::map<std::string, param_val_t> ret;
  ret["start"].d = 0.0;
  ret["stop"].d = 0.0;
  ret["amplitude"].d = 0.0;
  ret["offset"].d = 0.0;
  return ret;
}

// impulse::impulse(double startParam, double stopParam, double amplitudeParam,
// double offsetParam,int X, int Y){
Impulse::Impulse(unsigned int columns, unsigned int rows,
                 std::map<std::string, param_val_t> params)
    : Input(columns, rows, params, "Impulse") {
  start = getParam("start", params, Impulse::defaultParams).d;
  stop = getParam("stop", params, Impulse::defaultParams).d;
  amplitude = getParam("amplitude", params, Impulse::defaultParams).d;
  offset = getParam("offset", params, Impulse::defaultParams).d;

  std::cout << "Start: " << start << ", stop: " << stop
            << ", amplitude: " << amplitude << ", offset: " << offset
            << std::endl;

  output = new CImg<double>(this->columns, this->rows, 1, 3);

  std::cout << this->columns << " " << this->rows << std::endl;

  cimg_forXY(*output, x, y) {
    (*output)(x, y, 0, 0) = 1.0;
    (*output)(x, y, 0, 1) = 1.0;
    (*output)(x, y, 0, 2) = 1.0;
  }
}

CImg<double> *Impulse::getData(unsigned int t) {
  if (t >= start && t <= stop) {
    cimg_forXY(*output, x, y) {
      (*output)(x, y, 0, 0) = amplitude + offset;
      (*output)(x, y, 0, 1) = amplitude + offset;
      (*output)(x, y, 0, 2) = amplitude + offset;
    }
  } else {
    cimg_forXY(*output, x, y) {
      (*output)(x, y, 0, 0) = offset;
      (*output)(x, y, 0, 1) = offset;
      (*output)(x, y, 0, 2) = offset;
    }
  }

  return output;
}

void Impulse::getSize(unsigned int &columns, unsigned int &rows) {
  columns = this->columns;
  rows = this->rows;
}
