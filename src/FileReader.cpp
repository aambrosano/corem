#include "FileReader.h"
#include "python_bindings/PythonRetina.h"

FileReader::FileReader() {
    CorrectFile = true;
    continueReading = true;
    fileName = "";
}

void FileReader::reset() {
    CorrectFile = true;
    continueReading = true;
    fileName = "";
}

//-------------------------------------------------//


void FileReader::setDir(fs::path path){
    fileName = path;
}

//-------------------------------------------------//

void FileReader::allocateValues(){
    if (fs::exists(fileName)) {
        fin.open(fileName.c_str());
    }
    else {
        cout << "Wrong retina file path." << endl;
        CorrectFile = false;
        continueReading = false;
    }
}

//-------------------------------------------------//

void FileReader::parseFile(Retina& retina, DisplayManager &displayMg){
    // interpreting the retina scripts as real python files instead of parsing them
    ifstream in(fileName.c_str());
    string contents((istreambuf_iterator<char>(in)),
        istreambuf_iterator<char>());
    try {
        auto py__main__ = py::import("__main__");
        auto pyretina = py::import("retina");
        PythonRetina* pr = new PythonRetina(retina, displayMg);
        py__main__.attr("retina") = py::ptr(pr);
        PyRun_SimpleString(contents.c_str());
    } catch(py::error_already_set const &) {
        PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
        PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
        std::string err("Unfetchable Python error");
        if(type_ptr != NULL){
            py::handle<> h_type(type_ptr);
            py::str type_pstr(h_type);
            py::extract<std::string> e_type_pstr(type_pstr);
            if(e_type_pstr.check())
                err = e_type_pstr();
            else
                err = "Unknown exception type";
        }
        std::cout << "Error in Python: " << err << std::endl;

        std::string exception_msg = py::extract<std::string>(value_ptr);
        std::cout << exception_msg << std::endl;
    }
}

//-------------------------------------------------//

void FileReader::abort(int line){
    cout << "Incorrect syntax in line " << line << endl;
    cout << "Aborting parsing of retina file." << endl;
    continueReading = false;

}
