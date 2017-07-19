#include <corem/input/localizedImpulse.h>

LocalizedImpulse::LocalizedImpulse(int columns, int rows,
                                   std::map<std::string, param_val_t> params)
    : Input(columns, rows, params) {
    start = *getParam("start", params).vi;
    stop = *getParam("stop", params).vi;
    amplitude = *getParam("amplitude", params).vd;
    // offset = *getParam("offset", params).vd;
    offset = getParam("offset", params).d;
    startX = *getParam("startX", params).vi;
    startY = *getParam("startY", params).vi;
    sizeX = *getParam("sizeX", params).vi;
    sizeY = *getParam("sizeY", params).vi;
    std::cout << "start.size: " << start.size() << std::endl;

    output = new CImg<double>(this->columns, this->rows, 1, 3);

    std::cout << this->columns << " " << this->rows << std::endl;

    cimg_forXY(*output, x, y) {
        (*output)(x, y, 0, 0) = 1.0;
        (*output)(x, y, 0, 1) = 1.0;
        (*output)(x, y, 0, 2) = 1.0;
    }
}

const CImg<double> *LocalizedImpulse::getData(int t) {
    bool found = false;
    for (int i = 0; i < (int)start.size() && !found; i++) {
        std::cout << start[i] << std::endl;
        if (t >= start[i] && t <= stop[i]) {
            std::cout << "T " << t << std::endl;
            cimg_forXY(*output, x, y) {
                if (x >= startX[i] && x < startX[i] + sizeX[i] &&
                    y >= startY[i] && y < startY[i] + sizeY[i]) {
                    (*output)(x, y, 0, 0) = amplitude[i] + offset;
                    (*output)(x, y, 0, 1) = amplitude[i] + offset;
                    (*output)(x, y, 0, 2) = amplitude[i] + offset;
                }
            }
            found = true;
        }
    }
    if (!found) {
        cimg_forXY(*output, x, y) {
            (*output)(x, y, 0, 0) = offset;
            (*output)(x, y, 0, 1) = offset;
            (*output)(x, y, 0, 2) = offset;
        }
    }

    return output;
}

void LocalizedImpulse::getSize(int &columns, int &rows) const {
    columns = this->columns;
    rows = this->rows;
}
