#pragma once

#include <boost/python.hpp>
#include "../InterfaceNEST.h"

namespace py = boost::python;

class InterfaceNESTWrapper {
public:
    InterfaceNESTWrapper(std::string config_path);

    void update(const py::object& img);
    double getValue(int row, int col, string layer);

private:
    InterfaceNEST  *iface_;
    CImg<double> inputImg;
    int rows, cols;

    // main conversion function, dispatches to the ones below
    // at the end, the converted image can be found in inputImg
    void convertImage(const py::object& img);
    void convertndarray(const py::object& img);
};
