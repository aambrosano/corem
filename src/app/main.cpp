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

#include <COREM/core/retina.h>
#include <COREM/core/constants.h>

#include <CImg.h>

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

int main(int argc, char *argv[])
{
    Py_Initialize();

    boost::filesystem::path currentDirRoot(boost::filesystem::current_path());

    // delete files in results folder (if any)
    fs::path resultsDir = currentDirRoot / "results";
    fs::remove_all(resultsDir);

    // Create retina interface
    fs::path retinaPath;

    // read arguments or default script
    if (argc == 1) {
        retinaPath = currentDirRoot / constants::retinaScript;
    } else {
        retinaPath = (string)argv[1];
        if (!fs::exists(retinaPath))
            retinaPath = currentDirRoot / (string)argv[1];
        if (!fs::exists(retinaPath)) {
            std::cout << "Wrong retina file path" << std::endl;
            return EXIT_FAILURE;
        }
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
