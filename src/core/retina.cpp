#include <COREM/core/retina.h>

Retina::Retina(int columns, int rows, double temporal_step)
: columns_(columns), rows_(rows), simStep(temporal_step), displayMg(1, 1) {
    this->reset(columns, rows, temporal_step);
}

void Retina::reset(int columns, int rows, double temporal_step) {
    simStep = temporal_step;
    setColumns(columns);
    setRows(rows);
    pixelsPerDegree = 1.0;
    inputType = 0;
    numberImages = 0;
    repetitions = 0;

    verbose = true;

    modules.clear();
    modules.push_back((new module())); // This is the output module, I'll add it properly

    output = new CImg <double>(columns_, rows_, 1, 1, 0.0);
    accumulator = *(new CImg <double>(columns_, rows_, 1, 1, 0.0));

    RGBred =    *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    RGBgreen =  *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    RGBblue =   *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    ch1 =       *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    ch2 =       *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    ch3 =       *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    rods =      *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    X_mat =     *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    Y_mat =     *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    Z_mat =     *(new CImg <double>(columns_, rows_, 1, 1, 0.0));

    simCurrTime = 0;
    WN = nullptr;
    displayMg.reset();
}

//------------------------------------------------------------------------------//

void Retina::setColumns(int columns) {
    cout << "Retina::setColumns " << columns << endl;
    if (columns > 0) {
        columns_ = columns;
        displayMg.setColumns(columns_);
    }
}

void Retina::setRows(int rows) {
    cout << "Retina::setRows " << rows << endl;
    if (rows > 0) {
        rows_ = rows;
        displayMg.setRows(rows_);
    }
}

void Retina::set_step(double temporal_step) {
    if (temporal_step>0){
        simStep = temporal_step;
    }
}

void Retina::setPixelsPerDegree(double ppd) {
    pixelsPerDegree = ppd;
}

double Retina::getPixelsPerDegree(){
    return pixelsPerDegree;
}

int Retina::getColumns() {
    return columns_;
}

int Retina::getRows() {
    return rows_;
}

double Retina::getSimStep() {
    return simStep;
}

void Retina::setSimTotalTrials(double r) {
    totalNumberTrials = r;
}

void Retina::setSimCurrentTrial(double r) {
    currentTrial = r;
}

void Retina::setSimDuration(int t) {
    simDuration = t;
}

void Retina::setSimCurrTime(int t) {
    simCurrTime = t;
}

double Retina::getSimCurrentTrial() {
    return currentTrial;
}

double Retina::getSimTotalTrials() {
    return totalNumberTrials;
}

int Retina::getSimDuration() {
    return simDuration;
}

int Retina::getSimCurrTime() {
    return simCurrTime;
}

//------------------------------------------------------------------------------//

void Retina::setRepetitions(int r) {
    repetitions = r;
}

//------------------------------------------------------------------------------//


void Retina::allocateValues() {
    if(verbose)cout << "Allocating "<< (getNumberModules()-1) << " retinal modules." << endl;
    if(verbose)cout << "columns = "<< columns_ << endl;
    if(verbose)cout << "rows = "<< rows_ << endl;
    if(verbose)cout << "Temporal step = "<< simStep << " ms" << endl;
    output = new CImg <double>(columns_, rows_, 1, 1, 0.0);
    accumulator = *(new CImg <double>(columns_, rows_, 1, 1, 0.0));

    RGBred =    *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    RGBgreen =  *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    RGBblue =   *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    ch1 =       *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    ch2 =       *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    ch3 =       *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    rods =      *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    X_mat =     *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    Y_mat =     *(new CImg <double>(columns_, rows_, 1, 1, 0.0));
    Z_mat =     *(new CImg <double>(columns_, rows_, 1, 1, 0.0));

    for (unsigned int i = 1; i < modules.size(); i++) {
        module* m = modules[i];
        m->allocateValues();
        displayMg.addDisplay(i, m->getModuleID());
    }
}


