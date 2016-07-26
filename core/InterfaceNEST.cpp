#include "InterfaceNEST.h"

InterfaceNEST::InterfaceNEST(void) : retina(1, 1, 1.0), displayMg(1, 1), abortExecution(false) {}

//------------------------------------------------------------------------------//


void InterfaceNEST::reset(){
    abortExecution = false;
    retina.reset(1, 1, 1.0);
    displayMg.reset();
}

//------------------------------------------------------------------------------//


double InterfaceNEST::getTotalNumberTrials(){
    return totalNumberTrials;
}

//------------------------------------------------------------------------------//


int InterfaceNEST::getSimTime(){
    return totalSimTime;
}

//------------------------------------------------------------------------------//



double InterfaceNEST::getSimStep(){
    return step;
}

//------------------------------------------------------------------------------//

void InterfaceNEST::allocateValues(const char *retinaPath, const char *outputFile, double outputfactor, double currentRep) {

    try {
        // Loads the the retina circuit
        retina.loadCircuit(std::string(retinaPath), displayMg);
        // Set simulation time to 0
        SimTime = 0;

        // Simulation parameters
        totalSimTime = retina.getSimTime();
        totalNumberTrials = retina.getSimTotalRep();

        CurrentTrial = currentRep;
        retina.setSimCurrentRep(currentRep);

        // retina size and step
        sizeX=retina.getSizeX();
        sizeY=retina.getSizeY();
        step=retina.getStep();

        // Display manager
        displayMg.setX(sizeX);
        displayMg.setY(sizeY);

        // LN parameters
        displayMg.setLNFile(outputFile,outputfactor);

        // Allocate retina object
        retina.allocateValues();

        // Display manager
        for(int k=1;k<retina.getNumberModules();k++){
            displayMg.addModule(k,(retina.getModule(k))->getModuleID());
        }

    } catch (std::runtime_error e) {
        abortExecution = true;
        return;
    }


}



//------------------------------------------------------------------------------//


void InterfaceNEST::update(){
    CImg<double> *input= retina.feedInput(SimTime);
    retina.update();
    displayMg.updateDisplay(input,retina,SimTime,totalSimTime,CurrentTrial,totalNumberTrials);

    SimTime+=step;
}

//------------------------------------------------------------------------------//


double InterfaceNEST::getValue(double cell, string layer = "Output") {
    int select_image = int(cell)/(sizeX*sizeY);
    int pos_im = int(cell) - select_image*(sizeX*sizeY);

    module* aux = retina.getModule(0);
    module* neuron;

    // I don't know why they crash, so I don't want to remove these lines
    // vector <string> layersID;
    // layersID = aux->getID(select_image);

    int row = pos_im/sizeY;
    int col = pos_im%sizeY;

    // See above
    // if (layer == "Output") layer = layersID[0];

    for (int k = 1; k < retina.getNumberModules(); k++) {
        if (retina.getModule(k)->getModuleID() == layer) {
            return (*retina.getModule(k)->getOutput())(col, row, 0, 0);
        }
    }

    neuron = retina.getModule(0);
    return (*neuron->getOutput())(col, row, 0, 0);
}

//------------------------------------------------------------------------------//

void InterfaceNEST::setWhiteNoise(double mean, double contrast1, double contrast2, double period, double switchT,string id,double start, double stop) {
    retina.generateWhiteNoise(mean, contrast1, contrast2, period,switchT,sizeX, sizeY);
    displayMg.modifyLN(id,start,stop);
}

void InterfaceNEST::setImpulse(double start, double stop, double amplitude,double offset) {
    retina.generateImpulse(start, stop, amplitude,offset, sizeX, sizeY);
}

void InterfaceNEST::setGrating(int type,double step,double lengthB,double length,double length2,int X,int Y,double freq,double T,double Lum,double Cont,double phi,double phi_t,double theta,double red, double green, double blue,double red_phi, double green_phi,double blue_phi) {
    retina.generateGrating(type, step, lengthB, length, length2, X, Y, freq, T, Lum, Cont, phi, phi_t, theta,red,green,blue,red_phi, green_phi,blue_phi);
}
