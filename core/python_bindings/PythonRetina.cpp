#include "PythonRetina.h"
#include "../module.h"

void PythonRetina::TempStep(double step) {
    retina_->set_step(step);
};

void PythonRetina::SimTime(int sim_time) {
    retina_->setSimTime(sim_time);
};

void PythonRetina::NumTrials(double ntrials) {
    if (ntrials > 0) {
        retina_->setSimTotalRep(ntrials);
    }
    else {
        std::cout << "Error: ntrials <= 0" << std::endl;
        return;
    }
};

void PythonRetina::PixelsPerDegree(double ppd) {
    if (ppd > 0.0) {
        retina_->setPixelsPerDegree(ppd);
    }
    else {
        std::cout << "Error: ppd <= 0" << std::endl;
        return;
    }
};

void PythonRetina::NRepetitions(double repetitions) {
    if (repetitions > 0) {
        retina_->setRepetitions(repetitions);
    }
    else {
        std::cout << "Error: repetitions <= 0" << std::endl;
        return;
    }
};

void PythonRetina::DisplayDelay(int delay) {
    if (delay >= 0) {
        display_manager_->setDelay(delay);
    }
    else {
        std::cout << "Error: delay < 0" << std::endl;
        return;
    }
};

void PythonRetina::DisplayZoom(double zoom) {
    if (zoom >= 0) {
        display_manager_->setZoom(zoom);
    }
    else {
        std::cout << "Error: zoom < 0" << std::endl;
        return;
    }
};

void PythonRetina::DisplayWindows(int windows_per_row) {
    if (windows_per_row > 0) {
        display_manager_->setImagesPerRow(windows_per_row);
    }
    else {
        std::cout << "Error: windows_per_row <= 0" << std::endl;
        return;
    }
};

void PythonRetina::Input(std::string input_type, py::dict args) {
    if (input_type == "sequence") {
        // TODO: check return value
        retina_->setInputSeq(py::extract<std::string>(args["path"]));
    }
    else if (input_type == "grating") {
        // TODO: check return value
        retina_->generateGrating(
            py::extract<float>(args["type"]),
            py::extract<float>(args["step"]),
            py::extract<float>(args["length1"]),
            py::extract<float>(args["length2"]),
            py::extract<float>(args["length3"]),
            py::extract<float>(args["sizeX"]),
            py::extract<float>(args["sizeY"]),
            py::extract<float>(args["freq"]),
            py::extract<float>(args["period"]),
            py::extract<float>(args["Lum"]),
            py::extract<float>(args["Contr"]),
            py::extract<float>(args["phi_s"]),
            py::extract<float>(args["phi_t"]),
            py::extract<float>(args["orientation"]),
            py::extract<float>(args["red_weight"]),
            py::extract<float>(args["green_weight"]),
            py::extract<float>(args["blue_weight"]),
            py::extract<float>(args["red_phase"]),
            py::extract<float>(args["green_phase"]),
            py::extract<float>(args["blue_phase"])
        );
        retina_->setRepetitions(1.0);
    }
    else if (input_type == "fixationalMovGrating") {
        // TODO: check return value
        retina_->generateFixationalMovGrating(
            py::extract<float>(args["sizeX"]),
            py::extract<float>(args["sizeY"]),
            py::extract<float>(args["circle_radius"]),
            py::extract<float>(args["jitter_period"]),
            py::extract<float>(args["spatial_period"]),
            py::extract<float>(args["step_size"]),
            py::extract<float>(args["Lum"]),
            py::extract<float>(args["Contr"]),
            py::extract<float>(args["orientation"]),
            py::extract<float>(args["red_weight"]),
            py::extract<float>(args["green_weight"]),
            py::extract<float>(args["blue_weight"]),
            py::extract<float>(args["type1"]),
            py::extract<float>(args["type2"]),
            py::extract<float>(args["switch"])
        );
        retina_->setRepetitions(1.0);
    }
    else if (input_type == "whiteNoise") {
        // TODO: check return value
        retina_->generateWhiteNoise(
            py::extract<float>(args["mean"]),
            py::extract<float>(args["contrast1"]),
            py::extract<float>(args["contrast2"]),
            py::extract<float>(args["period"]),
            py::extract<float>(args["switch"]),
            py::extract<float>(args["sizeX"]),
            py::extract<float>(args["sizeY"])
        );
        retina_->setRepetitions(1.0);
    }
    else if (input_type == "impulse") {
        // TODO: check return value
        retina_->generateImpulse(
            py::extract<float>(args["start"]),
            py::extract<float>(args["stop"]),
            py::extract<float>(args["amplitude"]),
            py::extract<float>(args["offset"]),
            py::extract<float>(args["sizeX"]),
            py::extract<float>(args["sizeY"])
        );
        retina_->setRepetitions(1.0);
    }
    else if (input_type == "camera") {
        py::tuple size = py::extract<py::tuple>(args["size"]);
        retina_->setSizeX(py::extract<int>(size[1]));
        retina_->setSizeY(py::extract<int>(size[0]));
    }
    else {
        std::cout << "Error: invalid input type" << std::endl;
    }
}

