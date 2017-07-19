#ifndef SIMPLESINGLECOMPARTMENT_H
#define SIMPLESINGLECOMPARTMENT_H

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

#include "../constants.h"
#include "../module.h"

class EXPORT SimpleSingleCompartment : public Module {
   public:
    SimpleSingleCompartment(std::string id, retina_config_t *conf);
    SimpleSingleCompartment(const SingleCompartment &copy);
    ~SimpleSingleCompartment();

    virtual void update();
    virtual CImg<double> *getOutput();

   protected:
    // image buffers
    CImg<double> *conductance_;
    CImg<double> *current_;
    // Nernst potentials
    std::vector<double> E_;
    // membrane capacitance, resistance and tau
    double Cm_;
    double Rm_;
    double taum_;
    double El_;
    // membrane potential
    CImg<double> *current_potential_;
    CImg<double> *last_potential_;
    CImg<double> *total_cond_;
    CImg<double> *potential_inf_;
    CImg<double> *tau_;
    CImg<double> *exp_term_;
};

#endif  // SINGLECOMPARTMENT_H
