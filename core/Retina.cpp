#include "Retina.h"

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

void Retina::setSimTotalRep(double r) {
    totalNumberTrials = r;
}

void Retina::setSimCurrentRep(double r) {
    CurrentTrial = r;
}

void Retina::setSimTimeTot(int t) {
    simDuration = t;
}

void Retina::setSimTime(int t) {
    simCurrTime = t;
}

double Retina::getSimCurrentRep() {
    return CurrentTrial;
}

double Retina::getSimTotalRep() {
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
        simDuration, CurrentTrial, totalNumberTrials);
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
    WN->initializeDist(CurrentTrial);

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

void Retina::loadCircuit(std::string retinaPath) {

    if (!fs::exists(retinaPath)) {
        cout << "Wrong retina file path." << endl;
        return;
    }

    // interpreting the retina scripts as real python files instead of parsing them
    ifstream in(retinaPath);
    string contents((istreambuf_iterator<char>(in)),
        istreambuf_iterator<char>());
    try {
        auto py__main__ = py::import("__main__");
        auto pyretina = py::import("pyretina");
        RetinaLoader* pr = new RetinaLoader(*this);
        py__main__.attr("retina") = py::ptr(pr);
        PyRun_SimpleString(contents.c_str());
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
