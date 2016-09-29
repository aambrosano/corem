#ifndef INTERFACE_NEST_WRAPPER_H
#define INTERFACE_NEST_WRAPPER_H

#include <COREM/core/retina.h>

#include <boost/python.hpp>
#include <CImg.h>

namespace py = boost::python;

class RetinaWrapper {
public:
    RetinaWrapper(std::string config_path);
    ~RetinaWrapper();

    void update(const py::object& img);
    double getValue(int row, int col, std::string layer);

private:
    Retina *retina_;
    CImg<double> inputImg;
    int rows, cols;

    // main conversion function, dispatches to the ones below
    // at the end, the converted image can be found in inputImg
    void convertImage(const py::object& img);
    void convertndarray(const py::object& img);
};

#endif
