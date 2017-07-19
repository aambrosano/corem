#include <corem/input/impulse.h>

Impulse::Impulse(int columns, int rows,
                 std::map<std::string, param_val_t> params)
    : Input(columns, rows, params) {
    start = getParam("start", params).d;
    stop = getParam("stop", params).d;
    amplitude = getParam("amplitude", params).d;
    offset = getParam("offset", params).d;

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

const CImg<double> *Impulse::getData(int t) {
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

void Impulse::getSize(int &columns, int &rows) const {
    columns = this->columns;
    rows = this->rows;
}