CImg<double> *rgb2xyz(CImg<double>* input) {
    CImg<double> *res = new CImg<double>(input->width(), input->height(), 1, 3, 0.0);
    cimg_forXY(*input, x, y) {
        (*res)(x, y, 0, 0) = 0.4124564*(*input)(x, y, 0, 0) + 0.3575761*(*input)(x, y, 0, 1) + 0.1804375*(*input)(x, y, 0, 2),
        (*res)(x, y, 0, 1) = 0.2126729*(*input)(x, y, 0, 0) + 0.7151522*(*input)(x, y, 0, 1) + 0.0721750*(*input)(x, y, 0, 2),
        (*res)(x, y, 0, 2) = 0.0193339*(*input)(x, y, 0, 0) + 0.1191920*(*input)(x, y, 0, 1) + 0.9503041*(*input)(x, y, 0, 2);
    }
    return res;
}

CImg<double> *xyz2lms(CImg<double>* input) {
    CImg<double> *res = new CImg<double>(input->width(), input->height(), 1, 3, 0.0);
    cimg_forXY(*input, x, y) {
        (*res)(x, y, 0, 0) = 0.38971*(*input)(x, y, 0, 0) + 0.68898*(*input)(x, y, 0, 1) - 0.07868*(*input)(x, y, 0, 2),
        (*res)(x, y, 0, 1) = -0.22981*(*input)(x, y, 0, 0) + 1.1834*(*input)(x, y, 0, 1) + 0.04641*(*input)(x, y, 0, 2),
        (*res)(x, y, 0, 2) = (*input)(x, y, 0, 2);
    }
    return res;
}

CImg<double> *greyscale2rgb(CImg<double>* input) {
    CImg<double> *res = new CImg<double>(input->width(), input->height(), 1, 3, 0.0);
    cimg_forXY(*input, x, y) {
        (*res)(x, y, 0, 0) = (*input)(x, y, 0, 0),
        (*res)(x, y, 0, 1) = (*input)(x, y, 0, 0),
        (*res)(x, y, 0, 2) = (*input)(x, y, 0, 0);
    }
    return res;
}

CImg<double> *Retina::feedInput(CImg<double>* input) {
    /* if ((unsigned int)(input->size())==static_cast<unsigned int>(sizeX*sizeY))
        input = greyscale2rgb(input); */

    // To be kept until greyscale2rgb, xyz2lms and rgb2xyz are fully debugged

    if ((unsigned int)(input->size())==static_cast<unsigned int>(columns_*rows_)) {
        // Separate color channels
        cimg_forXY(*input,x,y) {
            RGBred(x,y,0,0) = (*input)(x,y,0,0),    // Red component of image sent to imgR
            RGBgreen(x,y,0,0) = (*input)(x,y,0,0),    // Green component of image sent to imgG
            RGBblue(x,y,0,0) = (*input)(x,y,0,0);    // Blue component of image sent to imgB
        }
    } else {
       // Separate color channels
       cimg_forXY(*input,x,y) {
           RGBred(x,y,0,0) = (*input)(x,y,0,0),    // Red component of image sent to imgR
           RGBgreen(x,y,0,0) = (*input)(x,y,0,1),    // Green component of image sent to imgG
           RGBblue(x,y,0,0) = (*input)(x,y,0,2);    // Blue component of image sent to imgB
       }
    }

    // Hunt-Pointer-Estévez (HPE) transform
    // sRGB --> XYZ
    X_mat = 0.4124564*RGBblue + 0.3575761*RGBgreen + 0.1804375*RGBred;
    Y_mat = 0.2126729*RGBblue + 0.7151522*RGBgreen + 0.0721750*RGBred;
    Z_mat = 0.0193339*RGBblue + 0.1191920*RGBgreen + 0.9503041*RGBred;

    // XYZ --> LMS
    ch1 = 0.38971*X_mat + 0.68898*Y_mat - 0.07868*Z_mat;
    ch2 = -0.22981*X_mat + 1.1834*Y_mat + 0.04641*Z_mat;
    ch3 = Z_mat;

    rods = (ch1+ch2+ch3)/3;

    //CImg<double> rods = *(xyz2lms(rgb2xyz(input)));

    for (unsigned int i = 1; i < modules.size(); i++) {

        module* neuron = modules[i];
        int number_of_ports = neuron->getSizeID();

        // port search
        for (int o=0;o<number_of_ports;o++){

            vector <string> l = neuron->getID(o);
            vector <int> p = neuron->getOperation(o);

            //image input
            string cellName = l[0];

            if (cellName == "L_cones") {
                    // acc umulator=rods.get_channel(2);
                    accumulator=ch3;
            } else if (cellName == "M_cones") {
                    // accumulator=rods.get_channel(1);//ch2;
                    accumulator=ch2;
            } else if (cellName == "S_cones") {
                    // accumulator=rods.get_channel(0);//ch1;
                    accumulator=ch1;
            } else if (cellName == "rods") {
                    // accumulator=rods/3; // TODO: This is wrong. Should be the sum of channels / 3 (single channel image).
                    accumulator=rods;
            } else {

            // other inputs rather than cones or rods

                //search for the first image
                for (unsigned int m = 1; m < modules.size(); m++) {
                    module* n = modules[m];
                    if (l[0] == n->getModuleID()) {
                        accumulator = *(n->getOutput());
                        break;
                    }
                }


                //other operations
                for (unsigned int k = 1; k < l.size(); k++) {
                    for (unsigned int m = 1; m < modules.size(); m++) {
                        module* n = modules[m];
                        if (l[k] == n->getModuleID()) {
                           if (p[k-1]==0){
                                accumulator += *(n->getOutput());
                            }else{
                                accumulator -= *(n->getOutput());
                            }
                           break;
                        }
                    }

                }
            }

            if (neuron->getTypeSynapse(o)==0)
                neuron->feedInput(accumulator,true,o);
            else
                neuron->feedInput(accumulator,false,o);
        }
    }

    lastFedInput = *input;
    return input;
}

