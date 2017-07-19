#include <corem/module/parrot.h>

Parrot::Parrot(std::string id, retina_config_t *conf, double gain)
    : Module(id, conf) {
    this->gain = gain;
    image = new CImg<double>(columns, rows, 1, 1, 0.0);
}

Parrot::~Parrot() { delete image; }

void Parrot::update() {
    c_begin = clock();
    b_begin = boost::chrono::system_clock::now();
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    if (this->source_ports.size() > 1)
        std::cout << "Warning: you have more than one connection to the parrot "
                     "module, only the first one will be used."
                  << std::endl;

    //    std::cout << "Parrot::update() " << image->size() << std::endl;
    //    std::cout << "Parrot::update() " << source_ports[0].getData()->size()
    //    << std::endl;
    for (int i = 0; i < (int)image->size(); i++) {
        image->_data[i] = gain * source_ports[0].getData()->_data[i];
    }
    //    std::cout << "Parrot::update() " << image->size() << std::endl;
    //     image->assign(*(source_ports[0].getData()));
    //     *image *= gain;
    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *Parrot::getOutput() const {
    //    std::cout << "Parrot::getOutput()" << image->size() << std::endl;
    return image;
}
