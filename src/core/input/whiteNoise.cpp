#include <COREM/core/input/whiteNoise.h>
// double mean, double contrast1, double contrast2, double period, double
// switchT

const std::map<std::string, param_val_t> WhiteNoise::defaultParams =
    WhiteNoise::createDefaults();

std::map<std::string, param_val_t> WhiteNoise::createDefaults() {
  std::map<std::string, param_val_t> ret;
  ret["mean"].d = 0.0;
  ret["contrast1"].d = 0.0;
  ret["contrast2"].d = 0.0;
  ret["period"].d = 0.0;
  ret["switchT"].d = 0.0;
  return ret;
}

// whiteNoise::whiteNoise(double mean, double contrast1, double contrast2,
// double period, double switchT, int X, int Y) {
WhiteNoise::WhiteNoise(unsigned int columns, unsigned int rows,
                       std::map<std::string, param_val_t> params)
    : Input(columns, rows, params, "WhiteNoise") {
  double mean = getParam("mean", params, WhiteNoise::defaultParams).d;
  double contrast1 = getParam("contrast1", params, WhiteNoise::defaultParams).d;
  double contrast2 = getParam("contrast2", params, WhiteNoise::defaultParams).d;

  switchTime = getParam("switchT", params, WhiteNoise::defaultParams).d;
  GaussianPeriod = getParam("period", params, WhiteNoise::defaultParams).d;

  distribution1 =
      *(new boost::normal_distribution<double>(mean, contrast1 * mean));
  distribution2 =
      *(new boost::normal_distribution<double>(mean, contrast2 * mean));

  output = new CImg<double>(columns, rows, 1, 3);

  cimg_forXY(*output, x, y) {
    (*output)(x, y, 0, 0) = 1.0, (*output)(x, y, 0, 1) = 1.0,
                       (*output)(x, y, 0, 2) = 1.0;
  }
}

//------------------------------------------------------------------------------//

void WhiteNoise::initializeDist(unsigned seed) {
  generator1 = *(new boost::random::linear_congruential<unsigned long, 16807, 0,
                                                        2147483647>(seed));
  generator2 = *(new boost::random::linear_congruential<unsigned long, 16807, 0,
                                                        2147483647>(seed));
}

//------------------------------------------------------------------------------//

// CImg<double>* whiteNoise::update(double t) {
CImg<double> *WhiteNoise::getData(unsigned int t) {
  // draw new value from Gaussian distribution
  if ((int)t % (int)GaussianPeriod == 0) {

    double value = 0;
    if (t < switchTime)
      value = distribution1(generator1);
    else
      value = distribution2(generator2);

    if (value < 0.0)
      value = 0.0;

    cimg_forXY(*output, x, y) {
      (*output)(x, y, 0, 0) = value * 255, (*output)(x, y, 0, 1) = value * 255,
                         (*output)(x, y, 0, 2) = value * 255;
    }
  }

  return output;
}

void WhiteNoise::getSize(unsigned int &columns, unsigned int &rows) {
  columns = this->columns;
  rows = this->rows;
}