//------------------------------------------------------------------------------//

CImg<double> *Retina::feedInput(int step){
    CImg <double> *input;

    // Input selection
    switch(inputType){
    case 0:
        if (step/repetitions < numberImages)
            input = inputSeq[step/repetitions];
        else
            input = inputSeq[numberImages-1];

        break;

    case 1:

        input = updateGrating(step);

        break;

    case 2:

        input = updateNoise(step);

        break;

    case 3:

        input = updateImpulse(step);

        break;

    case 4:
        input = updateFixGrating(step);
        break;

    default:
        input = nullptr;
        cout << "Wrong input type!" << endl;
        break;
    }

    input = this->feedInput(input);
    lastFedInput = *(input);
    return input;
}


//------------------------------------------------------------------------------//

void Retina::update(){
    for (unsigned int i = 1; i < modules.size(); i++) {
        modules[i]->update();
    }

    double switchTime = 0;
    // Updating the multimeter only in the last step
    if (getWhiteNoise() != nullptr) {
        switchTime = getWhiteNoise()->getSwitchTime();
    }
    displayMg.updateDisplay(&lastFedInput, modules, switchTime, simCurrTime,
        simDuration, currentTrial, totalNumberTrials);
    simCurrTime += simStep;
}

//------------------------------------------------------------------------------//

void Retina::addModule(module* m, string ID){
    m->setModuleID(ID);
    modules.push_back(m);
    if(verbose)cout << "Module "<< m->getModuleID() << " added to the retina structure" << endl;
}

module* Retina::getModule(int ID){
    return modules[ID];
}

int Retina::getNumberModules(){
    return modules.size();
}

//------------------------------------------------------------------------------//


bool Retina::setInputSeq (string directory) {
    vector<string> result;

    if (!fs::exists(directory) || !fs::is_directory(directory)) return false;

    fs::directory_iterator end_iter;
    for(fs::directory_iterator dir_iter(directory); dir_iter != end_iter; ++dir_iter) {
        if (fs::is_regular_file(dir_iter->status())) {
            result.push_back(dir_iter->path().string());
        }
    }
    sort(begin(result), end(result));

    CImg <double> image(result[0].c_str());
    setColumns(image.height());
    setRows(image.width());

    numberImages = result.size();
    inputSeq = new CImg<double>*[numberImages];

    for (int i = 0; i < numberImages; i++) {
        inputSeq[i] = new CImg <double>(columns_, rows_, 1, 3, true);
        inputSeq[i]->assign(result[i].c_str());
    }

    if(verbose) cout << numberImages << " images read from "<< directory << endl;
    inputType = 0;

    return true;
}

