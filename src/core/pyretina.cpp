#include "COREM/core/retina.h"

// step is in ms
void Retina::Py_TempStep(int step) { config->simulationStep = step; }

// sim_time is in ms
void Retina::Py_SimTime(int sim_time) { config->simulationTime = sim_time; }

void Retina::Py_NumTrials(int ntrials) { config->numTrials = ntrials; }

void Retina::Py_PixelsPerDegree(int ppd) { config->pixelsPerDegree = ppd; }

void Retina::Py_DisplayDelay(int delay) { config->displayDelay = delay; }

void Retina::Py_DisplayZoom(int zoom) { config->displayZoom = zoom; }

void Retina::Py_DisplayWindows(int windows_per_row) {
    if (windows_per_row > 0) {
        this->displayMg.setImagesPerRow(windows_per_row);
    } else {
        std::cout << "Error: windows_per_row <= 0" << std::endl;
        return;
    }
}

void Retina::Py_Input(std::string input_type, py::dict args) {
    if (input_type == "sequence") {
        std::map<std::string, param_val_t> params;
        params["path"].s =
            new std::string(py::extract<std::string>(args["path"]));
        params["time_per_image"].u =
            py::extract<unsigned int>(args["steps_per_image"]) *
            config->simulationStep;

        this->inputModule = new Sequence(0, 0, params);
        unsigned int cols, rows;
        this->inputModule->getSize(cols, rows);
        this->config->columns = cols;
        this->config->rows = rows;
    } else if (input_type == "image") {
        std::map<std::string, param_val_t> params;
        params["path"].s =
            new std::string(py::extract<std::string>(args["path"]));

        this->inputModule = new ImageSource(0, 0, params);
        unsigned int cols, rows;
        this->inputModule->getSize(cols, rows);
        this->config->columns = cols;
        this->config->rows = rows;
    } else if (input_type == "grating") {
        std::map<std::string, param_val_t> params;
        params["ptype"].i = (int)py::extract<float>(args["type"]);
        params["pstep"].d = (double)py::extract<float>(args["step"]);
        params["plengthB"].d = (double)py::extract<float>(args["length1"]);
        params["plength"].d = (double)py::extract<float>(args["length2"]);
        params["plength2"].d = (double)py::extract<float>(args["length3"]);
        params["pfreq"].d = (double)py::extract<float>(args["freq"]);
        params["pT"].d = (double)py::extract<float>(args["period"]);
        params["pLum"].d = (double)py::extract<float>(args["Lum"]);
        params["pCont"].d = (double)py::extract<float>(args["Contr"]);
        params["pphi"].d = (double)py::extract<float>(args["phi_s"]);
        params["pphi_t"].d = (double)py::extract<float>(args["phi_t"]);
        params["ptheta"].d = (double)py::extract<float>(args["orientation"]);
        params["red"].d = (double)py::extract<float>(args["red_weight"]);
        params["green"].d = (double)py::extract<float>(args["green_weight"]);
        params["blue"].d = (double)py::extract<float>(args["blue_weight"]);
        params["pred_phi"].d = (double)py::extract<float>(args["red_phase"]);
        params["pgreen_phi"].d =
            (double)py::extract<float>(args["green_phase"]);
        params["pblue_phi"].d = (double)py::extract<float>(args["blue_phase"]);

        inputModule =
            new GratingGenerator(py::extract<float>(args["columns"]),
                                 py::extract<float>(args["rows"]), params);

        this->config->columns = py::extract<unsigned int>(args["columns"]);
        this->config->rows = py::extract<unsigned int>(args["rows"]);
    } else if (input_type == "fixationalMovGrating") {
        std::map<std::string, param_val_t> params;

        params["radius"].d = (double)py::extract<float>(args["circle_radius"]);
        params["jitter"].d = (double)py::extract<float>(args["jitter_period"]);
        params["period"].d = (double)py::extract<float>(args["spatial_period"]);
        params["step"].d = (double)py::extract<float>(args["step_size"]);
        params["luminance"].d = (double)py::extract<float>(args["Lum"]);
        params["contrast"].d = (double)py::extract<float>(args["Contr"]);
        params["orientation"].d =
            (double)py::extract<float>(args["orientation"]);
        params["red_weight"].d = (double)py::extract<float>(args["red_weight"]);
        params["green_weight"].d =
            (double)py::extract<float>(args["green_weight"]);
        params["blue_weight"].d =
            (double)py::extract<float>(args["blue_weight"]);
        params["type1"].i = (int)py::extract<float>(args["type1"]);  // was t1
        params["type2"].i = (int)py::extract<float>(args["type2"]);  // was t2
        params["tswitch"].i =
            (int)py::extract<float>(args["switch"]);  // was ts

        this->inputModule =
            new FixationalMovGrating(py::extract<float>(args["columns"]),
                                     py::extract<float>(args["rows"]), params);

        this->config->columns = py::extract<unsigned int>(args["columns"]);
        this->config->rows = py::extract<unsigned int>(args["rows"]);
        // this->setRepetitions(1.0);
    } else if (input_type == "whiteNoise") {
        // TODO: check return value
        std::map<std::string, param_val_t> params;

        params["mean"].d = (double)py::extract<float>(args["mean"]);
        params["contrast1"].d = (double)py::extract<float>(args["contrast1"]);
        params["contrast2"].d = (double)py::extract<float>(args["contrast2"]);
        params["period"].d = (double)py::extract<float>(args["period"]);
        params["switchT"].d = (double)py::extract<float>(args["switch"]);

        this->inputModule =
            new WhiteNoise(py::extract<float>(args["sizeX"]),
                           py::extract<float>(args["sizeY"]), params);
        ((WhiteNoise *)this->inputModule)
            ->initializeDist(simState->currentTrial);

        this->config->columns = py::extract<unsigned int>(args["sizeX"]);
        this->config->rows = py::extract<unsigned int>(args["sizeY"]);
    } else if (input_type == "impulse") {
        std::map<std::string, param_val_t> params;

        params["start"].d = (double)py::extract<float>(args["start"]);
        params["stop"].d = (double)py::extract<float>(args["stop"]);
        params["amplitude"].d = (double)py::extract<float>(args["amplitude"]);
        params["offset"].d = (double)py::extract<float>(args["offset"]);

        inputModule = new Impulse(py::extract<float>(args["columns"]),
                                  py::extract<float>(args["rows"]), params);

        this->config->columns = py::extract<unsigned int>(args["columns"]);
        this->config->rows = py::extract<unsigned int>(args["rows"]);
    } else if (input_type == "camera") {
        py::tuple size = py::extract<py::tuple>(args["size"]);
        this->config->columns = py::extract<unsigned int>(size[0]);
        this->config->rows = py::extract<unsigned int>(size[1]);
    } else {
        throw std::runtime_error("Error: invalid input type.");
    }

    this->initialize();
    this->displayMg.addInputDisplay();
}

