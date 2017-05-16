#include "COREM/core/module/parrot.h"

Parrot::Parrot(std::string id, retina_config_t *conf) : Module(id, conf) {}

void Parrot::update() {
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    if (this->source_ports.size() > 1)
        std::cout << "Warning: you have more than one connection to the parrot "
                     "module, only the first one will be used."
                  << std::endl;

    image = new CImg<double>(source_ports[0].getData());
}

CImg<double> *Parrot::getOutput() { return image; }
