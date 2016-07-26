#include "RetinaLoader.h"
#include <exception>
#include "../module.h"

void RetinaLoader::TempStep(double step) {
    retina_->set_step(step);
};

void RetinaLoader::SimTime(int sim_time) {
    retina_->setSimTimeTot(sim_time);
};

void RetinaLoader::NumTrials(double ntrials) {
    if (ntrials > 0) {
        retina_->setSimTotalRep(ntrials);
    }
    else {
        std::cout << "Error: ntrials <= 0" << std::endl;
        return;
    }
};

void RetinaLoader::PixelsPerDegree(double ppd) {
    if (ppd > 0.0) {
        retina_->setPixelsPerDegree(ppd);
    }
    else {
        std::cout << "Error: ppd <= 0" << std::endl;
        return;
    }
};

void RetinaLoader::NRepetitions(double repetitions) {
    if (repetitions > 0) {
        retina_->setRepetitions(repetitions);
    }
    else {
        std::cout << "Error: repetitions <= 0" << std::endl;
        return;
    }
};

void RetinaLoader::DisplayDelay(int delay) {
    if (delay >= 0) {
        retina_->displayMg.setDelay(delay);
    }
    else {
        std::cout << "Error: delay < 0" << std::endl;
        return;
    }
};

void RetinaLoader::DisplayZoom(double zoom) {
    if (zoom >= 0) {
        retina_->displayMg.setZoom(zoom);
    }
    else {
        std::cout << "Error: zoom < 0" << std::endl;
        return;
    }
};

void RetinaLoader::DisplayWindows(int windows_per_row) {
    if (windows_per_row > 0) {
        retina_->displayMg.setImagesPerRow(windows_per_row);
    }
    else {
        std::cout << "Error: windows_per_row <= 0" << std::endl;
        return;
    }
};

void RetinaLoader::Input(std::string input_type, py::dict args) {
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
            py::extract<float>(args["columns"]),
            py::extract<float>(args["rows"]),
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
            py::extract<float>(args["columns"]),
            py::extract<float>(args["rows"]),
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
            py::extract<float>(args["columns"]),
            py::extract<float>(args["rows"])
        );
        retina_->setRepetitions(1.0);
    }
    else if (input_type == "camera") {
        py::tuple size = py::extract<py::tuple>(args["size"]);
        retina_->setColumns(py::extract<int>(size[0]));
        retina_->setRows(py::extract<int>(size[1]));
    }
    else {
        std::cout << "Error: invalid input type" << std::endl;
    }
}

void RetinaLoader::Create(std::string module_type, std::string module_id, py::dict args) {
    module* newModule;
    if (module_type == "LinearFilter") {
        newModule = new LinearFilter(retina_->getColumns(), retina_->getRows(), retina_->getSimStep(), 0.0);
    }
    else if (module_type == "SingleCompartment") {
        newModule = new SingleCompartment(retina_->getColumns(), retina_->getRows(), retina_->getSimStep());
    }
    else if (module_type == "StaticNonLinearity") {
        newModule = new StaticNonLinearity(retina_->getColumns(), retina_->getRows(), retina_->getSimStep(), 0);
    }
    else if (module_type == "CustomNonLinearity") {
        newModule = new StaticNonLinearity(retina_->getColumns(), retina_->getRows(), retina_->getSimStep(), 1);
    }
    else if (module_type == "SigmoidNonLinearity") {
        newModule = new StaticNonLinearity(retina_->getColumns(), retina_->getRows(), retina_->getSimStep(), 2);
    }
    else if (module_type == "ShortTermPlasticity") {
        newModule = new ShortTermPlasticity(retina_->getColumns(), retina_->getRows(), retina_->getSimStep(), 0.0, 0.0, 0.0, 0.0, false);
    }
    else if (module_type == "GaussFilter") {
        newModule = new GaussFilter(retina_->getColumns(), retina_->getRows(), retina_->getPixelsPerDegree());
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

    for (unsigned int i = 0; i < len; i++) {
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

void RetinaLoader::Connect(py::object arg0, std::string connect_to, std::string type_synapse) {
    vector <int> operations;
    vector <string> modulesID;

    retina_->displayMg.allocateValues(retina_->getNumberModules(), retina_->getSimStep());
    std::string arg0_type = py::extract<std::string>(arg0.attr("__class__").attr("__name__"));

    if (arg0_type == "str") {
        modulesID.push_back(py::extract<std::string>(arg0));
    }
    else if (arg0_type == "list") {
        py::object l = arg0;
        unsigned long size = py::extract<unsigned long>(l.attr("__len__")());
        for (unsigned int i = 0; i < size; i++) {
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
    retina_->connect(modulesID, connect_to, operations, type_synapse);
}

void RetinaLoader::Show(std::string module_id, bool show, py::dict args) {
    for(int l = 1; l < retina_->getNumberModules(); l++){
        module *m = retina_->getModule(l);
        string ID = m->getModuleID();

        if (ID == module_id) {
            if (show) {
                retina_->displayMg.setIsShown(true, l);
                retina_->displayMg.setIsShown(true, 0);
            }
            else retina_->displayMg.setIsShown(false, l);
        }

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            retina_->displayMg.setMargin(margin, l);
        }
    }

    if (module_id == "Input") {
        if (show)
            retina_->displayMg.setIsShown(true, 0);
        else
            retina_->displayMg.setIsShown(false, 0);

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            retina_->displayMg.setMargin(margin, 0);
        }
    }
}

void RetinaLoader::Multimeter(std::string multimeter_type, std::string multimeter_id, std::string module_id, py::dict args, bool show) {
    std::string showstr;
    if (show) showstr = "True";
    else showstr = "False";

    std::transform(multimeter_type.begin(), multimeter_type.end(), multimeter_type.begin(), ::tolower);

    if (multimeter_type == "spatial") {
        int param1 = py::extract<int>(args["value"]);
        bool rowcol = py::extract<bool>(args["rowcol"]);

        if (!rowcol) param1 = -param1;

        retina_->displayMg.addMultimeterTempSpat(
            multimeter_id, module_id, param1, py::extract<int>(args["timeStep"]), false, showstr
        );
    }
    else if (multimeter_type == "temporal") {
        retina_->displayMg.addMultimeterTempSpat(
            multimeter_id, module_id, py::extract<int>(args["x"]), py::extract<int>(args["y"]), true, showstr
        );
    }
    else if (multimeter_type == "linear-nonlinear") {
        retina_->displayMg.addMultimeterLN(
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