void invalid_parameter(std::string what, std::string param) {
    std::cerr << "Invalid parameter " << param << " when creating " << what
              << "." << std::endl;
    throw std::runtime_error("Invalid parameter " + param + " when creating " +
                             what + ".");
}

LinearFilter *createLinearFilter(retina_config_t *conf, std::string id,
                                 const py::dict &args) {
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    std::string type;
    double tau;
    double n;

    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        param_val_t param;
        if (k == "type") {
            type = py::extract<std::string>(args[k].attr("__str__")());
        } else if (k == "tau") {
            tau = py::extract<double>(args[k]);
        } else if (k == "n") {
            n = py::extract<double>(args[k]);
        } else {
            invalid_parameter("LinearFilter", k);
        }
    }

    return new LinearFilter(id, conf, tau, n, type);
}

SingleCompartment *createSingleCompartment(retina_config_t *conf,
                                           std::string id,
                                           const py::dict &args) {
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    double Cm, Rm, tau;
    std::vector<double> E;
    unsigned int number_current_ports, number_conductance_ports;

    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        if (k == "Rm") {
            Rm = py::extract<double>(args[k]);
        } else if (k == "tau") {
            tau = py::extract<double>(args[k]);
        } else if (k == "Cm") {
            Cm = py::extract<double>(args[k]);
        } else if (k == "E") {
            long param_len = py::extract<long>(args[k].attr("__len__")());
            for (long i = 0; i < param_len; ++i) {
                E.push_back(
                    py::extract<double>(args[k].attr("__getitem__")(i)));
            }
        } else if (k == "number_current_ports") {
            number_current_ports = py::extract<unsigned int>(args[k]);
        } else if (k == "number_conductance_ports") {
            number_conductance_ports = py::extract<unsigned int>(args[k]);
        } else {
            invalid_parameter("SingleCompartment", k);
        }
    }

    return new SingleCompartment(id, conf, number_current_ports,
                                 number_conductance_ports, Rm, tau, Cm, E);
}

