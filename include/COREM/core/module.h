#ifndef COREM_MODULE_H
#define COREM_MODULE_H

#include <string>
#include <vector>
#include <map>

#include <CImg.h>

#include "constants.h"

using namespace cimg_library;

enum SynapseType { CURRENT, CURRENT_INHIB, CONDUCTANCE, CONDUCTANCE_INHIB };
enum Operation { ADD, SUB };

class ModulePort {
public:
    ModulePort(std::vector<std::string> sources, std::vector<Operation> operations, SynapseType synapseType);

    std::vector<std::string> getSources();
    std::vector<Operation> getOperations();
    SynapseType getSynapseType();
    void update(CImg<double> input);
    const CImg<double> getData();

protected:
    std::vector<std::string> sources_;
    std::vector<Operation> operations_;
    SynapseType type_;
    CImg<double> data_;
};

class EXPORT Module {
public:
    Module(std::string id, unsigned int columns, unsigned int rows, double temporalStep,
           std::map<std::string, double> parameters);

    void setSize(unsigned int columns, unsigned int rows);

    // TODO: find a way to remove isCurrent and port, as they are only used in
    // SingleCompartment()
    virtual void update() {}
    virtual CImg<double> *getOutput() { return NULL; }

    void addSource(Module *sourceModule, Operation operation, SynapseType synapse);
    bool operator==(std::string s);

    std::string getID();

    unsigned int columns, rows;
    std::vector<ModulePort> source_ports;

protected:
    std::string id_;
    double temporalStep_;
    std::map<std::string, double> parameters_;
};

#endif // COREM_MODULE_H
