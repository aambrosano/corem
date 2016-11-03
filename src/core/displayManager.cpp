#include <COREM/core/displayManager.h>

DisplayManager::DisplayManager(int x, int y) {
    columns_ = x;
    rows_ = y;
    last_row=0;
    last_col=0;
    imagesPerRow=0;

    displayZoom = 0;
    delay = 0;
    imagesPerRow=4;

    valuesAllocated = false;

}

void DisplayManager::setLNFile(const string &file, double ampl) {
    LNFile = file;
    LNfactor = ampl;
}

bool DisplayManager::hasLNFile() {
    return LNFile != "";
}

void DisplayManager::reset() {
    columns_ = 1;
    rows_ = 1;
    last_row=0;
    last_col=0;
    imagesPerRow=0;

    displayZoom = 0;
    delay = 0;
    imagesPerRow=4;

    valuesAllocated = false;

}

//------------------------------------------------------------------------------//

void DisplayManager::allocateValues(int number, double tstep) {

    if(valuesAllocated == false){

        simStep = tstep;
        numberModules = number;

#if cimg_display != 0 // Zoom factor has no meaning in case of no display
        // Readjusting zoom factor in case of oversizing
        if (displayZoom*(double)columns_ >= CImgDisplay::screen_width()/4){
            displayZoom = CImgDisplay::screen_width()/(4.0*(double)columns_);
            cout << "zoom has been readjusted to "<< displayZoom << endl;
        }
#endif

        for(int k=0;k<numberModules;k++){
            isShown.push_back(false);
        }

        for(int k=0;k<numberModules;k++){
            margin.push_back(0.0);
        }

        valuesAllocated = true;
    }

}

//------------------------------------------------------------------------------//


void DisplayManager::setColumns(int columns) {
    columns_ = columns;
}

void DisplayManager::setRows(int rows) {
    rows_ = rows;
}

void DisplayManager::setZoom(double zoom) {
    displayZoom = zoom;
}

void DisplayManager::setDelay(int displayDelay) {
    delay = displayDelay;
}

void DisplayManager::setImagesPerRow(int numberI){
    imagesPerRow = numberI;
}

void DisplayManager::setIsShown(bool value,int pos){

    isShown[pos]=value;
}

void DisplayManager::setMargin(double m, int pos){
    margin[pos]=m;
}


void DisplayManager::setSimStep(double value){
    simStep = value;

    cout << "Display simStep = "<< simStep << endl;
}

//------------------------------------------------------------------------------//

void DisplayManager::addMultimeter(string multimeterID, string moduleID, vector<int> &aux, MultimeterType mtype, bool shown) {
    cout << "Adding a multimeter (" << multimeterID << ") of type " << mtype << endl;
    multimeter* nm = new multimeter(columns_, rows_);
    nm->setSimStep(simStep);

    multimeters.push_back(nm);
    multimeterIDs.push_back(multimeterID);
    moduleIDs.push_back(moduleID);
    multimeterParam.push_back(aux);
    multimeterType.push_back(mtype);
    isShown.push_back(shown);
}

void DisplayManager::addMultimeterTempSpat(string multimeterID, string moduleID, int param1, int param2,bool temporalSpatial, string Show) {
    vector<int> aux;
    aux.push_back(param1);
    aux.push_back(param2);

    MultimeterType mtype;
    if(temporalSpatial) {
        mtype = Temporal;
    } else {
        mtype = Spatial;
    }

    cout << "temporalSpatial: " << temporalSpatial << " mtype: " << mtype << endl;

    addMultimeter(multimeterID, moduleID, aux, mtype, Show == "True");
}

void DisplayManager::addMultimeterLN(string multimeterID, string moduleID, int x, int y, double segment, double interval, double start, double stop, string Show) {
    vector<int> aux;
    aux.push_back(x);
    aux.push_back(y);

    LNSegment.push_back(segment);
    LNInterval.push_back(interval);
    LNStart.push_back(start);
    LNStop.push_back(stop);

    addMultimeter(multimeterID, moduleID, aux, LinearNonLinear, Show == "True");
}

void DisplayManager::modifyLN(string moduleID, double start, double stop) {
    for (unsigned int k = 0; k < multimeterIDs.size(); k++) {
        if (moduleID == multimeterIDs[k]) {
            LNStart[k] = start;
            LNStop[k] = stop;
            return;
        }
    }
}

//------------------------------------------------------------------------------//


void DisplayManager::addDisplay(int pos, string ID) {
#if cimg_display != 0
    // black image
    double newX = (double)columns_ * displayZoom;
    double newY = (double)rows_ * displayZoom;

    // Adding Input
    if (displays.size() == 0) {
        // create input display
        DisplayWithBar* inputDisplay = new DisplayWithBar("Input", newX, newY, false, 50, true);
        inputDisplay->move(0, 0);
        inputDisplay->setMargin(margin[0]);

        displays.push_back(inputDisplay);

        if (!isShown[0]) inputDisplay->hide();

    }

    DisplayWithBar* dwb = new DisplayWithBar(ID, newX, newY);
    dwb->setMargin(margin[pos]);

    if (isShown[pos]) {
        // new row of the display
        int capacity = int((CImgDisplay::screen_width()-newY-100) / (newY+50));

        if (last_col < capacity && last_col < imagesPerRow){
            last_col++;
        } else {
            last_col = 1;
            last_row++;
        }

        // move display
        dwb->move((int)last_col*(newY+80.0),(int)last_row*(newX+80.0));
    } else {
        dwb->hide();
    }

    // Save display
    displays.push_back(dwb);
#endif
}

