#include <COREM/core/module.h>

ModulePort::ModulePort(std::vector<std::string> sources,
                       std::vector<Operation> operations,
                       SynapseType synapseType)
    : sources_(sources), operations_(operations), type_(synapseType) {}

std::vector<std::string> ModulePort::getSources() { return sources_; }

std::vector<Operation> ModulePort::getOperations() { return operations_; }

SynapseType ModulePort::getSynapseType() { return type_; }

void ModulePort::update(CImg<double> input) { data_ = input; }

const CImg<double> ModulePort::getData() { return data_; }

Module::Module(std::string id, retina_config_t* conf)
    : columns(conf->columns), rows(conf->rows), id_(id), config(conf) {
    std::cout << "Module::Module() called with columns(" << columns
              << "), rows(" << rows << "), id(" << id << ")" << std::endl;
}

std::string Module::id() { return id_; }
