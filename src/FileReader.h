#ifndef FILEREADER_H
#define FILEREADER_H


/* BeginDocumentation
 * Name: FileReader
 *
 * Description: parsing of retina script and initialization of retina modules
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 */

#include "DisplayManager.h"
#include "StaticNonLinearity.h"
#include "ShortTermPlasticity.h"
#include "Retina.h"
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

using namespace cimg_library;
using namespace std;
namespace py = boost::python;
namespace fs = boost::filesystem;

// fixed parameters
const int MAX_CHARS_PER_LINE = 2048;
const int MAX_TOKENS_PER_LINE = 50;
const char* const DELIMITER1 = ".(),'";
const char* const DELIMITER2 = ",')";


class FileReader{
private:
    // to check syntax errors
    bool CorrectFile;
    bool continueReading;

    // File reader
    fs::path fileName;
    ifstream fin;

public:

    // Constructor
    FileReader();

    // reset parameters
    void reset();
    // set directory
    void setDir(fs::path path);
    // allocate values
    void allocateValues();

    // parsing of retina script and initialization of retina modules
    void parseFile(Retina &retina,DisplayManager &displayMg);
    bool getContReading(){return continueReading;}
    void abort(int line);
};

#endif // FILEREADER_H
