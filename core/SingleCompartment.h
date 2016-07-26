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
#include <chrono>
#endif

#include <iostream>
#include "vector"

#include "module.h"
#include "constants.h"

using namespace cimg_library;
using namespace std;

class EXPORT SingleCompartment:public module{
protected:
    // image buffers
    CImg<double>** conductances;
    CImg<double>** currents;
    int number_current_ports;
    int number_conductance_ports;
    // Nernst potentials
    vector <double> E;
    // membrane capacitance, resistance and tau
    double Cm, Rm, taum, El;
    // membrane potential
    CImg<double> *current_potential,*last_potential,*total_cond,*potential_inf,*tau,*exp_term;

public:
    // Constructor, copy, destructor.
    SingleCompartment(int x=1,int y=1,double temporal_step=1.0);
    SingleCompartment(const SingleCompartment& copy);
    ~SingleCompartment(void);

    // Allocate values and set protected parameters
    virtual void allocateValues();
    virtual void setX(int x){columns_=x;}
    virtual void setY(int y){rows_=y;}

    void set_Cm(double capacitance);
    void set_Rm(double resistance);
    void set_taum(double temporal_constant);
    void set_El(double Nerst_l);

    void set_E(double NernstPotential, int port);
    void set_number_current_ports(int number);
    void set_number_conductance_ports(int number);

    // New input and update of equations
    virtual void feedInput(const CImg<double> &new_input, bool isCurrent, int port);
    virtual void update();
    // set Parameters
    virtual bool setParameters(vector<double> params, vector<string> paramID);

    // Get output image (y(k))
    virtual CImg<double>* getOutput();
};

#endif // SINGLECOMPARTMENT_H