void PythonRetina::Create(std::string module_type, std::string module_id, py::dict args) {
    module* newModule;
    if (module_type == "LinearFilter") {
        newModule = new LinearFilter(retina_->getSizeX(), retina_->getSizeY(), retina_->getStep(), 0.0);
    }
    else if (module_type == "SingleCompartment") {
        newModule = new SingleCompartment(retina_->getSizeX(), retina_->getSizeY(), retina_->getStep());
    }
    else if (module_type == "StaticNonLinearity") {
        newModule = new StaticNonLinearity(retina_->getSizeX(), retina_->getSizeY(), retina_->getStep(), 0);
    }
    else if (module_type == "CustomNonLinearity") {
        newModule = new StaticNonLinearity(retina_->getSizeX(), retina_->getSizeY(), retina_->getStep(), 1);
    }
    else if (module_type == "SigmoidNonLinearity") {
        newModule = new StaticNonLinearity(retina_->getSizeX(), retina_->getSizeY(), retina_->getStep(), 2);
    }
    else if (module_type == "ShortTermPlasticity") {
        newModule = new ShortTermPlasticity(retina_->getSizeX(), retina_->getSizeY(), retina_->getStep(), 0.0, 0.0, 0.0, 0.0, false);
    }
    else if (module_type == "GaussFilter") {
        newModule = new GaussFilter(retina_->getSizeX(), retina_->getSizeY(), retina_->getPixelsPerDegree());
    }
    else {
        std::cout << "Error: invalid module type." << std::endl;
        return;
    }

    /* TODO: this is really ugly, but that's how modules are implemented.
     * Change the parameter mechanism. */
    vector<double> p;
    vector<string> pid;

    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    for (uint i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        if (k == "type" || k == "spaceVariantSigma") {
            p.push_back(0.0);
            pid.push_back(py::extract<std::string>(args[k].attr("__str__")()));
        }
        else {
            p.push_back(py::extract<double>(args[k]));
            pid.push_back(k);
        }
    }

    newModule->setParameters(p,pid);
    retina_->addModule(newModule, module_id);
}

void PythonRetina::Connect(py::object arg0, std::string connect_to, std::string type_synapse) {
    vector <int> operations;
    vector <string> modulesID;

    display_manager_->allocateValues(retina_->getNumberModules(), retina_->getStep());
    std::string arg0_type = py::extract<std::string>(arg0.attr("__class__").attr("__name__"));

    if (arg0_type == "str") {
        modulesID.push_back(py::extract<std::string>(arg0));
    }
    else if (arg0_type == "list") {
        py::object l = arg0;
        unsigned long size = py::extract<unsigned long>(l.attr("__len__")());
        for (uint i = 0; i < size; i++) {
            std::string entry = py::extract<std::string>(l[i]);
            if (i % 2) {
                if (entry == "+") operations.push_back(0);
                else if (entry == "-") operations.push_back(1);
                else {
                    std::cout << "Error: invalid operation." << std::endl;
                    return;
                }
            }
            else {
                modulesID.push_back(entry);
            }
        }
    }
    else {
        std::cout << "Error: invalid type for arg0" << std::endl;
        return;
    }

    // TODO: check return value
    retina_->connect(modulesID, connect_to.c_str(), operations, type_synapse.c_str());
}

