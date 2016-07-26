#include "InterfaceNESTWrapper.h"

void InterfaceNESTWrapper::convertImage(const boost::python::api::object& img) {

    std::string objtype = py::extract<std::string>(img.attr("__class__").attr("__module__"));
    objtype += ".";
    objtype += py::extract<std::string>(img.attr("__class__").attr("__name__"));

    if (objtype == "numpy.ndarray")
        convertndarray(img);
    else
        throw std::invalid_argument("Convertion from " + objtype + " is not supported.");

}

void InterfaceNESTWrapper::convertndarray(const boost::python::api::object& img) {

    Py_ssize_t bufLen;
    void const *buffer = NULL;
    py::object data = img.attr("data");
    bool isReadBuffer = !PyObject_AsReadBuffer(data.ptr(), &buffer, &bufLen);

    if (!isReadBuffer || bufLen != 3*cols*rows) {
        throw std::invalid_argument("Something is wrong with the image.");
    }

    unsigned char* buf = (unsigned char*)buffer;

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++){
            inputImg(i, j, 0, 0) = buf[3*(i+j*cols)];
            inputImg(i, j, 0, 1) = buf[3*(i+j*cols)+1];
            inputImg(i, j, 0, 2) = buf[3*(i+j*cols)+2];
        }
    }

}

InterfaceNESTWrapper::InterfaceNESTWrapper(string config_path) {
    iface_ = new InterfaceNEST();
    // local retina file
    fs::path retina_config_file(config_path);
    if (!fs::exists(config_path)) {
      std::cerr << "Local retina not found " << retina_config_file.string() << std::endl;
      std::cout << "Defaulting to generic retina folder." << std::endl;
      // default to retina file in models directory
      assert(getenv("NRP_MODELS_DIRECTORY") != nullptr);
      retina_config_file = (string)getenv("NRP_MODELS_DIRECTORY");
      retina_config_file /= "retina";
      retina_config_file /= config_path;
    }

    if (!fs::exists(retina_config_file.string())) {
      std::cerr << "Generic retina not found " << retina_config_file.string() << std::endl;
      throw std::runtime_error("Retina config file not found.\nLooked for:\n"+
                               config_path+"\n"+
                               retina_config_file.string());
    }

    iface_->allocateValues(retina_config_file.string().c_str(), constants::resultID,
                           constants::outputfactor, 0);
    rows = iface_->sizeX;
    cols = iface_->sizeY;
    inputImg.resize(cols, rows, 1, 3, -1);
}

void InterfaceNESTWrapper::update(const py::object& img) {
    convertImage(img);
    CImg<double>* input = iface_->retina.feedInput(&inputImg);
    iface_->retina.update();
    iface_->displayMg.updateDisplay(input,
        iface_->retina,
        iface_->SimTime,
        iface_->totalSimTime,
        iface_->CurrentTrial,
        iface_->totalNumberTrials);
    iface_->SimTime += iface_->step;
}

double InterfaceNESTWrapper::getValue(int row, int col, string layer="Output") {
    return iface_->getValue(row*iface_->sizeY+col, layer);
}
