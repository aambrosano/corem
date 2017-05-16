#include "COREM/core/retina.h"

Retina::Retina() {
    this->config = new retina_config_t;
    this->simState = new simulation_state_t;
}

// Retina::Retina(const Retina &copy) {
//    throw std::runtime_error("This shouldn't be called");
//}

void Retina::initialize() {
    std::cout << "Warning: make sure you instantiated correctly all the "
                 "simulation parameters "
              << "before calling initialize." << std::endl;

    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;

    this->displayMg.setConfig(this->config);

    this->multimeters.clear();
    this->modules.clear();

    this->modules.push_back(new Parrot(std::string("Output"), this->config));

    this->output = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->accumulator = *(new CImg<double>(cols, rows, 1, 1, 0.0));

    this->RGBred = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->RGBgreen = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->RGBblue = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->ch1 = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->ch2 = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->ch3 = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->rods = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->X_mat = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->Y_mat = *(new CImg<double>(cols, rows, 1, 1, 0.0));
    this->Z_mat = *(new CImg<double>(cols, rows, 1, 1, 0.0));

    simState->currentTime = 0;
}

retina_config_t *Retina::getConfig() { return config; }

CImg<double> *rgb2xyz(CImg<double> *input) {
    CImg<double> *res =
        new CImg<double>(input->width(), input->height(), 1, 3, 0.0);
    cimg_forXY(*input, x, y) {
        (*res)(x, y, 0, 0) = 0.4124564 * (*input)(x, y, 0, 0) +
                             0.3575761 * (*input)(x, y, 0, 1) +
                             0.1804375 * (*input)(x, y, 0, 2),
                        (*res)(x, y, 0, 1) = 0.2126729 * (*input)(x, y, 0, 0) +
                                             0.7151522 * (*input)(x, y, 0, 1) +
                                             0.0721750 * (*input)(x, y, 0, 2),
                        (*res)(x, y, 0, 2) = 0.0193339 * (*input)(x, y, 0, 0) +
                                             0.1191920 * (*input)(x, y, 0, 1) +
                                             0.9503041 * (*input)(x, y, 0, 2);
    }
    return res;
}

CImg<double> *xyz2lms(CImg<double> *input) {
    CImg<double> *res =
        new CImg<double>(input->width(), input->height(), 1, 3, 0.0);
    cimg_forXY(*input, x, y) {
        (*res)(x, y, 0, 0) = 0.38971 * (*input)(x, y, 0, 0) +
                             0.68898 * (*input)(x, y, 0, 1) -
                             0.07868 * (*input)(x, y, 0, 2),
                        (*res)(x, y, 0, 1) = -0.22981 * (*input)(x, y, 0, 0) +
                                             1.1834 * (*input)(x, y, 0, 1) +
                                             0.04641 * (*input)(x, y, 0, 2),
                        (*res)(x, y, 0, 2) = (*input)(x, y, 0, 2);
    }
    return res;
}

CImg<double> *greyscale2rgb(CImg<double> *input) {
    CImg<double> *res =
        new CImg<double>(input->width(), input->height(), 1, 3, 0.0);
    cimg_forXY(*input, x, y) {
        (*res)(x, y, 0, 0) = (*input)(x, y, 0, 0),
                        (*res)(x, y, 0, 1) = (*input)(x, y, 0, 0),
                        (*res)(x, y, 0, 2) = (*input)(x, y, 0, 0);
    }
    return res;
}

