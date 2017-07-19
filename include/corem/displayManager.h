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
    int imagesPerRow;
    vector<bool> isShown;
    vector<double> margin;

    // buffers of displays and images generated in each neural layer
    vector<DisplayWithBar *> displays;

    // last row to display
    int last_row, last_col;

   public:
    // Constructor, copy, destructor.
    DisplayManager();
    ~DisplayManager();
    void setSize(int columns, int rows);
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

    void show(std::string id);

    // Add display
    void addInputDisplay();
    void addDisplay(string ID);

    // Update displays
    void updateDisplays(const CImg<double> *input,
                        CImg<double> *photoreceptors[4],
                        std::vector<Module *> modules,
                        simulation_state_t *sim_state);

    // Set Sim step
    void setSimStep(double value);

   private:
    retina_config_t *config;
    CImgDisplay d;
};

#endif  // DISPLAYMANAGER_H
