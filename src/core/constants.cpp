#include <corem/constants.h>

namespace constants {
std::string retinaFolder = "COREM";
std::string retinaScript = "Retina_scripts/example_1.py";

const std::string resultID = "contrast";
const double outputfactor = 50.0;  // to scale LN analysis
}

param_val_t getParam(std::string name,
                     std::map<std::string, param_val_t> params) {
    std::map<std::string, param_val_t>::iterator itp = params.find(name);

    if (itp != params.end()) {
        return itp->second;
    } else {
        error("Parameter " + name + " could not be found.");
    }
}

void error(std::string msg) throw(std::runtime_error) {
    std::cerr << msg << std::endl;
    throw std::runtime_error(msg);
}

void debug(std::string msg) {
    if (verbose) {
        std::cout << msg << std::endl;
    }
}
