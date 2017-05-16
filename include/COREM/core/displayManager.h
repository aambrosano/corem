#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

/* BeginDocumentation
 * Name: DisplayManager
 *
 * Description: Displays of multimeters
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: multimeter
 */

#include "constants.h"
#include "displayWithBar.h"
#include "input.h"
#include "module.h"
#include "multimeter.h"

using namespace cimg_library;
using namespace std;

class EXPORT DisplayManager {
   protected:
    // Display parameters
    //    double displayZoom;
    //    double delay;
    int imagesPerRow;
    vector<bool> isShown;
    vector<double> margin;

    // buffers of displays and images generated in each neural layer
    vector<DisplayWithBar *> displays;
    CImg<double> **intermediateImages;

    // Multimeter buffer and parameters
    //    vector<multimeter *> multimeters;
    vector<string> multimeterIDs;
    vector<string> moduleIDs;
    vector<int> multimeterType;
    // to select either temporal or spatial multimeter
    vector<vector<int> > multimeterParam;
    // LN multimeters
    //    vector<double> LNSegment;
    //    vector<double> LNInterval;
    //    vector<double> LNStart;
    ////    vector<double> LNStop;
    //    string LNFile;
    //    double LNfactor;

    // last row to display
    int last_row, last_col;
    // color bars
    CImg<double> **bars;
    CImg<double> *templateBar;

    // values allocated
    bool valuesAllocated;

    // Sim step
    double simStep;

    //    void addMultimeter(string multimeterID, string moduleID, vector<int>
    //    &aux,
    //                       MultimeterType mtype, bool shown);

   public:
    // Constructor, copy, destructor.
    DisplayManager();
    void setSize(unsigned int columns, unsigned int rows);
    void setConfig(retina_config_t *conf);

    void reset();

    // Allocate values
    void allocateValues(int number, double tstep);

    // Set X and Y
    void setColumns(int columns);
    void setRows(int rows);

    // other Set functions
    void setImagesPerRow(int numberI);
    void setIsShown(bool value, int pos);
    void setMargin(double m, int pos);

    // Add display
    void addInputDisplay();
    void addDisplay(string ID);

    // Update displays
    void updateDisplay(Input *input, vector<Module *> &modules,
                       unsigned int switchTime, unsigned int currentTime,
                       unsigned int trial);

    // Set Sim step
    void setSimStep(double value);

   private:
    retina_config_t *config;
    CImgDisplay d;
};

#endif  // DISPLAYMANAGER_H
