#include <COREM/core/module/staticNonLinearity.h>

StaticNonLinearity::StaticNonLinearity(std::string id, retina_config_t *conf,
                                       unsigned int type, double slope,
                                       double offset, double exponent,
                                       double threshold)
    : Module(id, conf),
      type_(type),
      slope_(slope),
      offset_(offset),
      exponent_(exponent),
      threshold_(threshold) {
    // TODO: why 0.1?
    inputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    markers_ = new CImg<double>(columns, rows, 1, 1, 0.1);
}

StaticNonLinearity::~StaticNonLinearity() {
    delete inputImage_;
    delete outputImage_;
    delete markers_;
}

void StaticNonLinearity::update() {
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    // copy input image
    *inputImage_ = this->source_ports[0].getData();
    // polynomial function
    if (type_ == 0) {
        if (threshold_ != std::numeric_limits<double>::infinity()) {
            cimg_forXY((*inputImage_), x, y) {
                if ((*inputImage_)(x, y, 0, 0) < threshold_)
                    (*inputImage_)(x, y, 0, 0) = threshold_;
            }
        }
        *inputImage_ *= slope_;
        *inputImage_ += offset_;
        inputImage_->pow(exponent_);
    }

    // Symmetric sigmoid (only for negative values)
    else if (type_ == 2) {
        double absVal = 0.0;
        cimg_forXY((*inputImage_), x, y) {
            absVal = std::abs((*inputImage_)(x, y, 0, 0));
            (*inputImage_)(x, y, 0, 0) =
                sgn<double>((*inputImage_)(x, y, 0, 0)) *
                (exponent_ / (1.0 + exp(-absVal * slope_ + offset_)));
        }
    }

    // Standard sigmoid
    else if (type_ == 3) {
        double value = 0.0;
        cimg_forXY((*inputImage_), x, y) {
            value = (*inputImage_)(x, y, 0, 0);
            (*inputImage_)(x, y, 0, 0) =
                (exponent_ / (1.0 + exp(-value * slope_ + offset_)));
        }
    }

    *outputImage_ = *inputImage_;
}

CImg<double> *StaticNonLinearity::getOutput() { return outputImage_; }

template <typename T>
int StaticNonLinearity::sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
