#ifndef MODULE_H
#define MODULE_H


/* BeginDocumentation
 * Name: module
 *
 * Description: base class of retina structure. GaussFilter, LinearFilter, ShortTermPlasticity,
 * SingleCompartment and StaticNonLinearity inherit from module
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: GaussFilter, LinearFilter, ShortTermPlasticity, SingleCompartment,
 * StaticNonLinearity
 */

#include <CImg.h>
#include <vector>
#include <iostream>
#include "constants.h"

using namespace cimg_library;
using namespace std;

class EXPORT module {
protected:
    // Image size
    int columns_, rows_;
    double step;
    // module ID
    string ID;

    // input modules and arithmetic operations among them
    vector<vector<int>> sourceOperations;
    vector<vector<string>> sourceModuleIDs;
    // types of input synapses
    vector<int> typeSynapse;

public:
    // Constructor, copy, destructor.
    module(int columns=1,int rows=1,double temporal_step=1.0);

    // set protected parameters
    void setColumns(int columns);
    void setRows(int rows);
    void set_step(double temporal_step);

    // add operations or ID of input modules
    void addOperation(vector <int> ops){sourceOperations.push_back(ops);}
    void addSourceIDs(vector <string> ID){sourceModuleIDs.push_back(ID);}
    // get operations or ID of input modules
    vector <int> getOperation(int op){return sourceOperations[op];}
    vector <string> getID(int ID){return sourceModuleIDs[ID];}
    // get size of vectors
    int getSizeID() { return sourceModuleIDs.size(); }
    int getSizeArith() { return sourceOperations.size(); }

    // Set and get the name of the module
    void setModuleID(string s){ID=s;}
    string getModuleID(){return ID;}

    // Set and get synapse type
    void addTypeSynapse(int type){typeSynapse.push_back(type);}
    int getTypeSynapse(int port){return typeSynapse[port];}

    bool checkID(string name) { return ID == name; }

    // virtual functions //
    // Allocate values
    virtual void allocateValues() {}
    virtual void setX(int) {}
    virtual void setY(int) {}
    // New input and update of equations
    virtual void feedInput(const CImg<double>&, bool, int) {}
    virtual void update() {}
    // Get output image (y(k))
    virtual CImg<double>* getOutput() { return nullptr; }
    // set Parameters
    virtual bool setParameters(vector<double>, vector<string>) { return true; }
    virtual void clearParameters(vector<string>) {}
};

#endif // MODULE_H