StaticNonLinearity *createNonLinearity(retina_config_t *conf, std::string id,
                                       const py::dict &args,
                                       unsigned int type) {
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    double slope, offset, exponent;
    double threshold = std::numeric_limits<double>::infinity();

    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        if (k == "slope") {
            slope = py::extract<double>(args[k]);
        } else if (k == "offset") {
            offset = py::extract<double>(args[k]);
        } else if (k == "exponent") {
            exponent = py::extract<double>(args[k]);
        } else if (k == "threshold") {
            threshold = py::extract<double>(args[k]);
        } else {
            invalid_parameter("StaticNonLinearity", k);
        }
    }

    return new StaticNonLinearity(id, conf, type, slope, offset, exponent,
                                  threshold);
}

CustomNonLinearity *createCustomNonLinearity(retina_config_t *conf,
                                             std::string id,
                                             const py::dict &args) {
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    std::vector<double> slope, offset, exponent, start, end;

    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        if (k == "slope") {
            unsigned long param_len =
                py::extract<unsigned long>(args[k].attr("__len__")());
            for (int i = 0; i < param_len; ++i) {
                slope.push_back(
                    py::extract<double>(args[k].attr("__getitem__")(i)));
            }
        } else if (k == "offset") {
            unsigned long param_len =
                py::extract<unsigned long>(args[k].attr("__len__")());
            for (int i = 0; i < param_len; ++i) {
                offset.push_back(
                    py::extract<double>(args[k].attr("__getitem__")(i)));
            }
        } else if (k == "exponent") {
            unsigned long param_len =
                py::extract<unsigned long>(args[k].attr("__len__")());
            for (int i = 0; i < param_len; ++i) {
                exponent.push_back(
                    py::extract<double>(args[k].attr("__getitem__")(i)));
            }
        } else if (k == "start") {
            unsigned long param_len =
                py::extract<unsigned long>(args[k].attr("__len__")());
            for (int i = 0; i < param_len; ++i) {
                start.push_back(
                    py::extract<double>(args[k].attr("__getitem__")(i)));
            }
        } else if (k == "end") {
            unsigned long param_len =
                py::extract<unsigned long>(args[k].attr("__len__")());
            for (int i = 0; i < param_len; ++i) {
                end.push_back(
                    py::extract<double>(args[k].attr("__getitem__")(i)));
            }
        } else {
            invalid_parameter("CustomNonLinearity", k);
        }
    }

    return new CustomNonLinearity(id, conf, slope, offset, exponent, start,
                                  end);
}

