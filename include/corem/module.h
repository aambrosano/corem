#ifndef COREM_MODULE_H
#define COREM_MODULE_H

#include <ctime>
#include <map>
#include <string>
#include <vector>

#include <CImg.h>
#include <corem/displayWithBar.h>
#include <boost/chrono.hpp>

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
    void update(CImg<double> *input);
    void deallocate();
    const CImg<double> *getData();

   protected:
    std::vector<std::string> sources_;
    std::vector<Operation> operations_;
    SynapseType type_;
    CImg<double> *data_;
};

class EXPORT Module {
   public:
    Module(std::string id, retina_config_t *conf);

    virtual ~Module() = 0;
    virtual void update() = 0;
    virtual const CImg<double> *getOutput() const = 0;

    void addSource(Module *sourceModule, Operation operation,
                   SynapseType synapse);

    std::string id();
    std::vector<ModulePort> source_ports;

    double elapsed_time;
    double elapsed_wall_time;

   protected:
    retina_config_t *config;
    std::string id_;
    int columns;
    int rows;

    clock_t c_begin, c_end;  // For measuring CPU time
    boost::chrono::system_clock::time_point b_begin,
        b_end;  // For measuring wall clock time
};

#endif  // COREM_MODULE_H