CImg<double> *Retina::step(CImg<double> *input) {
    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;

    assert((unsigned int)input->width() == cols);
    assert((unsigned int)input->height() == rows);

    this->input.assign(*input);  // Copies...

    if ((unsigned int)(input->size()) ==
        static_cast<unsigned int>(cols * rows)) {
        // Separate color channels
        cimg_forXY(*input, x, y) {
            RGBred(x, y, 0, 0) =
                (*input)(x, y, 0, 0),  // Red component of image sent to imgR
                RGBgreen(x, y, 0, 0) = (*input)(
                    x, y, 0, 0),  // Green component of image sent to imgG
                RGBblue(x, y, 0, 0) = (*input)(
                    x, y, 0, 0);  // Blue component of image sent to imgB
        }
    } else {
        // Separate color channels
        cimg_forXY(*input, x, y) {
            RGBred(x, y, 0, 0) =
                (*input)(x, y, 0, 0),  // Red component of image sent to imgR
                RGBgreen(x, y, 0, 0) = (*input)(
                    x, y, 0, 1),  // Green component of image sent to imgG
                RGBblue(x, y, 0, 0) = (*input)(
                    x, y, 0, 2);  // Blue component of image sent to imgB
        }
    }

    // Hunt-Pointer-Estévez (HPE) transform
    // sRGB --> XYZ
    X_mat = 0.4124564 * RGBblue + 0.3575761 * RGBgreen + 0.1804375 * RGBred;
    Y_mat = 0.2126729 * RGBblue + 0.7151522 * RGBgreen + 0.0721750 * RGBred;
    Z_mat = 0.0193339 * RGBblue + 0.1191920 * RGBgreen + 0.9503041 * RGBred;

    // XYZ --> LMS
    ch1 = 0.38971 * X_mat + 0.68898 * Y_mat - 0.07868 * Z_mat;
    ch2 = -0.22981 * X_mat + 1.1834 * Y_mat + 0.04641 * Z_mat;
    ch3 = Z_mat;

    rods = (ch1 + ch2 + ch3) / 3;

    // CImg<double> rods = *(xyz2lms(rgb2xyz(input)));

    std::vector<Module *>::iterator module;

    for (module = modules.begin(); module != modules.end(); ++module) {
        // port search
        for (unsigned int o = 0; o < (*module)->source_ports.size(); o++) {
            ModulePort *mp = &((*module)->source_ports[o]);
            std::vector<std::string> l = mp->getSources();
            std::vector<Operation> p = mp->getOperations();

            accumulator = CImg<double>(cols, rows, 1, 1, 0);

            // image input
            if (l[0] == "L_cones") {
                // acc umulator=rods.get_channel(2);
                accumulator = ch3;
            } else if (l[0] == "M_cones") {
                // accumulator=rods.get_channel(1);//ch2;
                accumulator = ch2;
            } else if (l[0] == "S_cones") {
                // accumulator=rods.get_channel(0);//ch1;
                accumulator = ch1;
            } else if (l[0] == "rods") {
                // accumulator=rods/3; // TODO: This is wrong. Should be the sum
                // of
                // channels / 3 (single channel image).
                accumulator = rods;
            } else {
                // other inputs rather than cones or rods
                std::vector<Module *>::iterator module_in = modules.begin();
                for (; module_in != modules.end(); ++module_in) {
                    if (l[0] == (*module_in)->id()) {
                        accumulator = *((*module_in)->getOutput());
                        break;
                    }
                }

                // other operations
                for (unsigned int k = 1; k < l.size(); k++) {
                    for (unsigned int m = 0; m < modules.size(); m++) {
                        Module *n = modules[m];
                        if (l[k] == n->id()) {
                            if (p[k - 1] == ADD)
                                accumulator += *(n->getOutput());
                            else if (p[k - 1] == SUB)
                                accumulator -= *(n->getOutput());
                            else
                                std::cerr << "Retina::step(): wrong operation."
                                          << std::endl;
                            break;
                        }
                    }
                }
            }

            mp->update(accumulator);
        }
    }

    for (unsigned int i = 0; i < modules.size(); i++) {
        this->modules[i]->update();
    }

    for (unsigned int i = 0; i < multimeters.size(); i++) {
        this->multimeters[i]->record();
        if (simState->currentTime == this->multimeters[i]->showTime()) {
            this->multimeters[i]->showGP(0);
            this->multimeters[i]->dump();
        }
    }

    double switchTime = 0;

    if (inputModule->input_type == "WhiteNoise") {
        switchTime = ((WhiteNoise *)inputModule)->getSwitchTime();
    }

    this->displayMg.updateDisplay(this->inputModule, this->modules, switchTime,
                                  simState->currentTime,
                                  simState->currentTrial);
    simState->currentTime += config->simulationStep;

    return input;
}

CImg<double> *Retina::step() {
    CImg<double> *input;
    input = inputModule->getData(simState->currentTime);
    input = this->step(input);
    return input;
}

void Retina::run() {
    while (simState->currentTrial < config->numTrials) {
        while (simState->currentTime <= config->simulationTime) {
            this->step();
            // usleep(200000);
        }
        std::cout << simState->currentTrial << std::endl;
        simState->currentTime = 0;
        simState->currentTrial++;
    }
}

