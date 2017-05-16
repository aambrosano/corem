#ifndef COREM_MODULE_H
#define COREM_MODULE_H

#include <map>
#include <string>
#include <vector>

#include <CImg.h>
#include <COREM/core/displayWithBar.h>

#include "constants.h"

using namespace cimg_library;

enum SynapseType { CURRENT, CURRENT_INHIB, CONDUCTANCE, CONDUCTANCE_INHIB };
enum Operation { ADD, SUB };

class ModulePort {
   public:
    ModulePort(std::vector<std::string> sources,
               std::vector<Operation> operations, SynapseType synapseType);

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
    Module(std::string id, retina_config_t *conf);

    virtual void update() {}
    virtual CImg<double> *getOutput() = 0;

    void addSource(Module *sourceModule, Operation operation,
                   SynapseType synapse);

    std::string id();
    std::vector<ModulePort> source_ports;

   protected:
    retina_config_t *config;
    std::string id_;
    unsigned int columns;
    unsigned int rows;
};

#endif  // COREM_MODULE_H
