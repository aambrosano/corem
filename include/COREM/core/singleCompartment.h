#ifndef SINGLECOMPARTMENT_H
#define SINGLECOMPARTMENT_H

/* BeginDocumentation
 * Name: singleCompartment
 *
 * Description: generic single-compartment model of the membrane potential,
 * updated by Euler's method. The membrane potential is driven by conductance
 * and current inputs
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: module
 */

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

#include "module.h"
#include "constants.h"

class EXPORT SingleCompartment : public Module {
public:
    // Constructor, copy, destructor.
    SingleCompartment(std::string id, unsigned int columns, unsigned int rows, double temporalStep,
                      std::map<std::string, double> parameters);
    SingleCompartment(const SingleCompartment &copy);
    ~SingleCompartment();

    virtual void update();
    virtual CImg<double>* getOutput();

protected:
    // image buffers
    CImg<double>** conductances_;
    CImg<double>** currents_;
    int number_current_ports_;
    int number_conductance_ports_;
    int update_count_;
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

#endif // SINGLECOMPARTMENT_H