void Retina::addModule(Module *m) {
    modules.push_back(m);
    if (verbose) {
        std::cout << "Module " << m->id() << " added to the retina structure"
                  << std::endl;
    }
}

Module *Retina::getModule(int ID) { return modules[ID]; }

int Retina::getNumberModules() { return modules.size(); }

void Retina::connect(std::vector<std::string> from, std::string to,
                     std::vector<Operation> operations,
                     SynapseType type_synapse) {
    std::cout << "Warning: make sure you instantiated all the modules you want "
                 "to use."
              << std::endl;

    Module *neuronto = NULL;

    // Search for the destination module
    // Looking for the destination module
    bool found = false;
    for (unsigned int i = 0; i < modules.size() && !found; i++) {
        if (modules[i]->id() == to) {
            neuronto = modules[i];
            found = true;
        }
    }

    if (!found) {
        error("Retina::connect(): Destination module " + to + " not found.");
    }

    // Iterate through source modules and look if all of them are existing
    for (std::vector<std::string>::iterator ff = from.begin(); ff != from.end();
         ++ff) {
        // These modules are embedded
        if (*ff == "rods" || *ff == "L_cones" || *ff == "M_cones" ||
            *ff == "S_cones") {
            continue;
        }

        bool found = false;
        for (unsigned int k = 0; k < modules.size() && !found; k++) {
            if (modules[k]->id() == *ff) {
                found = true;
                if (verbose)
                    cout << modules[k]->id() << " is being conected to "
                         << neuronto->id() << endl;
            }
        }

        if (!found) {
            error("Retina::connect(): Source module " + to + " not found.");
        }
    }

    neuronto->source_ports.push_back(
        ModulePort(from, operations, type_synapse));
}

void Retina::loadCircuit(std::string config_path) {
    Py_Initialize();

    define_retina_python();

    if (!fs::exists(config_path)) {
        error("Retina config file " + config_path + " not found.");
    }

    // interpreting the retina scripts as real python files instead of parsing
    // them
    ifstream in(config_path.c_str());
    string contents((istreambuf_iterator<char>(in)),
                    istreambuf_iterator<char>());
    try {
        py::object py__main__ = py::import("__main__");
        py__main__.attr("retina") = py::ptr(this);
        py::exec(contents.c_str(), py__main__.attr("__dict__"));
        Py_Finalize();
    } catch (py::error_already_set const &) {
        PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
        PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
        std::string ret("Unfetchable Python error");
        if (type_ptr != NULL) {
            py::handle<> h_type(type_ptr);
            py::str type_pstr(h_type);
            py::extract<std::string> e_type_pstr(type_pstr);
            if (e_type_pstr.check())
                ret = e_type_pstr();
            else
                ret = "Unknown exception type";
        }
        std::cout << "Error in Python: " << ret << std::endl;

        std::string exception_msg = py::extract<std::string>(value_ptr);
        std::cout << exception_msg << std::endl;
        Py_Finalize();
        error("The retina configuration couldn't be loaded");
    }
}

double Retina::getValue(int cell, std::string layer) {
    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;
    int select_image = cell / (cols * rows);
    int pos_im = cell - select_image * (cols * rows);

    int row = pos_im / cols;
    int col = pos_im % cols;

    for (std::vector<Module *>::iterator m = modules.begin();
         m != modules.end(); ++m) {
        if ((*m)->id() == layer) {
            return (*((*m)->getOutput()))(col, row, 0, 0);
        }
    }

    error("Invalid layer");
}

void define_retina_python() {
    py::class_<Retina>("Retina", py::init<>())
        .def("TempStep", &Retina::Py_TempStep)
        .def("SimTime", &Retina::Py_SimTime)
        .def("NumTrials", &Retina::Py_NumTrials)
        .def("PixelsPerDegree", &Retina::Py_PixelsPerDegree)
        .def("DisplayDelay", &Retina::Py_DisplayDelay)
        .def("DisplayZoom", &Retina::Py_DisplayZoom)
        .def("DisplayWindows", &Retina::Py_DisplayWindows)
        .def("Input", &Retina::Py_Input)
        .def("Create", &Retina::Py_Create)
        .def("Connect", &Retina::Py_Connect)
        .def("Show", &Retina::Py_Show)
        .def("Multimeter", &Retina::Py_Multimeter)
        .def("Step", &Retina::Py_Step)
        .def("GetValue", &Retina::Py_GetValue);
}
