#include <COREM/core/input/gratingGenerator.h>
#include <COREM/core/input.h>

const std::map<std::string, param_val_t> GratingGenerator::defaultParams =
    GratingGenerator::createDefaults();

std::map<std::string, param_val_t> GratingGenerator::createDefaults() {
  std::map<std::string, param_val_t> ret;
  ret["ptype"].i = 0;
  ret["pstep"].d = 0.0;
  ret["plengthB"].d = 0.0;
  ret["plength"].d = 0.0;
  ret["plength2"].d = 0.0;
  ret["pfreq"].d = 0.0;
  ret["pT"].d = 0.0;
  ret["pLum"].d = 0.0;
  ret["pCont"].d = 0.0;
  ret["pphi"].d = 0.0;
  ret["pphi_t"].d = 0.0;
  ret["ptheta"].d = 0.0;
  ret["red"].d = 0.0;
  ret["green"].d = 0.0;
  ret["blue"].d = 0.0;
  ret["pred_phi"].d = 0.0;
  ret["pgreen_phi"].d = 0.0;
  ret["pblue_phi"].d = 0.0;
  return ret;
}

// GratingGenerator::GratingGenerator(int ptype, double pstep, double plengthB,
// double plength, double plength2, int pX, int pY, double pfreq, double pT,
// double pLum, double pCont, double pphi, double pphi_t, double ptheta, double
// red, double green, double blue, double pred_phi, double pgreen_phi, double
// pblue_phi){
//
GratingGenerator::GratingGenerator(unsigned int columns, unsigned int rows,
                                   std::map<string, param_val_t> params)
    : Input(columns, rows, params, "GratingGenerator") {
  type = getParam("ptype", params, defaultParams).i;
  step = getParam("pstep", params, defaultParams).d;
  lengthB = getParam("plengthB", params, defaultParams).d;
  length = getParam("plength", params, defaultParams).d;
  length2 = getParam("plength2", params, defaultParams).d;
  freq = getParam("pfreq", params, defaultParams).d;
  T = getParam("pT", params, defaultParams).d;
  Lum = getParam("pLum", params, defaultParams).d;
  Cont = getParam("pCont", params, defaultParams).d;
  phi = getParam("pphi", params, defaultParams).d;
  phi_t = getParam("pphi_t", params, defaultParams).d;
  theta = getParam("ptheta", params, defaultParams).d;
  r = getParam("red", params, defaultParams).d;
  g = getParam("green", params, defaultParams).d;
  b = getParam("blue", params, defaultParams).d;

  red_phi = getParam("pred_phi", params, defaultParams).d;
  green_phi = getParam("pgreen_phi", params, defaultParams).d;
  blue_phi = getParam("pblue_phi", params, defaultParams).d;

  Bsize = (int)(lengthB / step);
  first_grating_size = (int)(length / step);
  second_grating_size = (type == 2) ? (int)(length2 / step) : 0;

  x0 = (int)(this->columns / 2);
  y0 = (int)(this->rows / 2);
  cos_theta = cos(theta);
  sin_theta = sin(theta);
  A = Cont * Lum;
  aux = *(new CImg<double>(this->columns, this->rows, 1, 3));
}

//------------------------------------------------------------------------------//

// CImg <double>* GratingGenerator::compute_grating(double t){
CImg<double> *GratingGenerator::getData(unsigned int t) {

  if (t >= 0 && t < Bsize) {
    cimg_forXY(aux, x, y) {
      aux(x, y, 0, 0) = r * Lum, aux(x, y, 0, 1) = g * Lum,
                   aux(x, y, 0, 2) = b * Lum;
    }
  }

  double Pi = 3.14159265;

  if (t >= Bsize && t < Bsize + first_grating_size) {
    if (type == 0) {
      cimg_forXY(aux, x, y) {

        double value3 =
            b * Lum +
            b * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                         phi / 2 - freq * step * t) *
                            2 * Pi +
                        blue_phi * Pi);
        double value2 =
            g * Lum +
            g * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                         phi / 2 - freq * step * t) *
                            2 * Pi +
                        green_phi * Pi);
        double value1 =
            r * Lum +
            r * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                         phi / 2 - freq * step * t) *
                            2 * Pi +
                        red_phi * Pi);
        aux(x, y, 0, 0) = value1;
        aux(x, y, 0, 1) = value2;
        aux(x, y, 0, 2) = value3;
      }

    } else {
      if (type == 1) {
        cimg_forXY(aux, x, y) {
          double value3 =
              b * Lum +
              b * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                           phi / 2) *
                              2 * Pi +
                          blue_phi * Pi) *
                  cos((freq * step * t + phi_t / 2) * 2 * Pi);
          double value2 =
              g * Lum +
              g * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                           phi / 2) *
                              2 * Pi +
                          green_phi * Pi) *
                  cos((freq * step * t + phi_t / 2) * 2 * Pi);
          double value1 =
              r * Lum +
              r * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                           phi / 2) *
                              2 * Pi +
                          red_phi * Pi) *
                  cos((freq * step * t + phi_t / 2) * 2 * Pi);
          aux(x, y, 0, 0) = value1;
          aux(x, y, 0, 1) = value2;
          aux(x, y, 0, 2) = value3;
        }
      } else {
        cimg_forXY(aux, x, y) {
          double value3 =
              b * Lum +
              b * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                           phi / 2) *
                              2 * Pi +
                          blue_phi * Pi);
          double value2 =
              g * Lum +
              g * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                           phi / 2) *
                              2 * Pi +
                          green_phi * Pi);
          double value1 =
              r * Lum +
              r * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                           phi / 2) *
                              2 * Pi +
                          red_phi * Pi);
          aux(x, y, 0, 0) = value1;
          aux(x, y, 0, 1) = value2;
          aux(x, y, 0, 2) = value3;
        }
      }
    }
  }

  // adding the reversed grating in case that type==2
  if (t >= Bsize + first_grating_size &&
      t < Bsize + first_grating_size + second_grating_size) {
    cimg_forXY(aux, x, y) {
      double value3 =
          b * Lum -
          b * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                       phi / 2) *
                          2 * Pi +
                      blue_phi * Pi);
      double value2 =
          g * Lum -
          g * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                       phi / 2) *
                          2 * Pi +
                      green_phi * Pi);
      double value1 =
          r * Lum -
          r * A * cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                       phi / 2) *
                          2 * Pi +
                      red_phi * Pi);
      aux(x, y, 0, 0) = value1;
      aux(x, y, 0, 1) = value2;
      aux(x, y, 0, 2) = value3;
    }
  }

  return &aux;
}

void GratingGenerator::getSize(unsigned int &columns, unsigned int &rows) {
  columns = this->columns;
  rows = this->rows;
}
