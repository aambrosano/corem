#include <COREM/core/shortTermPlasticity.h>

ShortTermPlasticity::ShortTermPlasticity(std::string id, unsigned int columns, unsigned int rows,
                                 double temporalStep, std::map<std::string, double> parameters,
                                 double Am, double Vm, double Em, double th, bool isTh)
    : Module(id, columns, rows, temporalStep, parameters) {

    slope_ = Am;
    offset_ = Vm;
    exponent_ = Em;

    kf_ = 0.0;
    kd_ = 0.0;
    tau_ = 1.0;
    VInf_ = 0.0;

    if (isTh) {
        isThreshold_ = true;
        threshold_ = th;
    } else {
        isThreshold_ = false;
        threshold_ = 0.0;
    }

    for(std::map<std::string, double>::const_iterator entry = parameters_.begin();
        entry != parameters_.end(); ++entry) {
        if (entry->first == "slope") slope_ = entry->second;
        else if (entry->first == "offset") offset_ = entry->second;
        else if (entry->first == "exponent") exponent_ = entry->second;
        else if (entry->first == "threshold") threshold_ = entry->second;
        else if (entry->first == "kf") kf_ = entry->second;
        else if (entry->first == "kd") kd_ = entry->second;
        else if (entry->first == "tau") tau_ = entry->second;
        else if (entry->first == "VInf") VInf_ = entry->second;
        else std::cerr << "ShortTermPlasticity(): You used an unrecognized parameter." << std::endl;
    }

    inputImage_ = new CImg<double>*[7];

    for (int i = 0; i < 7;i++)
      inputImage_[i] = new CImg<double>(columns, rows, 1, 1, 0.0);


    // exp(-step/tau)
    (inputImage_[5])->fill(-temporalStep_/tau_);
    (inputImage_[5])->exp();

    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.0);
}

ShortTermPlasticity::~ShortTermPlasticity() {
    delete outputImage_;
    for (int i = 0; i < 7; i++) delete inputImage_[i];
    delete inputImage_;
}

void ShortTermPlasticity::update() {
    // The module is not connected
    if (this->source_ports.size() == 0)
        return;

    // copy input image
    *(inputImage_[0]) = this->source_ports[0].getData();

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
    (*(inputImage_[6]))=(*(inputImage_[3]));
    (*(inputImage_[3])).fill(0.0);

    // abs(input)
    *inputImage_[1] = *inputImage_[0];
    (inputImage_[1])->abs();
    (*inputImage_[1])+= DBL_EPSILON_STP;

    // kmInf
    (inputImage_[4])->fill(VInf_);
    (inputImage_[4])->div(kd_*(*(inputImage_[1])));

    // update of km(t)
    (*inputImage_[3]) += (*inputImage_[4]);
    (*inputImage_[3]) -= (*(inputImage_[4])).mul(*(inputImage_[5]));
    (*inputImage_[3]) += (*(inputImage_[6])).mul(*(inputImage_[5]));

    // km(t)*abs(input)
    (*inputImage_[1]) = *inputImage_[0];
    (inputImage_[1])->abs();
    (inputImage_[1])->mul(*inputImage_[3]);

    // update of P
    (*inputImage_[2])+= kf_*(*inputImage_[1] - *inputImage_[2]);

    // Threshold
    if(isThreshold_){
        cimg_forXY((*inputImage_[0]), x, y) {
            if((*inputImage_[0])(x, y, 0, 0) < threshold_)
                (*inputImage_[0])(x, y, 0, 0) = threshold_;
        }
    }


    // slope, constant offset and exponent
    (*inputImage_[0]) *= slope_;
    (*inputImage_[0]) += offset_;
    (*inputImage_[0]) += (*inputImage_[2]);
    inputImage_[0]->pow(exponent_);

    *outputImage_ = *inputImage_[0];
}

CImg<double>* ShortTermPlasticity::getOutput() {
    return outputImage_;
}
