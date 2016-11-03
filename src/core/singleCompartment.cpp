#include <COREM/core/singleCompartment.h>
#include <cassert>

SingleCompartment::SingleCompartment(std::string id, unsigned int columns, unsigned int rows,
                                     double temporalStep, std::map<std::string, double> parameters)
    : Module (id, columns, rows, temporalStep, parameters) {
    Cm_ = 10e-9;
    Rm_ = 1.0;
    taum_ = 10e-6;
    El_ = 0.0;

    number_current_ports_ = 0;
    number_conductance_ports_ = 0;
    update_count_ = 0;

    current_potential_ = new CImg<double>(columns, rows, 1, 1, 0.0);
    last_potential_ = new CImg<double>(columns, rows, 1, 1, 0.0);
    total_cond_ = new CImg<double>(columns, rows, 1, 1, 0.0);
    potential_inf_ = new CImg<double>(columns, rows, 1, 1, 0.0);
    tau_ = new CImg<double>(columns, rows, 1, 1, 0.0);
    exp_term_ = new CImg<double>(columns, rows, 1, 1, 0.0);

    // TODO: the number of current and conductance ports is given at the
    // beginning, but there's no check that the number of connections is
    // compliant.
    for(std::map<std::string, double>::const_iterator entry = parameters_.begin();
        entry != parameters_.end(); ++entry) {
        if (entry->first == "number_current_ports") {
            assert(entry->second >= 0);
            number_current_ports_ = entry->second;
        }
        else if (entry->first == "number_conductance_ports") {
            assert(entry->second >= 0);
            number_conductance_ports_ = entry->second;
        }
        else if (entry->first == "Rm") {
            assert(entry->second > 0);
            Rm_ = entry->second;
        }
        else if (entry->first == "tau") {
            assert(entry->second > 0);
            taum_ = entry->second;
        }
        else if (entry->first == "Cm") {
            assert(entry->second > 0);
            Cm_ = entry->second;
        }
        else if (entry->first == "E") {
            assert(entry->second >= 0);
            El_ = entry->second;
        }
    }

    for (int i = 0; i < number_conductance_ports_; i++)
        E_.push_back(El_);

    conductances_ = new CImg<double>*[number_conductance_ports_];
    currents_ = new CImg<double>*[number_current_ports_];

    for (int i = 0; i < number_conductance_ports_; i++)
      conductances_[i] = new CImg<double>(columns, rows, 1, 1, 0.0);
    for (int j = 0; j < number_current_ports_; j++)
      currents_[j] = new CImg<double>(columns, rows, 1, 1, 0.0);
}

SingleCompartment::~SingleCompartment(){
    for (int i = 0; i < number_conductance_ports_;i++) delete conductances_[i];
    for (int j = 0; j < number_current_ports_; j++) delete currents_[j];

    if (conductances_) delete[] conductances_;
    if (currents_) delete[] currents_;

    if (current_potential_) delete current_potential_;
    if (last_potential_) delete last_potential_;
    if (total_cond_) delete total_cond_;
    if (potential_inf_) delete potential_inf_;
    if (tau_) delete tau_;
    if (exp_term_) delete exp_term_;
}

void SingleCompartment::update() {
#ifdef DEBUG
    bchrono::time_point<bchrono::system_clock> start = bchrono::system_clock::now();
#endif

    // The module is not connected
    if (this->source_ports.size() == 0)
        return;

    int icurr = 0;
    int icond = 0;

    for (unsigned int k = 0; k < source_ports.size(); k++) {
        SynapseType st = source_ports[k].getSynapseType();
        switch(st) {
        case CURRENT:
            *(currents_[icurr]) = source_ports[k].getData();
            icurr++;
            break;
        case CURRENT_INHIB:
            *(currents_[icurr]) = source_ports[k].getData();
            icurr++;
            break;
        case CONDUCTANCE:
            *(conductances_[icond]) = source_ports[k].getData();
            icond++;
            break;
        case CONDUCTANCE_INHIB:
            *(conductances_[icond]) = -source_ports[k].getData();
            icond++;
            break;
        }
    }

    (*last_potential_)=(*current_potential_);
    (*current_potential_).fill(0.0);

    // When there are conductance ports
    if (number_conductance_ports_ > 0) {

        (*total_cond_) = (*(conductances_[0]));
        for(int k = 1; k < number_conductance_ports_; k++)
            (*total_cond_) += (*(conductances_[k]));

        // in case total_cond = 0
        (*total_cond_) += DBL_EPSILON;

        // tau
        tau_->fill(Cm_);
        tau_->div((*total_cond_));

        // potential at infinity
        (*potential_inf_) = (*(conductances_[0]))*E_[0];

        for(int k = 1; k < number_conductance_ports_; k++)
            (*potential_inf_) += (*(conductances_[k]))*E_[k];
        for(int k = 0; k < number_current_ports_; k++)
            (*potential_inf_) += (*(currents_[k]));

        potential_inf_->div((*total_cond_));

    // When there are only current ports
    } else {
        //tau
        tau_->fill(taum_);
        // potential at infinity
        potential_inf_->fill(El_);
        for(int k = 0; k < number_current_ports_; k++)
            (*potential_inf_) += (*(currents_[k])) * Rm_;
    }

    // exponential term
    exp_term_->fill(-temporalStep_);
    exp_term_->div(*tau_);
    exp_term_->exp();

    // membrane potential update

    (*current_potential_) += (*potential_inf_);
    (*current_potential_) -= (*potential_inf_).mul(*exp_term_);
    (*current_potential_) += (*last_potential_).mul(*exp_term_);

#ifdef DEBUG
    bchrono::duration<double> diff = bchrono::system_clock::now()-start;
    constants::timesSinglecomp.push_back(diff.count());
    // std::cout << "SingleCompartment::update, " << diff.count() << std::endl;
#endif

}

CImg<double>* SingleCompartment::getOutput() {
    return current_potential_;
}
