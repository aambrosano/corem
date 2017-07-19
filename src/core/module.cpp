#include <corem/module.h>

ModulePort::ModulePort(std::vector<std::string> sources,
                       std::vector<Operation> operations,
                       SynapseType synapseType)
    : sources_(sources), operations_(operations), type_(synapseType) {
    data_ = new CImg<double>;
}

std::vector<std::string> ModulePort::getSources() { return sources_; }

std::vector<Operation> ModulePort::getOperations() { return operations_; }

SynapseType ModulePort::getSynapseType() { return type_; }

void ModulePort::update(CImg<double>* input) { *data_ = *input; }

const CImg<double>* ModulePort::getData() { return data_; }

void ModulePort::deallocate() {
    if (data_ != NULL) delete data_;
}

Module::Module(std::string id, retina_config_t* conf)
    : config(conf), id_(id), columns(conf->columns), rows(conf->rows) {
    if (verbose) {
        std::cout << "Module::Module() called with columns(" << columns
                  << "), rows(" << rows << "), id(" << id << ")" << std::endl;
    }
    this->elapsed_time = 0;
    this->elapsed_wall_time = 0;
}

Module::~Module() {
    std::vector<ModulePort>::iterator source;
    for (source = this->source_ports.begin();
         source != this->source_ports.end(); ++source) {
        (*source).deallocate();
    }
}

std::string Module::id() { return id_; }