ShortTermPlasticity *createShortTermPlasticity(retina_config_t *conf,
                                               std::string id,
                                               const py::dict &args) {
    // TODO: similar to StaticNonLinearity?
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    double slope, offset, exponent, kf, kd, vinf, tau;
    double threshold = std::numeric_limits<double>::infinity();

    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        if (k == "slope") {
            slope = py::extract<double>(args[k]);
        } else if (k == "offset") {
            offset = py::extract<double>(args[k]);
        } else if (k == "exponent") {
            exponent = py::extract<double>(args[k]);
        } else if (k == "kf") {
            kf = py::extract<double>(args[k]);
        } else if (k == "kd") {
            kd = py::extract<double>(args[k]);
        } else if (k == "VInf") {
            vinf = py::extract<double>(args[k]);
        } else if (k == "tau") {
            tau = py::extract<double>(args[k]);
        } else if (k == "threshold") {
            threshold = py::extract<double>(args[k]);
        } else {
            invalid_parameter("ShortTermPlasticity", k);
        }
    }

    return new ShortTermPlasticity(id, conf, slope, offset, exponent, kf, kd,
                                   tau, vinf, threshold);
}

GaussFilter *createGaussFilter(retina_config_t *conf, std::string id,
                               const py::dict &args) {
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    double sigma;

    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        param_val_t param;
        if (k == "sigma") {
            sigma = py::extract<double>(args[k]);
        } else {
            invalid_parameter("GaussFilter", k);
        }
    }

    return new GaussFilter(id, conf, sigma);
}

SpaceVariantGaussFilter *createSpaceVariantGaussFilter(retina_config_t *conf,
                                                       std::string id,
                                                       const py::dict &args) {
    const py::object keys = args.iterkeys();
    const py::object vals = args.itervalues();
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    double sigma, K, R0;
    for (unsigned int i = 0; i < len; i++) {
        py::object key, v;
        key = keys.attr("next")();
        v = vals.attr("next")();
        std::string k = py::extract<std::string>(key);

        if (k == "sigma") {
            sigma = py::extract<double>(args[k]);
        } else if (k == "K") {
            K = py::extract<double>(args[k]);
        } else if (k == "R0") {
            R0 = py::extract<double>(args[k]);
        } else {
            invalid_parameter("SpaceVariantGaussFilter", k);
        }
    }

    return new SpaceVariantGaussFilter(id, conf, sigma, K, R0);
}

Parrot *createParrot(retina_config_t *conf, std::string id,
                     const py::dict &args) {
    unsigned long len = py::extract<unsigned long>(args.attr("__len__")());

    if (len != 0) {
        std::cerr << "Unexpected parameters when adding Parrot." << std::endl;
        std::runtime_error("Unexpected parameter when adding Parrot.");
    }

    return new Parrot(id, conf);
}

void Retina::Py_Create(std::string module_type, std::string module_id,
                       py::dict args) {
    Module *newModule;

    if (module_type == "LinearFilter") {
        newModule = createLinearFilter(this->config, module_id, args);
    } else if (module_type == "SingleCompartment") {
        newModule = createSingleCompartment(this->config, module_id, args);
    } else if (module_type == "StaticNonLinearity") {
        newModule = createNonLinearity(this->config, module_id, args, 0);
    } else if (module_type == "CustomNonLinearity") {
        newModule = createCustomNonLinearity(this->config, module_id, args);
    } else if (module_type == "SymmetricSigmoidNonLinearity") {
        newModule = createNonLinearity(this->config, module_id, args, 2);
    } else if (module_type == "SigmoidNonlinearity") {
        newModule = createNonLinearity(this->config, module_id, args, 3);
    } else if (module_type == "ShortTermPlasticity") {
        newModule = createShortTermPlasticity(this->config, module_id, args);
    } else if (module_type == "GaussFilter") {
        newModule = createGaussFilter(this->config, module_id, args);
    } else if (module_type == "SpaceVariantGaussFilter") {
        newModule =
            createSpaceVariantGaussFilter(this->config, module_id, args);
    } else if (module_type == "Parrot") {
        newModule = createParrot(this->config, module_id, args);
    } else {
        throw std::runtime_error("Error: invalid module type.");
    }

    this->addModule(newModule);
    displayMg.addDisplay(newModule->id());
}

