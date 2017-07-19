#include <corem/module/singleCompartment.h>
#include <cassert>

SingleCompartment::SingleCompartment(std::string id, retina_config_t *conf,
                                     unsigned int current_ports,
                                     unsigned int conductance_ports, double Cm,
                                     double Rm, double tau,
                                     std::vector<double> E)
    : Module(id, conf),
      number_current_ports_(current_ports),
      number_conductance_ports_(conductance_ports),
      E_(E),
      Cm_(Cm),
      Rm_(Rm),
      taum_(tau) {
    assert(Cm_ >= 0);
    assert(taum_ >= 0);
    El_ = E_[0];

    // TODO: why 0.1?
    current_potential_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    last_potential_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    total_cond_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    potential_inf_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    tau_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    exp_term_ = new CImg<double>(columns, rows, 1, 1, 0.1);

    // TODO: the number of current and conductance ports is given at the
    // beginning, but there's no check that the number of connections is
    // compliant.

    conductances_ = new CImg<double> *[number_conductance_ports_];
    currents_ = new CImg<double> *[number_current_ports_];

    // TODO: why 0.1?
    for (int i = 0; i < number_conductance_ports_; i++)
        conductances_[i] = new CImg<double>(columns, rows, 1, 1, 0.1);
    for (int j = 0; j < number_current_ports_; j++)
        currents_[j] = new CImg<double>(columns, rows, 1, 1, 0.1);
}

SingleCompartment::~SingleCompartment() {
    for (int i = 0; i < number_conductance_ports_; i++) delete conductances_[i];
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
    c_begin = clock();
    b_begin = boost::chrono::system_clock::now();
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    int icurr = 0;
    int icond = 0;

    for (unsigned int k = 0; k < source_ports.size(); k++) {
        SynapseType st = source_ports[k].getSynapseType();
        switch (st) {
            case CURRENT:
                for (int i = 0; i < (int)currents_[icurr]->size(); i++) {
                    currents_[icurr]->_data[i] =
                        source_ports[k].getData()->_data[i];
                }
                icurr++;
                break;
            case CURRENT_INHIB:
                for (int i = 0; i < (int)currents_[icurr]->size(); i++) {
                    currents_[icurr]->_data[i] =
                        -source_ports[k].getData()->_data[i];
                }
                icurr++;
                break;
            case CONDUCTANCE:
                for (int i = 0; i < (int)conductances_[icond]->size(); i++) {
                    conductances_[icond]->_data[i] =
                        source_ports[k].getData()->_data[i];
                }
                icond++;
                break;
            case CONDUCTANCE_INHIB:
                for (int i = 0; i < (int)conductances_[icond]->size(); i++) {
                    conductances_[icond]->_data[i] =
                        -source_ports[k].getData()->_data[i];
                }
                icond++;
                break;
        }
    }

    (*last_potential_) = (*current_potential_);
    *current_potential_ = 0.0;

    // When there are conductance ports
    if (number_conductance_ports_ > 0) {
        for (int i = 0; i < (int)total_cond_->size(); i++) {
            total_cond_->_data[i] = DBL_EPSILON;
        }
        for (int k = 0; k < number_conductance_ports_; k++) {
            for (int i = 0; i < (int)total_cond_->size(); i++) {
                total_cond_->_data[i] += conductances_[k]->_data[i];
            }
        }
        for (int i = 0; i < (int)tau_->size(); i++) {
            tau_->_data[i] = Cm_ / total_cond_->_data[i];
        }
        for (int i = 0; i < (int)potential_inf_->size(); i++) {
            potential_inf_->_data[i] = 0.0;
        }

        for (int k = 0; k < number_conductance_ports_; k++) {
            for (int i = 0; i < (int)potential_inf_->size(); i++) {
                potential_inf_->_data[i] += conductances_[k]->_data[i] * E_[k];
            }
        }
        for (int k = 0; k < number_current_ports_; k++) {
            for (int i = 0; i < (int)potential_inf_->size(); i++) {
                potential_inf_->_data[i] += currents_[k]->_data[i];
            }
        }

        for (int i = 0; i < (int)potential_inf_->size(); i++) {
            potential_inf_->_data[i] /= total_cond_->_data[i];
        }
        // in case total_cond = 0
        //        *total_cond_ = 0;
        //        for (int k = 0; k < number_conductance_ports_; k++)
        //            (*total_cond_) += *(conductances_[k]);

        //        (*total_cond_) += DBL_EPSILON;

        //        // tau
        //        tau_->fill(Cm_);
        //        tau_->div((*total_cond_));

        //        // potential at infinity
        //        *potential_inf_ = (*(conductances_[0])) * E_[0];

        //        for (int k = 1; k < number_conductance_ports_; k++)
        //            *potential_inf_ += (*(conductances_[k])) * E_[k];
        //        for (int k = 0; k < number_current_ports_; k++)
        //            *potential_inf_ += *(currents_[k]);

        //        potential_inf_->div(*total_cond_);
        // When there are only current ports
    } else {
        for (int i = 0; i < (int)tau_->size(); i++) {
            tau_->_data[i] = taum_;
        }

        for (int i = 0; i < (int)potential_inf_->size(); i++) {
            potential_inf_->_data[i] = El_;
        }
        for (int k = 0; k < number_current_ports_; k++) {
            for (int i = 0; i < (int)potential_inf_->size(); i++) {
                potential_inf_->_data[i] /= currents_[k]->_data[i] * Rm_;
            }
        }
        //        // tau
        //        tau_->fill(taum_);
        //        // potential at infinity
        //        potential_inf_->fill(El_);
        //        for (int k = 0; k < number_current_ports_; k++)
        //            *potential_inf_ += (*(currents_[k])) * Rm_;
    }

    // exponential term
    // TODO: there's a substantial bottleneck here due to the computation of
    // std::exp
    for (int i = 0; i < (int)exp_term_->size(); i++) {
        exp_term_->_data[i] =
            std::exp(-(config->simulationStep) / tau_->_data[i]);
    }

    //    exp_term_->fill(-config->simulationStep);
    //    exp_term_->div(*tau_);
    //    exp_term_->exp();

    // membrane potential update

    *current_potential_ += *potential_inf_;
    //    //    assert(current_potential_->operator()(0, 0, 0, 0) !=
    //    //           std::numeric_limits<double>::infinity());
    //    //    assert(current_potential_->operator()(0, 0, 0, 0) !=
    //    //           -std::numeric_limits<double>::infinity());
    *current_potential_ -= (*potential_inf_).mul(*exp_term_);
    //    //    assert(current_potential_->operator()(0, 0, 0, 0) !=
    //    //           std::numeric_limits<double>::infinity());
    //    //    assert(current_potential_->operator()(0, 0, 0, 0) !=
    //    //           -std::numeric_limits<double>::infinity());
    *current_potential_ += (*last_potential_).mul(*exp_term_);

    //    //    //    assert(current_potential_->operator()(0, 0, 0, 0) !=
    //    //    //           std::numeric_limits<double>::infinity());
    //    //    //    assert(current_potential_->operator()(0, 0, 0, 0) !=
    //    //    //           -std::numeric_limits<double>::infinity());

    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *SingleCompartment::getOutput() const {
    return current_potential_;
}

void SingleCompartment::tweak(std::vector<double> new_E) {
    assert(new_E.size() == this->E_.size());
    this->E_ = new_E;
}