//------------------------------------------------------------------------------//


int Retina::getNumberImages(){
    return numberImages;
}

//------------------------------------------------------------------------------//


bool Retina::connect(vector <string> from, string to, vector <int> operations, string type_synapse) {
    // valueToReturn is true if all the "from" modules are found
    bool valueToReturn = false;

    module* neuronto;

    if (to == "Output") {
        neuronto = modules[0];
        neuronto->addSourceIDs(from);
        neuronto->addOperation(operations);
        valueToReturn = true;
    } else {
        // Looking for the destination module
        for (unsigned int i = 1; i < modules.size() && !valueToReturn; i++){
            if (modules[i]->checkID(to)) {
                neuronto = modules[i];
                valueToReturn = true;
            }
        }

        // The destination module was not found
        if (!valueToReturn) return valueToReturn;

        // Iterate through source modules and look if all of them are existing
        for (const string &ff: from) {

            // These modules are embedded
            if (ff == "rods" || ff == "L_cones" || ff == "M_cones" || ff == "S_cones") { continue; }

            valueToReturn = false;
            for (unsigned int k = 1; k < modules.size() && !valueToReturn; k++) {
                if (modules[k]->checkID(ff)) {
                    valueToReturn = true;
                    if(verbose) cout << modules[k]->getModuleID() << " has been conected to "<< neuronto->getModuleID() << endl;
                }
            }
        }

        // Some of the source modules were not found
        if (!valueToReturn) return valueToReturn;

        neuronto->addSourceIDs(from);
        neuronto->addOperation(operations);

        int typeSyn = 0;

        if (type_synapse == "Current") {
            typeSyn = 0;
        } else if (type_synapse == "Conductance") {
            typeSyn = 1;
        } else {
            valueToReturn = false;
        }
        neuronto->addTypeSynapse(typeSyn);
    }

    return valueToReturn;
}


//------------------------------------------------------------------------------//

bool Retina::generateGrating(int type, double step, double lengthB, double length, double length2, int X, int Y, double freq, double T, double Lum, double Cont, double phi, double phi_t, double theta, double red, double green, double blue, double red_phi, double green_phi, double blue_phi) {

    bool valueToReturn = false;

    g = new GratingGenerator(type, step, lengthB, length, length2, X, Y, freq, T, Lum, Cont, phi, phi_t, theta,red,green,blue,red_phi, green_phi,blue_phi);

    setColumns(X);
    setRows(Y);
    valueToReturn=true;

    inputType = 1;

    return valueToReturn;
}

CImg <double>* Retina::updateGrating(double t){
    return g->compute_grating(t);
}

//------------------------------------------------------------------------------//


bool Retina::generateWhiteNoise(double mean, double contrast1,double contrast2, double period, double switchT,int X, int Y) {

    bool valueToReturn = false;

    WN = new whiteNoise(mean,contrast1,contrast2,period,switchT,X,Y);
    WN->initializeDist(currentTrial);

    setColumns(X);
    setRows(Y);
    valueToReturn=true;

    inputType = 2;

    return valueToReturn;
}

CImg<double>* Retina::updateNoise(double t){
    return WN->update(t);
}

whiteNoise* Retina::getWhiteNoise(){
    return WN;
}


//------------------------------------------------------------------------------//


bool Retina::generateImpulse(double start, double stop, double amplitude,double offset, int X, int Y) {

    bool valueToReturn = false;

    imp = new impulse(start,stop,amplitude,offset,X,Y);

    setColumns(X);
    setRows(Y);

    valueToReturn=true;

    inputType = 3;

    return valueToReturn;
}

CImg<double>* Retina::updateImpulse(double t){
    return imp->update(t);
}

//------------------------------------------------------------------------------//

bool Retina::generateFixationalMovGrating(int X, int Y, double radius, double jitter, double period, double step, double luminance, double contrast, double orientation, double red_weight, double green_weigh, double blue_weight, int type1, int type2, int ts) {

    bool valueToReturn = false;

    fg = new fixationalMovGrating(X, Y, radius, jitter, period, step, luminance, contrast, orientation, red_weight, green_weigh, blue_weight, type1, type2, ts);

    setColumns(X);
    setRows(Y);

    valueToReturn=true;

    inputType = 4;

    return valueToReturn;
}

