#include "COREM/core/input/image.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const std::map<std::string, param_val_t> ImageSource::defaultParams =
    ImageSource::createDefaults();

std::map<std::string, param_val_t> ImageSource::createDefaults() {
    std::map<std::string, param_val_t> ret;
    ret["path"].s = new std::string(".");
    return ret;
}

ImageSource::ImageSource(unsigned int columns, unsigned int rows,
                         std::map<std::string, param_val_t> params)
    : Input(columns, rows, params, "ImageSource") {
    std::string path = *getParam("path", params, defaultParams).s;
    if (!fs::exists(path) || !fs::is_regular_file(path))
        throw std::runtime_error(
            (new std::string("File " + path + " not found or invalid."))
                ->c_str());

    input = new CImg<double>;
    input->assign(path.c_str());

    this->columns = input->width();
    this->rows = input->height();
}

CImg<double> *ImageSource::getData(unsigned int t) {
    // q.display(*input);
    return input;
}

void ImageSource::getSize(unsigned int &columns_, unsigned int &rows_) {
    columns_ = this->columns;
    rows_ = this->rows;
}