void Retina::Py_Connect(py::object arg0, std::string connect_to,
                        std::string type_synapse) {
    vector<Operation> operations;
    vector<string> modulesID;

    std::string arg0_type =
        py::extract<std::string>(arg0.attr("__class__").attr("__name__"));

    if (arg0_type == "str") {
        modulesID.push_back(py::extract<std::string>(arg0));
    } else if (arg0_type == "list") {
        py::object l = arg0;
        unsigned long size = py::extract<unsigned long>(l.attr("__len__")());
        for (unsigned int i = 0; i < size; i++) {
            std::string entry = py::extract<std::string>(l[i]);
            if (i % 2) {
                if (entry == "+")
                    operations.push_back(ADD);
                else if (entry == "-")
                    operations.push_back(SUB);
                else {
                    std::cout << "Error: invalid operation." << std::endl;
                    return;
                }
            } else {
                modulesID.push_back(entry);
            }
        }
    } else {
        std::cout << "Error: invalid type for arg0" << std::endl;
        return;
    }

    SynapseType st;
    if (type_synapse == "Current") {
        st = CURRENT;
    } else if (type_synapse == "CurrentInhib") {
        st = CURRENT_INHIB;
    } else if (type_synapse == "Conductance") {
        st = CONDUCTANCE;
    } else if (type_synapse == "ConductanceInhib") {
        st = CONDUCTANCE_INHIB;
    } else {
        std::string strfrom;
        if (operations.size() != 0) {
            strfrom = "(" + modulesID[0];
            if (operations[0] == ADD)
                strfrom += " + ";
            else
                strfrom += " - ";
            strfrom += modulesID[1] + ")";
        } else {
            strfrom = modulesID[0];
        }
        error("Retina::Py_Connect(" + strfrom + " -> " + connect_to +
              "): invalid synapse type.");
    }

    // TODO: check return value
    this->connect(modulesID, connect_to, operations, st);
}

void Retina::Py_Show(std::string module_id, bool show, py::dict args) {
    for (int l = 1; l < this->getNumberModules(); l++) {
        Module *m = this->getModule(l);
        string ID = m->id();

        if (ID == module_id) {
            if (show) {
                this->displayMg.setIsShown(true, l);
                this->displayMg.setIsShown(true, 0);
            } else
                this->displayMg.setIsShown(false, l);
        }

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            this->displayMg.setMargin(margin, l);
        }
    }

    if (module_id == "Input") {
        if (show) {
            this->displayMg.setIsShown(true, 0);
        } else {
            this->displayMg.setIsShown(false, 0);
        }

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            this->displayMg.setMargin(margin, 0);
        }
    }
}