CImg <double>* Retina::updateFixGrating(double t){
    return fg->compute_grating(t);
}

void Retina::loadCircuit(std::string config_path) {
    Py_Initialize();

    define_retina_python();

    fs::path retina_config_file(config_path);
    if (!fs::exists(retina_config_file)) {
        std::cerr << "Circuit file not found " << retina_config_file.string() << std::endl;
        std::cout << "Searching into generic retina folder." << std::endl;
        // default to retina file in models directory
        assert(getenv("NRP_MODELS_DIRECTORY") != nullptr);
        retina_config_file = (string)getenv("NRP_MODELS_DIRECTORY");
        retina_config_file /= "retina";
        retina_config_file /= config_path;
    }

    if (!fs::exists(retina_config_file.string())) {
      std::cerr << "Generic configuration not found " << retina_config_file.string() << std::endl;
      throw std::runtime_error("Retina config file not found.\nLooked for:\n"+
                               config_path+"\n"+
                               retina_config_file.string());
    }

    // interpreting the retina scripts as real python files instead of parsing them
    ifstream in(retina_config_file.string());
    string contents((istreambuf_iterator<char>(in)),
        istreambuf_iterator<char>());
    try {
        auto py__main__ = py::import("__main__");
        py__main__.attr("retina") = py::ptr(this);
        py::exec(contents.c_str(), py__main__.attr("__dict__"));
        this->allocateValues();
        Py_Finalize();
    } catch(py::error_already_set const &) {
        PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
        PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
        std::string ret("Unfetchable Python error");
        if(type_ptr != NULL){
            py::handle<> h_type(type_ptr);
            py::str type_pstr(h_type);
            py::extract<std::string> e_type_pstr(type_pstr);
            if(e_type_pstr.check())
                ret = e_type_pstr();
            else
                ret = "Unknown exception type";
        }
        std::cout << "Error in Python: " << ret << std::endl;

        std::string exception_msg = py::extract<std::string>(value_ptr);
        std::cout << exception_msg << std::endl;
        Py_Finalize();
        throw std::runtime_error("The retina configuration couldn't be loaded");
    }

}

double Retina::getValue(int cell, string layer) {
    int select_image = cell/(columns_*rows_);
    int pos_im = cell - select_image*(columns_*rows_);

    int row = pos_im / rows_;
    int col = pos_im % rows_;

    if (layer == "Output") {
        layer = modules[0]->getID(0)[0];
    }

    for (module* &m : modules) {
        if (m->getModuleID() == layer) {
            return (*m->getOutput())(col, row, 0, 0);
        }
    }

    throw std::runtime_error("Invalid layer");
}

void Retina::Py_TempStep(double step) {
    this->set_step(step);
};

void Retina::Py_SimTime(int sim_time) {
    this->setSimDuration(sim_time);
};

void Retina::Py_NumTrials(double ntrials) {
    if (ntrials > 0) {
        this->setSimTotalTrials(ntrials);
    }
    else {
        std::cout << "Error: ntrials <= 0" << std::endl;
        return;
    }
};

void Retina::Py_PixelsPerDegree(double ppd) {
    if (ppd > 0.0) {
        this->setPixelsPerDegree(ppd);
    }
    else {
        std::cout << "Error: ppd <= 0" << std::endl;
        return;
    }
};

void Retina::Py_NRepetitions(double repetitions) {
    if (repetitions > 0) {
        this->setRepetitions(repetitions);
    }
    else {
        std::cout << "Error: repetitions <= 0" << std::endl;
        return;
    }
};

void Retina::Py_DisplayDelay(int delay) {
    if (delay >= 0) {
        this->displayMg.setDelay(delay);
    }
    else {
        std::cout << "Error: delay < 0" << std::endl;
        return;
    }
};

void Retina::Py_DisplayZoom(double zoom) {
    if (zoom >= 0) {
        this->displayMg.setZoom(zoom);
    }
    else {
        std::cout << "Error: zoom < 0" << std::endl;
        return;
    }
};