void PythonRetina::Show(std::string module_id, bool show, py::dict args) {
    for(int l = 1; l < retina_->getNumberModules(); l++){
        module *m = retina_->getModule(l);
        string ID = m->getModuleID();

        if (ID == module_id) {
            if (show) {
                display_manager_->setIsShown(true, l);
                display_manager_->setIsShown(true, 0);
            }
            else display_manager_->setIsShown(false, l);
        }

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            display_manager_->setMargin(margin, l);
        }
    }

    if (module_id == "Input") {
        if (show)
            display_manager_->setIsShown(true, 0);
        else
            display_manager_->setIsShown(false, 0);

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            display_manager_->setMargin(margin, 0);
        }
    }
}

void PythonRetina::Multimeter(std::string multimeter_type, std::string multimeter_id, std::string module_id, py::dict args, bool show) {
    std::string showstr;
    if (show) showstr = "True";
    else showstr = "False";

    if (multimeter_type == "spatial") {
        int param1 = py::extract<int>(args["value"]);
        bool rowcol = py::extract<bool>(args["rowcol"]);

        if (!rowcol) param1 = -param1;

        display_manager_->addMultimeterTempSpat(
            multimeter_id, module_id, param1, py::extract<int>(args["timeStep"]), false, showstr
        );
    }
    else if (multimeter_type == "temporal") {
        display_manager_->addMultimeterTempSpat(
            multimeter_id, module_id, py::extract<int>(args["x"]), py::extract<int>(args["y"]), true, showstr
        );
    }
    else if (multimeter_type == "Linear-Nonlinear") {
        display_manager_->addMultimeterLN(
            multimeter_id, module_id, py::extract<int>(args["x"]), py::extract<int>(args["y"]),
            py::extract<double>(args["segment"]), py::extract<double>(args["interval"]),
            py::extract<double>(args["start"]), py::extract<double>(args["stop"]), showstr
        );
    }
    else {
        std::cout << "Error: invalid multimeter type." << std::endl;
        return;
    }
}

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
            inputImg(i, j, 0, 0) = buf[3*(i+j*cols)+2];
            inputImg(i, j, 0, 1) = buf[3*(i+j*cols)+1];
            inputImg(i, j, 0, 2) = buf[3*(i+j*cols)];
        }
    }

}

InterfaceNESTWrapper::InterfaceNESTWrapper(string config_path) {
    iface_ = new InterfaceNEST();
    iface_->allocateValues(config_path.c_str(), constants::resultID, constants::outputfactor, 0);
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
    // delete input;
    // delete actual_img;
}

double InterfaceNESTWrapper::getValue(int row, int col) {
    return iface_->getValue(row*iface_->sizeY+col);
}

BOOST_PYTHON_MODULE(pyretina)
{
    py::class_<PythonRetina>("Retina", py::init<Retina&, DisplayManager&>())
        .def("TempStep", &PythonRetina::TempStep)
        .def("SimTime", &PythonRetina::SimTime)
        .def("NumTrials", &PythonRetina::NumTrials)
        .def("PixelsPerDegree", &PythonRetina::PixelsPerDegree)
        .def("NRepetitions", &PythonRetina::NRepetitions)
        .def("DisplayDelay", &PythonRetina::DisplayDelay)
        .def("DisplayZoom", &PythonRetina::DisplayZoom)
        .def("DisplayWindows", &PythonRetina::DisplayWindows)
        .def("Input", &PythonRetina::Input)
        .def("Create", &PythonRetina::Create)
        .def("Connect", &PythonRetina::Connect)
        .def("Show", &PythonRetina::Show)
        .def("Multimeter", &PythonRetina::Multimeter);
    py::class_<InterfaceNESTWrapper>("InterfaceNEST", py::init<std::string>())
        .def("update", &InterfaceNESTWrapper::update)
        .def("getValue", &InterfaceNESTWrapper::getValue);
}
