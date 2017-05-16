#include <COREM/core/input/fixationalMovGrating.h>

const std::map<std::string, param_val_t> FixationalMovGrating::defaultParams =
    FixationalMovGrating::createDefaults();

std::map<std::string, param_val_t> FixationalMovGrating::createDefaults() {
    std::map<std::string, param_val_t> ret;
    ret["radius"].d = 1.0;
    ret["jitter"].d = 15.0;
    ret["period"].d = 10.0;
    ret["step"].d = 1.0;
    ret["luminance"].d = 100.0;
    ret["contrast"].d = 0.5;
    ret["orientation"].d = 0.0;
    ret["red_weight"].d = 1.0;
    ret["green_weight"].d = 1.0;
    ret["blue_weight"].d = 1.0;
    ret["type1"].i = 0;   // was t1
    ret["type2"].i = 0;   // was t2
    ret["tswitch"].i = 0; // was ts
    return ret;
}

FixationalMovGrating::FixationalMovGrating(unsigned int columns,
                                           unsigned int rows,
                                           std::map<string, param_val_t> params)
    : Input(columns, rows, params, "FixationalMovGrating") {
    unsigned seed1, seed2, seed3, seed4;
    seed1 = boost::chrono::system_clock::now().time_since_epoch().count();
    seed3 = boost::chrono::system_clock::now().time_since_epoch().count();

    this->columns = columns;
    this->rows = rows;

    this->type1 = getParam("type1", params, defaultParams).i;
    this->type2 = getParam("type2", params, defaultParams).i;
    this->tswitch = getParam("tswitch", params, defaultParams).i;
    this->circle_radius = getParam("radius", params, defaultParams).d;
    this->jitter_period = getParam("jitter", params, defaultParams).d;
    this->spatial_period = getParam("period", params, defaultParams).d;
    this->step_size = getParam("step", params, defaultParams).d;
    this->Lum = getParam("luminance", params, defaultParams).d;
    this->Cont = getParam("contrast", params, defaultParams).d;
    this->theta = getParam("orientation", params, defaultParams).d;
    this->r = getParam("red_weight", params, defaultParams).d;
    this->g = getParam("green_weight", params, defaultParams).d;
    this->b = getParam("blue_weight", params, defaultParams).d;

    x0 = this->columns / 2;
    y0 = this->rows / 2;
    cos_theta = cos(theta);
    sin_theta = sin(theta);
    A = Cont * Lum;
    aux = *(new CImg<double>(this->columns, this->rows, 1, 3));

    generator1 =
        *(new boost::random::linear_congruential<unsigned long, 16807, 0,
                                                 2147483647>(seed1));
    distribution1 =
        *(new boost::random::normal_distribution<double>(0.0, step_size));

    if (type1 == 0) {
        seed2 = boost::chrono::system_clock::now().time_since_epoch().count();
    } else {
        seed2 = seed1;
    }

    generator2 =
        *(new boost::random::linear_congruential<unsigned long, 16807, 0,
                                                 2147483647>(seed2));
    distribution2 =
        *(new boost::random::normal_distribution<double>(0.0, step_size));

    generator3 =
        *(new boost::random::linear_congruential<unsigned long, 16807, 0,
                                                 2147483647>(seed3));

    if (type2 == 0) {
        seed4 = boost::chrono::system_clock::now().time_since_epoch().count();
    } else {
        seed4 = seed3;
    }

    generator4 =
        *(new boost::random::linear_congruential<unsigned long, 16807, 0,
                                                 2147483647>(seed4));

    Pi = 3.14159265;
    jitter1 = 0.0;
    jitter2 = 0.0;
    j1 = -step_size;
    value1 = value2 = value3 = 0.0;
}

CImg<double> *FixationalMovGrating::getData(unsigned int t) {
    if ((int)t % (int)jitter_period == 0) {

        if (t < tswitch) {
            j1 = distribution1(generator1);
            j2 = distribution2(generator2);

            // shift fixed
            //            if(type1 == 0){
            //                j1 = -j1;
            //                j2 = -j1;
            //            }else{
            //                j1 = -j1;
            //                j2 = j1;
            //            }

        } else {
            j1 = distribution1(generator3);
            j2 = distribution2(generator4);

            // shift fixed
            //            if(type2 == 0){
            //                j1 = -j1;
            //                j2 = -j1;
            //            }else{
            //                j1 = -j1;
            //                j2 = j1;
            //            }
        }

        jitter1 = j1;
        jitter2 = j2;

        cimg_forXY(aux, x, y) {

            radius = sqrt((double(x) - double(x0)) * (double(x) - double(x0)) +
                          (double(y) - double(y0)) * (double(y) - double(y0)));

            if (radius < circle_radius) {
                value3 = b * Lum +
                         b * A * cos(Pi / 2 +
                                     (((x - x0 + jitter1) * cos_theta +
                                       (y - y0 + jitter1) * sin_theta) /
                                      spatial_period) *
                                         2 * Pi);
                value2 = g * Lum +
                         g * A * cos(Pi / 2 +
                                     (((x - x0 + jitter1) * cos_theta +
                                       (y - y0 + jitter1) * sin_theta) /
                                      spatial_period) *
                                         2 * Pi);
                value1 = r * Lum +
                         r * A * cos(Pi / 2 +
                                     (((x - x0 + jitter1) * cos_theta +
                                       (y - y0 + jitter1) * sin_theta) /
                                      spatial_period) *
                                         2 * Pi);
            } else {
                value3 = b * Lum +
                         b * A * cos(Pi / 2 +
                                     (((x - x0 + jitter2) * cos_theta +
                                       (y - y0 + jitter2) * sin_theta) /
                                      spatial_period) *
                                         2 * Pi);
                value2 = g * Lum +
                         g * A * cos(Pi / 2 +
                                     (((x - x0 + jitter2) * cos_theta +
                                       (y - y0 + jitter2) * sin_theta) /
                                      spatial_period) *
                                         2 * Pi);
                value1 = r * Lum +
                         r * A * cos(Pi / 2 +
                                     (((x - x0 + jitter2) * cos_theta +
                                       (y - y0 + jitter2) * sin_theta) /
                                      spatial_period) *
                                         2 * Pi);
            }

            aux(x, y, 0, 0) = value1;
            aux(x, y, 0, 1) = value2;
            aux(x, y, 0, 2) = value3;
        }
    }

    return &aux;
}

void FixationalMovGrating::getSize(unsigned int &columns, unsigned int &rows) {
    columns = this->columns;
    rows = this->rows;
}