void Retina::Py_DisplayWindows(int windows_per_row) {
    if (windows_per_row > 0) {
        this->displayMg.setImagesPerRow(windows_per_row);
    }
    else {
        std::cout << "Error: windows_per_row <= 0" << std::endl;
        return;
    }
};

void Retina::Py_Input(std::string input_type, py::dict args) {
    if (input_type == "sequence") {
        // TODO: check return value
        this->setInputSeq(py::extract<std::string>(args["path"]));
    }
    else if (input_type == "grating") {
        // TODO: check return value
        this->generateGrating(
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
        this->setRepetitions(1.0);
    }
    else if (input_type == "fixationalMovGrating") {
        // TODO: check return value
        this->generateFixationalMovGrating(
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
        this->setRepetitions(1.0);
    }
    else if (input_type == "whiteNoise") {
        // TODO: check return value
        this->generateWhiteNoise(
            py::extract<float>(args["mean"]),
            py::extract<float>(args["contrast1"]),
            py::extract<float>(args["contrast2"]),
            py::extract<float>(args["period"]),
            py::extract<float>(args["switch"]),
            py::extract<float>(args["sizeX"]),
            py::extract<float>(args["sizeY"])
        );
        this->setRepetitions(1.0);
    }
    else if (input_type == "impulse") {
        // TODO: check return value
        this->generateImpulse(
            py::extract<float>(args["start"]),
            py::extract<float>(args["stop"]),
            py::extract<float>(args["amplitude"]),
            py::extract<float>(args["offset"]),
            py::extract<float>(args["columns"]),
            py::extract<float>(args["rows"])
        );
        this->setRepetitions(1.0);
    }
    else if (input_type == "camera") {
        py::tuple size = py::extract<py::tuple>(args["size"]);
        this->setColumns(py::extract<int>(size[0]));
        this->setRows(py::extract<int>(size[1]));
    }
    else {
        std::cout << "Error: invalid input type" << std::endl;
    }
}

void Retina::Py_Create(std::string module_type, std::string module_id, py::dict args) {
    module* newModule;
    if (module_type == "LinearFilter") {
        newModule = new LinearFilter(this->getColumns(), this->getRows(), this->getSimStep(), 0.0);
    }
    else if (module_type == "SingleCompartment") {
        newModule = new SingleCompartment(this->getColumns(), this->getRows(), this->getSimStep());
    }
    else if (module_type == "StaticNonLinearity") {
        newModule = new StaticNonLinearity(this->getColumns(), this->getRows(), this->getSimStep(), 0);
    }
    else if (module_type == "CustomNonLinearity") {
        newModule = new StaticNonLinearity(this->getColumns(), this->getRows(), this->getSimStep(), 1);
    }
    else if (module_type == "SigmoidNonLinearity") {
        newModule = new StaticNonLinearity(this->getColumns(), this->getRows(), this->getSimStep(), 2);
    }
    else if (module_type == "ShortTermPlasticity") {
        newModule = new ShortTermPlasticity(this->getColumns(), this->getRows(), this->getSimStep(), 0.0, 0.0, 0.0, 0.0, false);
    }
    else if (module_type == "GaussFilter") {
        newModule = new GaussFilter(this->getColumns(), this->getRows(), this->getPixelsPerDegree());
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
    this->addModule(newModule, module_id);
}

void Retina::Py_Connect(py::object arg0, std::string connect_to, std::string type_synapse) {
    vector <int> operations;
    vector <string> modulesID;

    this->displayMg.allocateValues(this->getNumberModules(), this->getSimStep());
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
    this->connect(modulesID, connect_to, operations, type_synapse);
}

void Retina::Py_Show(std::string module_id, bool show, py::dict args) {
    for(int l = 1; l < this->getNumberModules(); l++){
        module *m = this->getModule(l);
        string ID = m->getModuleID();

        if (ID == module_id) {
            if (show) {
                this->displayMg.setIsShown(true, l);
                this->displayMg.setIsShown(true, 0);
            }
            else this->displayMg.setIsShown(false, l);
        }

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            this->displayMg.setMargin(margin, l);
        }
    }

    if (module_id == "Input") {
        if (show)
            this->displayMg.setIsShown(true, 0);
        else
            this->displayMg.setIsShown(false, 0);

        unsigned long margin = py::extract<unsigned long>(args["margin"]);
        if (margin > 0) {
            this->displayMg.setMargin(margin, 0);
        }
    }
}

void Retina::Py_Multimeter(std::string multimeter_type, std::string multimeter_id, std::string module_id, py::dict args, bool show) {
    std::string showstr;
    if (show) showstr = "True";
    else showstr = "False";

    std::transform(multimeter_type.begin(), multimeter_type.end(), multimeter_type.begin(), ::tolower);

    if (multimeter_type == "spatial") {
        int param1 = py::extract<int>(args["value"]);
        bool rowcol = py::extract<bool>(args["rowcol"]);

        if (!rowcol) param1 = -param1;

        this->displayMg.addMultimeterTempSpat(
            multimeter_id, module_id, param1, py::extract<int>(args["timeStep"]), false, showstr
        );
    }
    else if (multimeter_type == "temporal") {
        this->displayMg.addMultimeterTempSpat(
            multimeter_id, module_id, py::extract<int>(args["x"]), py::extract<int>(args["y"]), true, showstr
        );
    }
    else if (multimeter_type == "linear-nonlinear") {
        this->displayMg.addMultimeterLN(
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

CImg<double>* Retina::convertImage(const boost::python::api::object& img) {
    std::string objtype = py::extract<std::string>(img.attr("__class__").attr("__module__"));
    objtype += ".";
    objtype += py::extract<std::string>(img.attr("__class__").attr("__name__"));

    if (objtype == "numpy.ndarray")
        return convertndarray(img);
    else
        throw std::invalid_argument("Convertion from " + objtype + " is not supported.");

}

CImg<double>* Retina::convertndarray(const boost::python::api::object& img) {
    CImg<double>* ret = new CImg<double>(rows_, columns_, 3, 1, 0.0);

    Py_ssize_t bufLen;
    void const *buffer = NULL;
    py::object data = img.attr("data");
    bool isReadBuffer = !PyObject_AsReadBuffer(data.ptr(), &buffer, &bufLen);

    if (!isReadBuffer || bufLen != 3*columns_*rows_) {
        throw std::invalid_argument("Something is wrong with the image.");
    }

    unsigned char* buf = (unsigned char*)buffer;

    for (int i = 0; i < columns_; i++) {
        for (int j = 0; j < rows_; j++){
            (*ret)(i, j, 0, 0) = buf[3*(i+j*columns_)];
            (*ret)(i, j, 0, 1) = buf[3*(i+j*columns_)+1];
            (*ret)(i, j, 0, 2) = buf[3*(i+j*columns_)+2];
        }
    }

    return ret;
}

void Retina::Py_Update(const py::object& img) {
    CImg<double>* inputImg = this->convertImage(img);
    this->feedInput(inputImg);
    this->update();
}

double Retina::Py_GetValue(int row, int col, string layer) {
    // TODO: this is working but it's clearly wrong (should be either
    // row*columns_+col or col*rows_+row). Check
    return this->getValue(row*rows_+col, layer);
}

void define_retina_python() {
    py::class_<Retina>("Retina", py::init<>())
        .def("TempStep", &Retina::Py_TempStep)
        .def("SimTime", &Retina::Py_SimTime)
        .def("NumTrials", &Retina::Py_NumTrials)
        .def("PixelsPerDegree", &Retina::Py_PixelsPerDegree)
        .def("NRepetitions", &Retina::Py_NRepetitions)
        .def("DisplayDelay", &Retina::Py_DisplayDelay)
        .def("DisplayZoom", &Retina::Py_DisplayZoom)
        .def("DisplayWindows", &Retina::Py_DisplayWindows)
        .def("Input", &Retina::Py_Input)
        .def("Create", &Retina::Py_Create)
        .def("Connect", &Retina::Py_Connect)
        .def("Show", &Retina::Py_Show)
        .def("Multimeter", &Retina::Py_Multimeter)
        .def("Update", &Retina::Py_Update)
        .def("GetValue", &Retina::Py_GetValue);
}
