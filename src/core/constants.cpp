#include <COREM/core/constants.h>

namespace constants {
std::string retinaFolder = "COREM";
std::string retinaScript = "Retina_scripts/example_1.py";

const std::string resultID = "contrast";
const double outputfactor = 50.0;  // to scale LN analysis
}

param_val_t getParam(std::string name,
                     std::map<std::string, param_val_t> params,
                     std::map<std::string, param_val_t> defaults) {
    std::map<std::string, param_val_t>::iterator itp = params.find(name);
    std::map<std::string, param_val_t>::iterator itd = params.find(name);
    if (itd == defaults.end()) {
        std::cout
            << "You're trying to get a parameter with no default value. Are "
               "you sure that's what you want to do?"
            << std::endl;
    }

    if (itp != params.end())
        return itp->second;
    else if (itd != defaults.end())
        return itd->second;
    else
        error("Parameter " + name + " could not be found.");
}

void error(std::string msg) {
    std::cerr << msg << std::endl;
    throw std::runtime_error(msg);
}

void debug(std::string msg) {
    if (verbose) {
        std::cout << msg << std::endl;
    }
}