void Retina::Py_Multimeter(std::string multimeter_type,
                           std::string multimeter_id, std::string module_id,
                           py::dict args, bool show) {
    std::transform(multimeter_type.begin(), multimeter_type.end(),
                   multimeter_type.begin(), ::tolower);

    Multimeter *mul;
    Module *module = NULL;
    CImg<double> *src_img;

    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;

    for (unsigned int k = 1; k < this->modules.size() && module == NULL; k++) {
        if (this->modules[k]->id() == module_id) {
            module = this->modules[k];
        }
    }

    if (module == NULL && module_id != "Input" && module_id != "L_cones" &&
        module_id != "M_cones" && module_id != "S_cones" &&
        module_id != "rods") {
        error("Invalid Source module " + module_id);
    }

    if (module_id == "Input")
        src_img = &this->input;
    else if (module_id == "L_cones")
        src_img = &ch1;
    else if (module_id == "M_cones")
        src_img = &ch2;
    else if (module_id == "S_cones")
        src_img = &ch3;
    else if (module_id == "rods")
        src_img = &rods;

    if (multimeter_type == "spatial") {
        unsigned row_or_col = py::extract<unsigned int>(args["line"]);
        std::string orientation = py::extract<std::string>(args["orientation"]);
        unsigned int timeToShow = py::extract<unsigned int>(args["timeStep"]);

        Orientation or_;

        if (orientation == "horizontal")
            or_ = HORIZONTAL;
        else if (orientation == "vertical")
            or_ = VERTICAL;
        else
            throw std::runtime_error("Invalid value for orientation.");

        mul = new SpatialMultimeter(cols, rows, this->config, multimeter_id,
                                    timeToShow, or_, row_or_col);
    } else if (multimeter_type == "temporal") {
        mul = new TemporalMultimeter(
            cols, rows, this->config, multimeter_id,
            this->config->simulationTime, py::extract<unsigned int>(args["x"]),
            py::extract<unsigned int>(args["y"]), config->simulationStep);
        /* this->displayMg.addMultimeterTempSpat(
            multimeter_id, module_id, py::extract<int>(args["x"]),
            py::extract<int>(args["y"]), true, showstr); */
    } else if (multimeter_type == "linear-nonlinear") {
        // TODO: despaghettify LNMultimeter and put it here
        mul = new LNMultimeter(cols, rows, this->config, multimeter_id,
                               this->config->simulationTime,
                               py::extract<unsigned int>(args["x"]),
                               py::extract<unsigned int>(args["y"]),
                               py::extract<double>(args["segment"]),
                               py::extract<double>(args["interval"]),
                               py::extract<double>(args["start"]),
                               py::extract<double>(args["stop"]));
        /* this->displayMg.addMultimeterLN(
            multimeter_id, module_id, py::extract<int>(args["x"]),
            py::extract<int>(args["y"]),
           py::extract<double>(args["segment"]),
            py::extract<double>(args["interval"]),
            py::extract<double>(args["start"]),
            py::extract<double>(args["stop"]), showstr); */
        mul->setSourceImage(&this->input);

    } else {
        std::cout << "Error: invalid multimeter type." << std::endl;
        return;
    }

    mul->setShowable(show);
    if (module != NULL)
        mul->setSourceModule(module);
    else
        mul->setSourceImage(src_img);
    multimeters.push_back(mul);
}

CImg<double> *Retina::convertImage(const boost::python::api::object &img) {
    std::string objtype =
        py::extract<std::string>(img.attr("__class__").attr("__module__"));
    objtype += ".";
    objtype += py::extract<std::string>(img.attr("__class__").attr("__name__"));

    if (objtype == "numpy.ndarray") {
        return convertndarray(img);
    } else {
        throw std::invalid_argument("Convertion from " + objtype +
                                    " is not supported.");
    }
}

CImg<double> *Retina::convertndarray(const boost::python::api::object &img) {
    unsigned int cols = this->config->columns;
    unsigned int rows = this->config->rows;

    CImg<double> *ret = new CImg<double>(cols, rows, 1, 3, 0.0);

    Py_ssize_t bufLen;
    void const *buffer = NULL;
    py::object data = img.attr("data");
    bool isReadBuffer = !PyObject_AsReadBuffer(data.ptr(), &buffer, &bufLen);

    if (!isReadBuffer || bufLen != 3 * cols * rows) {
        throw std::invalid_argument("Something is wrong with the image.");
    }

    unsigned char *buf = (unsigned char *)buffer;

    for (unsigned int i = 0; i < cols; i++) {
        for (unsigned int j = 0; j < rows; j++) {
            (*ret)(i, j, 0, 0) = buf[3 * (i + j * cols)];
            (*ret)(i, j, 0, 1) = buf[3 * (i + j * cols) + 1];
            (*ret)(i, j, 0, 2) = buf[3 * (i + j * cols) + 2];
        }
    }

    return ret;
}

void Retina::Py_Step(const py::object &img) {
    CImg<double> *inputImg = this->convertImage(img);
    this->step(inputImg);
}

double Retina::Py_GetValue(int row, int col, string layer) {
    return this->getValue(row * this->config->columns + col, layer);
}
