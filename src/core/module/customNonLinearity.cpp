#include <COREM/core/module/customNonLinearity.h>

CustomNonLinearity::CustomNonLinearity(std::string id, retina_config_t *conf,
                                       std::vector<double> slope,
                                       std::vector<double> offset,
                                       std::vector<double> exponent,
                                       std::vector<double> start,
                                       std::vector<double> end)
    : Module(id, conf),
      slope_(slope),
      offset_(offset),
      exponent_(exponent),
      start_(start),
      end_(end) {
    // TODO: why 0.1?
    inputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    markers_ = new CImg<double>(columns, rows, 1, 1, 0.1);
}

CustomNonLinearity::~CustomNonLinearity() {
    delete inputImage_;
    delete outputImage_;
    delete markers_;
}

void CustomNonLinearity::update() {
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    // copy input image
    *inputImage_ = this->source_ports[0].getData();

    // Probably obtainable with some threshold functions
    // piecewise function
    markers_->fill(0.0);
    for (unsigned int k = 0; k < slope_.size(); k++) {
        cimg_forXY((*inputImage_), x, y) {
            if ((*inputImage_)(x, y, 0, 0) >= start_[k] &&
                (*inputImage_)(x, y, 0, 0) < end_[k] &&
                (*markers_)(x, y, 0, 0) == 0.0) {
                (*inputImage_)(x, y, 0, 0) *= slope_[k];
                (*inputImage_)(x, y, 0, 0) += offset_[k];
                (*inputImage_)(x, y, 0, 0) =
                    std::pow((*inputImage_)(x, y, 0, 0), exponent_[k]);
                (*markers_)(x, y, 0, 0) = 1.0;
            }
        }
    }

    *outputImage_ = *inputImage_;
}

CImg<double> *CustomNonLinearity::getOutput() { return outputImage_; }
