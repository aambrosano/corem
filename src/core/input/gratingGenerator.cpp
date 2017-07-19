#include <corem/input.h>
#include <corem/input/gratingGenerator.h>

GratingGenerator::GratingGenerator(int columns, int rows,
                                   std::map<string, param_val_t> params)
    : Input(columns, rows, params) {
    type = getParam("ptype", params).i;
    step = getParam("pstep", params).d;
    lengthB = getParam("plengthB", params).d;
    length = getParam("plength", params).d;
    length2 = getParam("plength2", params).d;
    freq = getParam("pfreq", params).d;
    T = getParam("pT", params).d;
    Lum = getParam("pLum", params).d;
    Cont = getParam("pCont", params).d;
    phi = getParam("pphi", params).d;
    phi_t = getParam("pphi_t", params).d;
    theta = getParam("ptheta", params).d;
    r = getParam("red", params).d;
    g = getParam("green", params).d;
    b = getParam("blue", params).d;

    red_phi = getParam("pred_phi", params).d;
    green_phi = getParam("pgreen_phi", params).d;
    blue_phi = getParam("pblue_phi", params).d;

    Bsize = (int)(lengthB / step);
    first_grating_size = (int)(length / step);
    second_grating_size = (type == 2) ? (int)(length2 / step) : 0;

    x0 = (int)(this->columns / 2);
    y0 = (int)(this->rows / 2);
    cos_theta = cos(theta);
    sin_theta = sin(theta);
    A = Cont * Lum;
    aux = new CImg<double>(this->columns, this->rows, 1, 3);
}

//------------------------------------------------------------------------------//

// CImg <double>* GratingGenerator::compute_grating(double t){
const CImg<double> *GratingGenerator::getData(int t) {
    if (t >= 0 && t < Bsize) {
        cimg_forXY(*aux, x, y) {
            (*aux)(x, y, 0, 0) = r * Lum, (*aux)(x, y, 0, 1) = g * Lum,
                            (*aux)(x, y, 0, 2) = b * Lum;
        }
    }

    double Pi = 3.14159265;

    if (t >= Bsize && t < Bsize + first_grating_size) {
        if (type == 0) {
            cimg_forXY(*aux, x, y) {
                double value3 =
                    b * Lum +
                    b * A *
                        cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                             phi / 2 - freq * step * t) *
                                2 * Pi +
                            blue_phi * Pi);
                double value2 =
                    g * Lum +
                    g * A *
                        cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                             phi / 2 - freq * step * t) *
                                2 * Pi +
                            green_phi * Pi);
                double value1 =
                    r * Lum +
                    r * A *
                        cos((((x - x0) * cos_theta + (y - y0) * sin_theta) / T +
                             phi / 2 - freq * step * t) *
                                2 * Pi +
                            red_phi * Pi);
                (*aux)(x, y, 0, 0) = value1;
                (*aux)(x, y, 0, 1) = value2;
                (*aux)(x, y, 0, 2) = value3;
            }

        } else {
            if (type == 1) {
                cimg_forXY(*aux, x, y) {
                    double value3 =
                        b * Lum +
                        b * A *
                            cos((((x - x0) * cos_theta + (y - y0) * sin_theta) /
                                     T +
                                 phi / 2) *
                                    2 * Pi +
                                blue_phi * Pi) *
                            cos((freq * step * t + phi_t / 2) * 2 * Pi);
                    double value2 =
                        g * Lum +
                        g * A *
                            cos((((x - x0) * cos_theta + (y - y0) * sin_theta) /
                                     T +
                                 phi / 2) *
                                    2 * Pi +
                                green_phi * Pi) *
                            cos((freq * step * t + phi_t / 2) * 2 * Pi);
                    double value1 =
                        r * Lum +
                        r * A *
                            cos((((x - x0) * cos_theta + (y - y0) * sin_theta) /
                                     T +
                                 phi / 2) *
                                    2 * Pi +
                                red_phi * Pi) *
                            cos((freq * step * t + phi_t / 2) * 2 * Pi);
                    (*aux)(x, y, 0, 0) = value1;
                    (*aux)(x, y, 0, 1) = value2;
                    (*aux)(x, y, 0, 2) = value3;
                }
            } else {
                cimg_forXY(*aux, x, y) {
                    double value3 =
                        b * Lum +
                        b * A *
                            cos((((x - x0) * cos_theta + (y - y0) * sin_theta) /
                                     T +
                                 phi / 2) *
                                    2 * Pi +
                                blue_phi * Pi);
                    double value2 =
                        g * Lum +
                        g * A *
                            cos((((x - x0) * cos_theta + (y - y0) * sin_theta) /
                                     T +
                                 phi / 2) *
                                    2 * Pi +
                                green_phi * Pi);
                    double value1 =
                        r * Lum +
                        r * A *
                            cos((((x - x0) * cos_theta + (y - y0) * sin_theta) /
                                     T +
                                 phi / 2) *
                                    2 * Pi +
                                red_phi * Pi);
                    (*aux)(x, y, 0, 0) = value1;
                    (*aux)(x, y, 0, 1) = value2;
                    (*aux)(x, y, 0, 2) = value3;
                }
            }
        }
    }

    // adding the reversed grating in case that type==2
    if (t >= Bsize + first_grating_size &&
        t < Bsize + first_grating_size + second_grating_size) {
        cimg_forXY(*aux, x, y) {
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
            (*aux)(x, y, 0, 0) = value1;
            (*aux)(x, y, 0, 1) = value2;
            (*aux)(x, y, 0, 2) = value3;
        }
    }

    return aux;
}

void GratingGenerator::getSize(int &columns, int &rows) const {
    columns = this->columns;
    rows = this->rows;
}
