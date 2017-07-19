#include <corem/retina.h>

Retina::Retina() {
    this->config = new retina_config_t;
    this->simState = new simulation_state_t;
    this->displayMg = new DisplayManager;
    this->inputModule = NULL;
    this->inputModule = NULL;
    this->output = NULL;
    this->accumulator = NULL;
    this->RGBred = NULL;
    this->RGBblue = NULL;
    this->RGBgreen = NULL;
    this->L_cones = NULL;
    this->M_cones = NULL;
    this->S_cones = NULL;
    this->rods = NULL;

    this->t_modules = 0;
    this->t_module_ports = 0;
    this->t_multimeters = 0;
    this->t_RGB_to_LMS = 0;
    this->t_update_display = 0;
}

Retina::~Retina() {
    std::cout << "Retina destructor called" << std::endl;

    // Removing modules
    std::vector<Module *>::iterator module;
    for (module = modules.begin(); module != modules.end(); ++module) {
        if (*module != NULL) delete *module;
    }

    // Removing multimeters
    std::vector<Multimeter *>::iterator multimeter;
    for (multimeter = multimeters.begin(); multimeter != multimeters.end();
         ++multimeter) {
        if (*multimeter != NULL) delete *multimeter;
    }

    if (this->inputModule != NULL) delete this->inputModule;
    if (this->displayMg != NULL) delete this->displayMg;
    if (this->output != NULL) delete this->output;
    if (this->accumulator != NULL) delete this->accumulator;
    if (this->RGBred != NULL) delete this->RGBred;
    if (this->RGBblue != NULL) delete this->RGBblue;
    if (this->RGBgreen != NULL) delete this->RGBgreen;
    if (this->L_cones != NULL) delete this->L_cones;
    if (this->M_cones != NULL) delete this->M_cones;
    if (this->S_cones != NULL) delete this->S_cones;
    if (this->rods != NULL) delete this->rods;
    if (this->config != NULL) delete this->config;
    if (this->simState != NULL) delete this->simState;
}

void Retina::initialize() {
    std::cout << "Warning: make sure you instantiated correctly all the "
                 "simulation parameters "
              << "before calling initialize." << std::endl;

    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;

    this->displayMg->setConfig(this->config);

    this->multimeters.clear();
    this->modules.clear();

    this->modules.push_back(
        new Parrot(std::string("Output"), this->config, 1.0));

    this->output = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->accumulator = new CImg<double>(cols, rows, 1, 1, 0.0);

    this->RGBred = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->RGBgreen = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->RGBblue = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->L_cones = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->M_cones = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->S_cones = new CImg<double>(cols, rows, 1, 1, 0.0);
    this->rods = new CImg<double>(cols, rows, 1, 1, 0.0);

    simState->currentTime = 0;
}

retina_config_t *Retina::getConfig() { return config; }

