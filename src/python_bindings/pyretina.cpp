#include <corem/retina.h>
#include <boost/python.hpp>

#define BOOST_PYTHON_STATIC_LIB

BOOST_PYTHON_MODULE(pyretina) { define_retina_python(); }