//------------------------------------------------------------------------------//


void DisplayManager::updateDisplay(CImg <double> *input, vector<Module *> &modules, double switchTime,
                                   int simCurrTime, double simDuration, double numberTrials, double totalNumberTrials) {
#if cimg_display != 0
    double scaledCols = (double)columns_ * displayZoom;
    double scaledRows = (double)rows_ * displayZoom;

    // Display input
    if (isShown[0]) {
        displays[0]->update(input);
    }

    // show modules
    for (int k=0; k < numberModules - 1; k++) {
        if (isShown[k+1]) {
            displays[k+1]->update(modules[k+1]->getOutput());
        }
    }

    // update multimeters
    for (unsigned int i = 0; i < multimeters.size(); i++){
        multimeter *m = multimeters[i];
        Module *n = NULL;
        // find target module
        if(moduleIDs[i] != "Input") {
            for(unsigned int j = 1; j < modules.size(); j++){
                n = modules[j];
                if (*modules[j] == moduleIDs[i]) break;
            }
        }

        // temporal and LN mult.
        if(multimeterType[i] == Temporal || multimeterType[i] == LinearNonLinear) {
            vector<int> aux = multimeterParam[i];

            if(moduleIDs[i] == "Input") {
                m->recordValue((*input)(aux[0], aux[1], 0, 0));
            } else {
                m->recordValue((*n->getOutput())(aux[0], aux[1], 0, 0));
            }

            m->recordInput((*input)(aux[0], aux[1], 0, 0));
        }
        // spatial mult.
        else if(multimeterType[i] == Spatial) {
            vector <int> aux = multimeterParam[i];
            if(simCurrTime == aux[1]) {

                // set position
                int capacity = int((CImgDisplay::screen_width()-scaledCols-100) / (scaledCols+50));

                if (last_col<capacity && last_col < imagesPerRow){
                    last_col++;
                }else{
                    last_col = 1;
                    last_row++;
                }

                if (isShown[numberModules+i]) {

                    if (moduleIDs[i] == "Input") {

                        if(aux[0]>0)
                            m->showSpatialProfile((*input),true,aux[0],multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-1);
                        else
                            m->showSpatialProfile((*input),false,-aux[0],multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-1);

                    }else{
                        if(aux[0]>0)
                            m->showSpatialProfile((*n->getOutput()),true,aux[0],multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-1);
                        else
                            m->showSpatialProfile((*n->getOutput()),false,-aux[0],multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-1);

                    }
                }
            }
        }

    }

    // display temporal and LN multimeters for the last simulation step
    if (simCurrTime == simDuration - simStep) {

        int LNMultimeters = 0;
        for (unsigned int i = 0; i < multimeters.size(); i++) {
            multimeter *m = multimeters[i];

            // set position
            if(multimeterType[i] == Temporal || multimeterType[i] == LinearNonLinear){
                int capacity = int((CImgDisplay::screen_width()-scaledCols-100) / (scaledCols+50));

                if (last_col<capacity && last_col < imagesPerRow){
                    last_col++;
                }else{
                    last_col = 1;
                    last_row++;
                }
            }

            if(multimeterType[i] == Temporal){

                if(isShown[numberModules+i]==true){
                    if (i<multimeters.size()-1)
                        m->showTemporalProfile(multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),delay,LNFile);
                    else
                        m->showTemporalProfile(multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-1,LNFile);

                }else{
                    m->showTemporalProfile(multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-2,LNFile);
                }
            } else if(multimeterType[i] == LinearNonLinear) {
                if (isShown[numberModules+i] == false) {
                    m->showLNAnalysis(multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-2,LNSegment[LNMultimeters]/simStep,LNInterval[LNMultimeters]/simStep,LNStart[LNMultimeters]/simStep,LNStop[LNMultimeters]/simStep,totalNumberTrials,LNFile);
                } else {
                    if (i < multimeters.size() - 1)
                        m->showLNAnalysis(multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),delay,LNSegment[LNMultimeters]/simStep,LNInterval[LNMultimeters]/simStep,LNStart[LNMultimeters]/simStep,LNStop[LNMultimeters]/simStep,totalNumberTrials,LNFile);
                    else
                        m->showLNAnalysis(multimeterIDs[i],(int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-1,LNSegment[LNMultimeters]/simStep,LNInterval[LNMultimeters]/simStep,LNStart[LNMultimeters]/simStep,LNStop[LNMultimeters]/simStep,totalNumberTrials,LNFile);
                }

                // check last trial to show average results

                if(numberTrials == totalNumberTrials-1) {
                    m->getSwitchTime(switchTime);
                    if (isShown[numberModules+i] == false) {
                        m->showLNAnalysisAvg((int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),-2,LNSegment[LNMultimeters]/simStep,LNStart[LNMultimeters]/simStep, LNStop[LNMultimeters]/simStep,totalNumberTrials,LNFile,LNfactor);
                    }else{
                        m->showLNAnalysisAvg((int)last_col*(scaledRows+80.0),(int)last_row*(scaledCols+80.0),0,LNSegment[LNMultimeters]/simStep,LNStart[LNMultimeters]/simStep, LNStop[LNMultimeters]/simStep,totalNumberTrials,LNFile,LNfactor);
                    }
                }


                LNMultimeters++;
            }



        }


    }

    // Show displays if there's an input display
    if(isShown[0])
        displays[0]->wait(delay);
#endif
}

//------------------------------------------------------------------------------//