const CImg<double> *Retina::step(const CImg<double> *input) {
    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;

    assert((unsigned int)input->width() == cols);
    assert((unsigned int)input->height() == rows);

    this->input = input;

    //    std::cout << 1 << std::endl;

    if ((unsigned int)(input->size()) ==
        static_cast<unsigned int>(cols * rows)) {
        assert(false);
        // Separate color channels
        *RGBred = *input;    // Red component of image sent to imgR
        *RGBgreen = *input;  // Green component of image sent to imgG
        *RGBblue = *input;   // Blue component of image sent to imgB
    } else {
        // Separate color channels
        cimg_forXY(*input, x, y) {
            (*RGBred)(x, y, 0, 0) =
                (*input)(x, y, 0, 0),  // Red component of image sent to imgR
                (*RGBgreen)(x, y, 0, 0) = (*input)(
                    x, y, 0, 1),  // Green component of image sent to imgG
                (*RGBblue)(x, y, 0, 0) = (*input)(
                    x, y, 0, 2);  // Blue component of image sent to imgB
        }
    }
    clock_t begin = clock();
    //    std::cout << 2 << std::endl;
    // Hunt-Pointer-Estévez (HPE) transform

    // sRGB --> XYZ
    // X = 0.4124564 * R + 0.3575761 * G + 0.1804375 * B
    // Y = 0.2126729 * R + 0.7151522 * G + 0.0721750 * B
    // Z = 0.0193339 * R + 0.1191920 * G + 0.9503041 * B

    // {
    //  {0.4124564, 0.3575761, 0.1804375},
    //  {0.2126729, 0.7151522, 0.0721750},
    //  {0.0193339, 0.1191920, 0.9503041}
    // }

    // XYZ -> LMS
    // L =  0.38971 * X + 0.68898 * Y - 0.07868 * Z
    // M = -0.22981 * X + 1.18340 * Y + 0.04641 * Z
    // S =  0.00000 * X + 0.00000 * Y + 1.00000 * Z
    // {
    //  {0.38971, 0.68898, -0.07868},
    //  {-0.22981, 1.18340, 0.04641},
    //  {0, 0, 1}
    // }

    // Multiplication of the matrices (XYZ2LMS * sRGB2XYZ) gives:
    // {
    //  {0.305745, 0.622699, 0.0452755},
    //  {0.157788, 0.769668, 0.0880492},
    //  {0.0193339, 0.119192, 0.950304}
    // }

    // L =  0.305745 * R + 0.622699 * G + 0.0452755 * B
    // M =  0.157788 * R + 0.769668 * G + 0.0880492 * B
    // S =  0.0193339* R + 0.119192 * G + 0.950304  * B

    CImg<double> *channels[3] = {RGBred, RGBgreen, RGBblue};
    double coeff_L[3] = {0.305745, 0.622699, 0.0452755};
    double coeff_M[3] = {0.157788, 0.769668, 0.0880492};
    double coeff_S[3] = {0.0193339, 0.119192, 0.950304};

    // (coeff_L + coeff_M + coeff_S) / 3
    double coeff_R[3] = {0.160956, 0.503853, 0.36121};

    *L_cones = 0.0;  // L
    *M_cones = 0.0;  // M
    *S_cones = 0.0;  // S
    *rods = 0.0;

    //    std::cout << "L_cones size: " << L_cones->size() << std::endl;
    //    std::cout << "M_cones size: " << M_cones->size() << std::endl;
    //    std::cout << "S_cones size: " << S_cones->size() << std::endl;
    //    std::cout << "rods size: " << rods->size() << std::endl;

    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < (int)L_cones->size(); i++) {
            L_cones->_data[i] += coeff_L[k] * channels[k]->_data[i];
        }
    }
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < (int)M_cones->size(); i++) {
            M_cones->_data[i] += coeff_M[k] * channels[k]->_data[i];
        }
    }
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < (int)S_cones->size(); i++) {
            S_cones->_data[i] += coeff_S[k] * channels[k]->_data[i];
        }
    }
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < (int)rods->size(); i++) {
            rods->_data[i] += coeff_R[k] * channels[k]->_data[i];
        }
    }
    //    std::cout << 3 << std::endl;
    //    if (simState->currentTime == 0) {
    //        std::ofstream ofB, ofG, ofR;
    //        ofB.open("blue.data");
    //        ofG.open("green.data");
    //        ofR.open("red.data");
    //        for (int i = 0; i < L_cones->size(); i++) {
    //            ofB << L_cones->_data[i] << ", ";
    //            ofG << M_cones->_data[i] << ", ";
    //            ofR << S_cones->_data[i] << ", ";
    //        }
    //        ofB.close();
    //        ofG.close();
    //        ofR.close();

    //        RGBred->save("0_red.png");
    //        RGBgreen->save("0_green.png");
    //        RGBblue->save("0_blue.png");

    //        L_cones->save("L_cones.png");
    //        M_cones->save("M_cones.png");
    //        S_cones->save("S_cones.png");
    //        rods->save("rods.png");
    //    }

    clock_t end = clock();
    t_RGB_to_LMS += (double)(end - begin) / CLOCKS_PER_SEC;

    begin = clock();
    std::vector<Module *>::iterator module;

    for (module = modules.begin(); module != modules.end(); ++module) {
        // port search
        for (unsigned int o = 0; o < (*module)->source_ports.size(); o++) {
            ModulePort *mp = &((*module)->source_ports[o]);
            std::vector<std::string> l = mp->getSources();
            std::vector<Operation> p = mp->getOperations();

            // image input
            if (l[0] == "L_cones") {
                *accumulator = *L_cones;
            } else if (l[0] == "M_cones") {
                *accumulator = *M_cones;
            } else if (l[0] == "S_cones") {
                *accumulator = *S_cones;
            } else if (l[0] == "rods") {
                *accumulator = *rods;
            } else {
                //                std::cout << "problem?" << l[0] << std::endl;
                *accumulator = *(this->getModule(l[0])->getOutput());
                //                std::cout << " no problem" << std::endl;
                // other operations
                for (unsigned int k = 1; k < l.size(); k++) {
                    for (unsigned int m = 0; m < modules.size(); m++) {
                        Module *n = modules[m];
                        if (l[k] == n->id()) {
                            if (p[k - 1] == ADD)
                                *accumulator += *(n->getOutput());
                            else if (p[k - 1] == SUB)
                                *accumulator -= *(n->getOutput());
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
    //    std::cout << 4 << std::endl;

    end = clock();
    t_module_ports += (double)(end - begin) / CLOCKS_PER_SEC;

    begin = clock();
    for (unsigned int i = 0; i < modules.size(); i++) {
        //        std::cout << "problem? " << modules[i]->id() << std::endl;
        this->modules[i]->update();
        //        std::cout << "no problem " << modules[i]->id() << std::endl;
    }
    end = clock();
    t_modules += (double)(end - begin) / CLOCKS_PER_SEC;

    begin = clock();
    for (unsigned int i = 0; i < multimeters.size(); i++) {
        this->multimeters[i]->record();
        if (simState->currentTime == this->multimeters[i]->showTime()) {
            this->multimeters[i]->showGP(0);
            this->multimeters[i]->dump();
        }
    }
    end = clock();
    t_multimeters += (double)(end - begin) / CLOCKS_PER_SEC;

    begin = clock();
    double switchTime = 0;

    if (this->config->inputType == "WhiteNoise") {
        switchTime = ((WhiteNoise *)inputModule)->getSwitchTime();
    }
    //    std::cout << 5 << std::endl;
    CImg<double> *photoreceptors[4] = {L_cones, M_cones, S_cones, rods};
    this->displayMg->updateDisplays(this->input, photoreceptors, modules,
                                    this->simState);
    //    std::cout << 6 << std::endl;
    simState->currentTime += config->simulationStep;

    end = clock();
    t_update_display += double(end - begin) / CLOCKS_PER_SEC;

    return input;
}

const CImg<double> *Retina::getData(std::string id) {
    if (id == "Input") {
        return this->input;
    } else if (id == "L_cones")
        return this->L_cones;
    else if (id == "M_cones")
        return this->M_cones;
    else if (id == "S_cones")
        return this->S_cones;
    else if (id == "rods")
        return this->rods;
    else
        return this->getModule(id)->getOutput();
}

const CImg<double> *Retina::step() {
    return this->step(inputModule->getData(simState->currentTime));
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

Module *Retina::getModule(const std::string s) {
    for (unsigned int i = 0; i < this->modules.size(); i++) {
        if (this->modules[i]->id() == s) return this->modules[i];
    }

    return NULL;
}

int Retina::getNumberModules() { return modules.size(); }

void Retina::connect(std::vector<std::string> from, std::string to,
                     std::vector<Operation> operations,
                     SynapseType type_synapse) {
    if (verbose) {
        std::cout
            << "Warning: make sure you instantiated all the modules you want "
               "to use."
            << std::endl;
    }

    Module *neuronto = this->getModule(to);
    if (neuronto == NULL) {
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

        Module *neuronfrom = this->getModule(*ff);

        if (neuronfrom == NULL) {
            error("Retina::connect(): Source module " + to + " not found.");
        }

        if (verbose) {
            std::cout << neuronfrom->id() << " is being conected to "
                      << neuronto->id() << std::endl;
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
        PyErr_Print();
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
    return 0;  // It won't ever get here
}

const CImg<double> *Retina::getOutput(string layer) {
    //    if (layer == "Output") {
    //        layer = modules[0]->source_ports[0].getSources()[0];
    //        // layer = modules[0]->getID(0)[0];
    //    }

    for (std::vector<Module *>::iterator m = modules.begin();
         m != modules.end(); ++m) {
        if ((*m)->id() == layer) {
            return (*m)->getOutput();
        }
    }

    throw std::runtime_error("Invalid layer");
}

void Retina::stats() {
    std::cout << "Single modules elapsed time:" << std::endl;
    for (int i = 0; i < (int)this->modules.size(); i++) {
        std::cout << this->modules[i]->id() << ", "
                  << this->modules[i]->elapsed_time << ", "
                  << this->modules[i]->elapsed_wall_time << std::endl;
    }
    std::cout << "Retina RGB to LMS: " << t_RGB_to_LMS << std::endl;
    std::cout << "Retina update module ports: " << t_module_ports << std::endl;
    std::cout << "Retina update modules: " << t_modules << std::endl;
    std::cout << "Retina update multimeters: " << t_multimeters << std::endl;
    std::cout << "Retina update display: " << t_update_display << std::endl;
}

void define_retina_python() {
    void (Retina::*Py_Step_obj)(const py::object &) = &Retina::Py_Step;
    void (Retina::*Py_Step_void)() = &Retina::Py_Step;

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
        .def("Tweak", &Retina::Py_Tweak)
        .def("Step", Py_Step_obj)
        .def("Step", Py_Step_void)
        .def("Run", &Retina::Py_Run)
        .def("GetValue", &Retina::Py_GetValue)
        .def("GetOutput", &Retina::Py_GetOutput)
        .def("GetSpikes", &Retina::Py_GetSpikes)
        .def("Stats", &Retina::Py_Stats);
}
