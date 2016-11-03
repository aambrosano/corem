#include <COREM/core/staticNonLinearity.h>

StaticNonLinearity::StaticNonLinearity(std::string id, unsigned int columns, unsigned int rows,
                                       double temporalStep, std::map<std::string, double> parameters,
                                       int type)
    : Module(id, columns, rows, temporalStep, parameters) {
    type_ = type;
    isThreshold_ = false;

    for(std::map<std::string, double>::const_iterator entry = parameters_.begin();
        entry != parameters_.end(); ++entry) {
        if (entry->first == "slope") slope_.push_back(entry->second);
        else if (entry->first == "offset") offset_.push_back(entry->second);
        else if (entry->first == "exponent") exponent_.push_back(entry->second);
        else if (entry->first == "max") exponent_.push_back(entry->second); // ?????
        else if (entry->first == "threshold") {
            threshold_.push_back(entry->second);
            isThreshold_ = true;
        }
        else if (entry->first == "start") start_.push_back(entry->second);
        else if (entry->first == "end") end_.push_back(entry->second);
        else std::cerr << "StaticNonLinearity(): You used an unrecognized parameter." << std::endl;
    }

    inputImage_  = new CImg<double>(columns, rows, 1, 1, 0.0);
    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.0);
    markers_     = new CImg<double>(columns, rows, 1, 1, 0.0);
}

StaticNonLinearity::~StaticNonLinearity() {
    delete inputImage_;
    delete outputImage_;
    delete markers_;
}

void StaticNonLinearity::update() {
    // The module is not connected
    if (this->source_ports.size() == 0)
        return;

    // copy input image
    *(inputImage_) = this->source_ports[0].getData();
    // polynomial function
    if(type_ == 0) {

        if (isThreshold_) {
            cimg_forXY((*inputImage_),x,y) {
                if((*inputImage_)(x, y, 0, 0) < threshold_[0])
                    (*inputImage_)(x, y, 0, 0) = threshold_[0];
            }
        }

        (*inputImage_) *= slope_[0];
        (*inputImage_) += offset_[0];
        inputImage_->pow(exponent_[0]);
    }

    // piecewise function
    else if(type_ == 1) {
            markers_->fill(0.0);
            for(unsigned int k = 0; k < slope_.size(); k++) {
                cimg_forXY((*inputImage_), x, y) {
                    if((*inputImage_)(x, y, 0, 0) >= start_[k] && (*inputImage_)(x, y, 0, 0) < end_[k] && (*markers_)(x, y, 0, 0) == 0.0) {
                        (*inputImage_)(x, y, 0, 0) *= slope_[k];
                        (*inputImage_)(x, y, 0, 0) += offset_[k];
                        (*inputImage_)(x, y, 0, 0) = pow((*inputImage_)(x,y,0,0),exponent_[k]);
                        (*markers_)(x, y, 0, 0) = 1.0;

                    }
                }

            }
    }

    // Symmetric sigmoid (only for negative values)
    else if(type_ == 2) {
        double absVal = 0.0;
        cimg_forXY((*inputImage_), x, y) {
            absVal = abs((*inputImage_)(x, y, 0, 0));
            (*inputImage_)(x, y, 0, 0) = sgn<double>((*inputImage_)(x, y, 0, 0))*(exponent_[0] / (1.0 + exp(-absVal*slope_[0] + offset_[0])));
        }
    }

    // Standard sigmoid
    else if(type_ == 3) {
        double value = 0.0;
        cimg_forXY((*inputImage_), x, y) {
            value = (*inputImage_)(x, y, 0, 0);
            (*inputImage_)(x, y, 0, 0) = (exponent_[0] / (1.0 + exp(-value*slope_[0] + offset_[0])));
        }

    }

    *outputImage_ = *inputImage_;

}

CImg<double>* StaticNonLinearity::getOutput(){
    return outputImage_;
}

template <typename T> int StaticNonLinearity::sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
