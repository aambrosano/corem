/* BeginDocumentation
 * Name: main
 *
 * Description: fixed time-step simulation of the retina script specified
 * in "constants.cpp" or passed through arguments.
 *
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso:InterfaceNEST
 */

#include "../CImg-1.6.0_rolling141127/CImg.h"
#include "InterfaceNEST.h"
#include "constants.h"
#include <boost/filesystem.hpp>
#include <boost/python.hpp>

using namespace cimg_library;
using namespace std;

namespace fs = boost::filesystem;
namespace py = boost::python;

// create a dummy image with a moving red pixel
CImg<double>* custom_img(int currTime) {
    CImg<double>* retval = new CImg<double>(25, 25, 1, 3);
    for (int i = 0; i < retval->width(); i++) {
        for (int j = 0; j < retval->height(); j++) {
            (*retval)(i, j, 0, 0) = (*retval)(i, j, 0, 1) = (*retval)(i, j, 0, 2) = 255;
        }
    }

    (*retval)(currTime % retval->width(), currTime % retval->height(), 0, 0) = 255;
    (*retval)(currTime % retval->width(), currTime % retval->height(), 0, 1) = 0;
    (*retval)(currTime % retval->width(), currTime % retval->height(), 0, 2) = 0;
    return retval;
}

// main
int main(int argc, char *argv[])
{
    #ifdef USE_PYTHON_INTERPRETER
    Py_Initialize();
    #endif
    // string currentDirRoot = constants::getPath();
    // Assuming the executable will be generated and executed in /build
    fs::path currentDirRoot = fs::initial_path() / "..";
    cout << currentDirRoot.string() << endl;

    // delete files in results folder (if any)
    fs::path resultsDir = currentDirRoot / "results";
    fs::remove_all(resultsDir);

    // Create retina interface
    fs::path retinaString;

    // read arguments or default script
    if (argc == 1) {
        retinaString = currentDirRoot / constants::retinaScript;
    } else {
        retinaString = currentDirRoot / "Retina_scripts" / (string)argv[1];
    }

    const char * retinaSim = retinaString.c_str();

    InterfaceNEST interface;
    interface.allocateValues(retinaSim, constants::resultID, constants::outputfactor, 0);

    // Read number of trials and simulation time
    double trials = interface.getTotalNumberTrials();
    int simTime = interface.getSimTime();
    double simStep = interface.getSimStep();

    cout << "Simulation time: " << simTime << endl;
    cout << "Trials: " << trials << endl;
    cout << "Simulation step: " << simStep << endl;

    // Simulation
    for (unsigned int i = 0; i < trials; i++){

        // Create new retina interface for every trial (reset values)
        InterfaceNEST interface;
        interface.allocateValues(retinaSim, constants::resultID, constants::outputfactor, i);

        cout << "-- Trial " << i << " --" << endl;

        if(interface.getAbortExecution() == false){
            for(int k = 0; k < simTime; k += simStep){
                // Uses images from the conf file
                // interface.update();

                // Custom images
                CImg<double> *a = custom_img(k);
                CImg<double> *input = interface.retina.feedInput(a);
                interface.retina.update();
                interface.displayMg.updateDisplay(input,
                    interface.retina,
                    interface.SimTime,
                    interface.totalSimTime,
                    interface.CurrentTrial,
                    interface.totalNumberTrials);
                interface.SimTime+=interface.step;
            }
        }

    }

    #ifdef USE_PYTHON_INTERPRETER
    Py_Finalize();
    #endif

    return 0;
}
