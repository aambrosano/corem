#include <corem/input/whiteNoise.h>

WhiteNoise::WhiteNoise(int columns, int rows,
                       std::map<std::string, param_val_t> params)
    : Input(columns, rows, params) {
    double mean = getParam("mean", params).d;
    double contrast1 = getParam("contrast1", params).d;
    double contrast2 = getParam("contrast2", params).d;

    switchTime = getParam("switchT", params).d;
    GaussianPeriod = getParam("period", params).d;

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
    generator1 = *(new boost::random::linear_congruential<unsigned long, 16807,
                                                          0, 2147483647>(seed));
    generator2 = *(new boost::random::linear_congruential<unsigned long, 16807,
                                                          0, 2147483647>(seed));
}

//------------------------------------------------------------------------------//

const CImg<double> *WhiteNoise::getData(int t) {
    // draw new value from Gaussian distribution
    if (t % (int)GaussianPeriod == 0) {
        double value = 0;
        if (t < switchTime)
            value = distribution1(generator1);
        else
            value = distribution2(generator2);

        if (value < 0.0) value = 0.0;

        cimg_forXY(*output, x, y) {
            (*output)(x, y, 0, 0) = value * 255,
                               (*output)(x, y, 0, 1) = value * 255,
                               (*output)(x, y, 0, 2) = value * 255;
        }
    }

    return output;
}

void WhiteNoise::getSize(int &columns, int &rows) const {
    columns = this->columns;
    rows = this->rows;
}
