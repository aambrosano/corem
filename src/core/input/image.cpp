#include <corem/input/image.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ImageSource::ImageSource(int columns, int rows,
                         std::map<std::string, param_val_t> params)
    : Input(columns, rows, params) {
    std::string *path = getParam("path", params).s;
    if (!fs::exists(*path) || !fs::is_regular_file(*path))
        throw std::runtime_error(
            (new std::string("File " + *path + " not found or invalid."))
                ->c_str());

    input = new CImg<double>;
    input->assign(path->c_str());

    this->columns = input->width();
    this->rows = input->height();
    delete path;
}

ImageSource::~ImageSource() {
    if (input != NULL) delete input;
}

const CImg<double> *ImageSource::getData(int t) {
    UNUSED(t);  // The input is constant so t is not necessary here
    return input;
}

void ImageSource::getSize(int &columns_, int &rows_) const {
    columns_ = this->columns;
    rows_ = this->rows;
}
