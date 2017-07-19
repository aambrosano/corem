#include <corem/module/shortTermPlasticity.h>
#include <limits>

ShortTermPlasticity::ShortTermPlasticity(std::string id, retina_config_t *conf,
                                         double slope, double offset,
                                         double exponent, double threshold,
                                         double kf, double kd, double tau,
                                         double vinf)
    : Module(id, conf),
      slope_(slope),
      offset_(offset),
      exponent_(exponent),
      threshold_(threshold),
      kf_(kf),
      kd_(kd),
      tau_(tau),
      VInf_(vinf) {
    inputImage_ = new CImg<double> *[7];

    // TODO: why 0.1?
    for (int i = 0; i < 7; i++)
        inputImage_[i] = new CImg<double>(columns, rows, 1, 1, 0.1);

    // exp(-step/tau)
    (inputImage_[5])->fill(-config->simulationStep / tau_);
    (inputImage_[5])->exp();

    // TODO: why 0.1?
    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
}

ShortTermPlasticity::~ShortTermPlasticity() {
    delete outputImage_;
    for (int i = 0; i < 7; i++) delete inputImage_[i];
    delete inputImage_;
}

void ShortTermPlasticity::update() {
    c_begin = clock();
    b_begin =
        boost::chrono::system_clock::now();  // The module is not connected
    if (this->source_ports.size() == 0) return;

    // copy input image
    (inputImage_[0])->assign(*(this->source_ports[0].getData()));

    // kmInf = (Vinf/(kd*abs(input)))
    // km(t+1) = kmInf + [km(t) - kmInf]*exp(-step/tau)
    // P = P + kf*(km*abs(input) - P)

    // ImagePointer[0] -> input image
    // ImagePointer[1] -> abs(input image)
    // ImagePointer[2] -> P
    // ImagePointer[3] -> km(t)
    // ImagePointer[4] -> kmInf
    // ImagePointer[5] -> exp(-step/tau)
    // ImagePointer[6] -> km(t-1)

    // km(t-1)
    (*(inputImage_[6])) = (*(inputImage_[3]));
    (*(inputImage_[3])).fill(0.0);

    // abs(input)
    *inputImage_[1] = *inputImage_[0];
    (inputImage_[1])->abs();
    (*inputImage_[1]) += DBL_EPSILON_STP;

    // kmInf
    (inputImage_[4])->fill(VInf_);
    (inputImage_[4])->div(kd_ * (*(inputImage_[1])));

    // update of km(t)
    (*inputImage_[3]) += (*inputImage_[4]);
    (*inputImage_[3]) -= (*(inputImage_[4])).mul(*(inputImage_[5]));
    (*inputImage_[3]) += (*(inputImage_[6])).mul(*(inputImage_[5]));

    // km(t)*abs(input)
    (*inputImage_[1]) = *inputImage_[0];
    (inputImage_[1])->abs();
    (inputImage_[1])->mul(*inputImage_[3]);

    // update of P
    (*inputImage_[2]) += kf_ * (*inputImage_[1] - *inputImage_[2]);

    // Threshold
    if (threshold_ != std::numeric_limits<double>::infinity()) {
        cimg_forXY((*inputImage_[0]), x, y) {
            if ((*inputImage_[0])(x, y, 0, 0) < threshold_)
                (*inputImage_[0])(x, y, 0, 0) = threshold_;
        }
    }

    // slope, constant offset and exponent
    (*inputImage_[0]) *= slope_;
    (*inputImage_[0]) += offset_;
    (*inputImage_[0]) += (*inputImage_[2]);
    inputImage_[0]->pow(exponent_);

    *outputImage_ = *inputImage_[0];
    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *ShortTermPlasticity::getOutput() const {
    return outputImage_;
}
