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
#include "Retina.h"
#include "constants.h"
#include <boost/filesystem.hpp>
#include <boost/python.hpp>

using namespace cimg_library;
using namespace std;

namespace fs = boost::filesystem;
namespace py = boost::python;

CImg<double>* custom_img(int currTime, int width, int height) {
    CImg<double>* retval = new CImg<double>(width, height, 1, 3);
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

CImg<double>* impulse_image(int width, int height) {
    CImg<double>* retval = new CImg<double>(width, height, 1, 3);
    for (int i = 0; i < retval->width(); i++) {
        for (int j = 0; j < retval->height(); j++) {
            if (i == 0) {
                (*retval)(i, j, 0, 0) = 255;
                (*retval)(i, j, 0, 1) = 255;
                (*retval)(i, j, 0, 2) = 255;
            }
            else {
                (*retval)(i, j, 0, 0) = (*retval)(i, j, 0, 1) = (*retval)(i, j, 0, 2) = 0;
            }
        }
    }
    return retval;
}

// main
int main(int argc, char *argv[])
{
    Py_Initialize();

    // Assuming the executable will be generated and executed in /build
    fs::path currentDirRoot = fs::initial_path() / "..";
    cout << currentDirRoot.string() << endl;

    // delete files in results folder (if any)
    fs::path resultsDir = currentDirRoot / "results";
    fs::remove_all(resultsDir);

    // Create retina interface
    fs::path retinaPath;

    // read arguments or default script
    if (argc == 1) {
        retinaPath = currentDirRoot / constants::retinaScript;
    } else {
        retinaPath = fs::initial_path() / (string)argv[1];
    }

    Retina retina(1, 1, 1.0);
    retina.loadCircuit(retinaPath.string());
    retina.allocateValues();
    retina.displayMg.setLNFile(constants::resultID, constants::outputfactor);

    CImg<double> inputImg;
    if (argc >= 3) {
        inputImg.load(argv[2]);
    }

    for(int k = 0; k < retina.getSimDuration() / retina.getSimStep(); k += retina.getSimStep()) {
        if (argc >= 3) {
            // Using the argument image
            retina.feedInput(&inputImg);
        }
        else {
            // Assuming no input is needed, so the retina configuration has a reference
            // to an image sequence
            retina.feedInput(k);
        }
        retina.update();
    }

    Py_Finalize();

    return 0;
}
