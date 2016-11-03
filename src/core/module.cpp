#include <COREM/core/module.h>

ModulePort::ModulePort(std::vector<std::string> sources, std::vector<Operation> operations,
                       SynapseType synapseType)
    : sources_(sources), operations_(operations), type_(synapseType) {}

std::vector<std::string> ModulePort::getSources() {
    return sources_;
}

std::vector<Operation> ModulePort::getOperations() {
    return operations_;
}

SynapseType ModulePort::getSynapseType() {
    return type_;
}

void ModulePort::update(CImg<double> input) {
    data_ = input;
}

const CImg<double> ModulePort::getData() {
    return data_;
}

Module::Module(std::string id, unsigned int columns, unsigned int rows, double temporalStep,
               std::map<std::string, double> parameters)
    : columns(columns), rows(rows), id_(id), temporalStep_(temporalStep), parameters_(parameters) {
    std::cout << "Module::Module() called with columns(" << columns << "), rows(" << rows
              << "), id(" << id << ")" << std::endl;
}

void Module::setSize(unsigned int columns, unsigned int rows) {
    columns = columns;
    rows = rows;
}

bool Module::operator==(std::string s) {
    return id_ == s;
}

std::string Module::getID()
{
    return id_;
}
