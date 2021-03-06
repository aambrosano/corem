#include <corem/multimeter/LNMultimeter.h>

LNMultimeter::LNMultimeter(int columns, int rows, retina_config_t *conf,
                           std::string id, int timeToShow, int x, int y,
                           double segment, double interval, double start,
                           double stop)
    : Multimeter(columns, rows, conf, id, timeToShow),
      callCount(0),
      x(x),
      y(y),
      segment(segment),
      interval(interval),
      start(start),
      stop(stop) {
    // To use with a different implementation of record
    /* this->input_data.assign((stop - start) / config->simulationStep(),
    0.0);
    this->data.assign((stop - start) / config->simulationStep(), 0.0); */
    //    this->historyInput.assign(this->data.size() * config->numTrials(),
    //    0.0);
    //    this->historyTemporal.assign(this->data.size() *
    //    config->numTrials(),
    //                                 0.0);
}

void LNMultimeter::record() {
    this->input_data.push_back(this->source_image->operator()(x, y, 0, 0));
    this->data.push_back(
        this->source_module->getOutput()->operator()(x, y, 0, 0));
}

void LNMultimeter::show(int waitTime) {
    callCount++;
    std::cout << callCount << std::endl;
    std::cout << config->numTrials << std::endl;
    if (callCount == config->numTrials) {
        this->showAvg(waitTime);
        return;
    }
    // normalize vectors
    std::vector<double> newInput(this->data.size(), 0.0);
    std::vector<double> newTemporal(this->data.size(), 0.0);

    historyInput.clear();
    historyTemporal.clear();

    double mean_value1 = 0.0;
    double mean_value2 = 0.0;

    for (unsigned int k = 0; k < this->input_data.size(); k++) {
        mean_value1 += this->input_data[k];
        mean_value2 += this->data[k];
    }

    mean_value1 /= this->input_data.size();
    mean_value2 /= this->data.size();

    for (unsigned int k = (this->data.size() / 100) + 1;
         k < this->input_data.size(); k++) {
        newInput[k] = this->input_data[k] - mean_value1;
        newTemporal[k] = this->data[k] - mean_value2;

        if (k * config->simulationStep >= start &&
            k * config->simulationStep < stop) {
            historyInput.push_back(this->input_data[k] - mean_value1);
            historyTemporal.push_back(this->data[k]);
        }
    }

    //    std::string LNFile = this->id + ".txt";
    //    // save seq
    //    std::string seqFile1 = "inp" + LNFile;
    //    std::string seqFile2 = "tmp" + LNFile;

    //    saveSeq(historyInput, seqFile1, (stop - start) * 2 * simStep);
    //    saveSeq(historyTemporal, seqFile2, (stop - start) * 2 * simStep);

    // calculate NFFT as the next higher power of 2 >= Nx.
    int NFFT = (int)pow(2.0, ceil(log((double)segment) / log(2.0)));

    // allocate memory for NFFT complex numbers
    double *R = (double *)malloc((2 * NFFT + 1) * sizeof(double));
    double *S = (double *)malloc((2 * NFFT + 1) * sizeof(double));
    double *S_conj = (double *)malloc((2 * NFFT + 1) * sizeof(double));
    double *numerator = (double *)malloc((2 * NFFT + 1) * sizeof(double));
    double *denominator = (double *)malloc((2 * NFFT + 1) * sizeof(double));
    //    double *F = (double *)malloc((2 * NFFT + 1) * sizeof(double));
    F = (double *)malloc((2 * NFFT + 1) * sizeof(double));

    std::fill_n(numerator, 2 * NFFT + 1, 0.0);
    std::fill_n(denominator, 2 * NFFT + 1, 0.0);
    std::fill_n(F, 2 * NFFT + 1, 0.0);

    int number_rep = int((stop - start) / interval);
    for (int rep = 0; rep < number_rep; rep++) {
        // Storing vectors in a complex array. This is needed even though x(n)
        // is purely real in this case.

        // Zeroing R, S and S_conj
        std::fill_n(R, 2 * NFFT + 1, 0.0);
        std::fill_n(S, 2 * NFFT + 1, 0.0);
        std::fill_n(S_conj, 2 * NFFT + 1, 0.0);

        for (int i = 0; i < segment; i++) {
            // Setting only the real part as the immaginary one is 0
            R[2 * i + 1] = newTemporal[start + rep * interval + i];
            S[2 * i + 1] = newInput[start + rep * interval + i];
        }

        // FFT
        fft(R, NFFT, 1);
        fft(S, NFFT, 1);
        conj(S, S_conj, NFFT);

        // numerator = \sum_(reps) S^ * R
        // denominator = \sum_(reps) S^ * S
        for (int i = 0; i < NFFT; i++) {
            numerator[2 * i + 1] += (S_conj[2 * i + 1] * R[2 * i + 1]) -
                                    (S_conj[2 * i + 2] * R[2 * i + 2]);
            numerator[2 * i + 2] += (S_conj[2 * i + 1] * R[2 * i + 2]) +
                                    (S_conj[2 * i + 2] * R[2 * i + 1]);

            denominator[2 * i + 1] += (S_conj[2 * i + 1] * S[2 * i + 1]) -
                                      (S_conj[2 * i + 2] * S[2 * i + 2]);
            denominator[2 * i + 2] += (S_conj[2 * i + 1] * S[2 * i + 2]) +
                                      (S_conj[2 * i + 2] * S[2 * i + 1]);
        }
    }

    for (int i = 0; i < NFFT; i++) {
        numerator[2 * i + 1] /= number_rep;
        numerator[2 * i + 2] /= number_rep;
        denominator[2 * i + 1] /= number_rep;
        denominator[2 * i + 2] /= number_rep;

        F[2 * i + 1] =
            ((numerator[2 * i + 1] * denominator[2 * i + 1]) +
             (numerator[2 * i + 2] * denominator[2 * i + 2])) /
            ((denominator[2 * i + 1]) * (denominator[2 * i + 1]) +
             (denominator[2 * i + 2]) * (denominator[2 * i + 2]) + DBL_EPSILON);
        F[2 * i + 2] =
            ((numerator[2 * i + 2] * denominator[2 * i + 1]) -
             (numerator[2 * i + 1] * denominator[2 * i + 2])) /
            ((denominator[2 * i + 1]) * (denominator[2 * i + 1]) +
             (denominator[2 * i + 2]) * (denominator[2 * i + 2]) + DBL_EPSILON);
    }

    // iFFT of F
    fft(F, NFFT, -1);

    // normalize the IFFT of F
    for (int i = 0; i < NFFT; i++) {
        F[2 * i + 1] /= NFFT;
        F[2 * i + 2] /= NFFT;
    }

    //    saveArray(F, int(2 * NFFT + 1), LNFile);

    // code for parameter waitTime:
    // waitTime = -2 -> display is not shown
    // waitTime = -1 -> display is shown till the user close it
    // waitTime >= 0 -> display is shown a duration waitTime

    int numberTrials = 1;  // This isn't used anyway
    if (waitTime > -2 && numberTrials == 1) {
        //        CImg <double> *temporalProfile = new CImg
        //        <double>(segment,1,1,1,0);

        //        double max_value1 = 0;
        //        double min_value1 = 0;

        //        for(int k=0;k<segment;k++){
        //            (*temporalProfile)(k,0,0,0)=F[2*k+1];

        //            if (F[2*k+1]>max_value1)
        //                max_value1 = F[2*k+1];
        //            if (F[2*k+1]<min_value1)
        //                min_value1 = F[2*k+1];
        //        }

        //        const unsigned char color1[] = {255,0,0};
        //        const unsigned char color2[] = {0,0,255};
        //        unsigned char back_color[] = {255,255,255};
        //        CImg <unsigned char> *profile = new CImg <unsigned
        //        char>(400,256,1,3,0);
        //        profile->fill(*back_color);
        //        const char * titleChar = (title).c_str();
        //        CImgDisplay *draw_disp = new CImgDisplay(*profile,titleChar);

        //        profile->draw_graph(temporalProfile->get_crop(0,0,0,0,segment-1,0,0,0)*255/(max_value1-min_value1)
        //        - min_value1*255/(max_value1-min_value1),color1,1,1,4,255,0);
        //        profile->draw_axes(0.0,segment*simStep,max_value1,min_value1,color2,1,-80,-80,0,0,~0U,~0U,20).display(*draw_disp);

        //        // move display
        //        draw_disp->move(col,row);

        //        // wait for the user closes display
        //        if(waitTime == -1){
        //            while (!draw_disp->is_closed()) {
        //                draw_disp->wait();
        //            }
        //        }
    }
}

void LNMultimeter::showGP(int wat) {
    UNUSED(wat);  // TODO: implement multimeter
}
void LNMultimeter::showAvgGP() {}
void LNMultimeter::dump(std::string) {}

void LNMultimeter::showAvg(int waitTime) {
    // normalize input
    double mean_value1 = 0;

    for (unsigned int k = 0; k < this->input_data.size(); k++) {
        mean_value1 += this->input_data[k];
    }

    mean_value1 /= this->input_data.size();

    for (unsigned int k = 0; k < this->input_data.size(); k++) {
        this->input_data[k] = (this->input_data[k] - mean_value1);
    }

    // read values from file
    //    fs::path to_file = getExecutablePath() / "results" / LNFile;

    //    std::ifstream fin;
    //    fin.open(to_file.string().c_str());
    //    std::vector<double> F;

    //    while (!fin.eof()) {
    //        char buf[1000];
    //        fin.getline(buf, 1000);
    //        const char *token[1] = {};
    //        token[0] = strtok(buf, "\n");

    //        F.push_back(atof(token[0]));
    //    }

    //    fin.close();

    // Non-linearity: The stimulus is convolved with the filter.
    // g = S*F

    //    string seqFile1 = "inp" + LNFile;
    //    string seqFile2 = "tmp" + LNFile;

    //    std::vector<double> historyInput = readSeq(seqFile1);
    //    std::vector<double> historyTemporal = readSeq(seqFile2);
    int length = historyInput.size();

    // create arrays to store FFT of G, S and F

    int NFFT = (int)pow(2.0, ceil(log((double)segment) / log(2.0)));
    int newNFFT = (int)pow(2.0, ceil(log((double)length) / log(2.0)));

    double *g = (double *)malloc((2 * newNFFT + 1) * sizeof(double));
    double *newS = (double *)malloc((2 * newNFFT + 1) * sizeof(double));
    double *newF = (double *)malloc((2 * newNFFT + 1) * sizeof(double));

    std::fill_n(g, 2 * newNFFT + 1, 0.0);
    std::fill_n(newS, 2 * newNFFT + 1, 0.0);
    std::fill_n(newF, 2 * newNFFT + 1, 0.0);

    for (int i = 0; i < length; i++) {
        newS[2 * i + 1] = historyInput[i];
    }

    for (int i = 0; i < NFFT; i++) {
        newF[2 * i + 1] = (F[2 * i + 1] / config->numTrials);
        newF[2 * i + 2] = (F[2 * i + 2] / config->numTrials);
    }

    // FFT
    fft(newF, newNFFT, 1);
    fft(newS, newNFFT, 1);

    // Convolution
    for (int i = 0; i < newNFFT; i++) {
        g[2 * i + 1] = (newF[2 * i + 1] * newS[2 * i + 1]) -
                       (newF[2 * i + 2] * newS[2 * i + 2]);
        g[2 * i + 2] = (newF[2 * i + 1] * newS[2 * i + 2]) +
                       (newF[2 * i + 2] * newS[2 * i + 1]);
    }

    // iFFT of g
    fft(g, newNFFT, -1);

    // normalize the IFFT of g
    for (int i = 0; i < newNFFT; i++) {
        g[2 * i + 1] /= newNFFT;
        g[2 * i + 2] /= newNFFT;
    }

    // Normalize g: the variance of the filtered stimulus is equal to
    // the variance of the stimulus (Baccus 2002)

    double varianceG = 0;
    double varianceS = 0;

    //    for (int k=0;k<newNFFT;k++) {
    //        if((k+start)<(start+length))
    //            varianceS += input[k+start]*input[k+start];
    //         varianceG += g[2*k+1]*g[2*k+1];
    //    }

    for (int k = 0; k < newNFFT; k++) {
        if (k < length) varianceS += historyInput[k] * historyInput[k];
        varianceG += g[2 * k + 1] * g[2 * k + 1];
    }

    varianceS /= length;
    varianceG /= newNFFT;

    for (int k = 0; k < NFFT; k++)
        F[2 * k + 1] *=
            (sqrt(varianceS) / sqrt(varianceG)) * constants::outputfactor;

    for (int k = 0; k < newNFFT; k++)
        g[2 * k + 1] *= (sqrt(varianceS) / sqrt(varianceG));

    // Plot of F
    segment = int(segment / 3);
    CImg<double> *temporalProfile = new CImg<double>(segment, 1, 1, 1, 0);

    double max_value1 = 0;
    double min_value1 = 0;

    double max_valuex = 0;
    double max_valuey = -DBL_INF;
    double min_valuex = 0;
    double min_valuey = DBL_INF;

    double *auxF = new double[int(segment)];

    for (int k = 0; k < segment; k++) {
        (*temporalProfile)(k, 0, 0, 0) = F[2 * k + 1] / config->numTrials;
        auxF[k] = F[2 * k + 1] / config->numTrials;
        if (F[2 * k + 1] / config->numTrials > max_value1)
            max_value1 = F[2 * k + 1] / config->numTrials;
        if (F[2 * k + 1] / config->numTrials < min_value1)
            min_value1 = F[2 * k + 1] / config->numTrials;
    }

    // remove LN file and save new file
    //    removeResultsFile(LNFile);
    //    saveArray(auxF, int(segment), LNFile);

    delete auxF;

    // discard the beginning of the sequence to plot g
    // int begin = start;
    // if(length > 300)
    //     begin = start+200;

    // Plot of the response vs g

    max_valuex = sqrt(varianceS);
    min_valuex = -sqrt(varianceS);

    CImg<double> *staticProfile = new CImg<double>(400, 1, 1, 1, 0);
    double numberPoints[400];
    for (int l = 0; l < 400; l++) numberPoints[l] = 0;

    //    for(int k=begin;k<(start+length);k++){
    //        if(abs(g[2*(k-(int)start)+1]) < sqrt(varianceS)){
    //            double value1 =
    //            (g[2*(k-(int)start)+1]-min_valuex)*(399/(max_valuex-min_valuex));
    //            (*staticProfile)(int(value1),0,0,0)+=(k/((start+length)))*temporal[k];
    //            numberPoints[int(value1)]+=(k/((start+length)));
    ////            (*staticProfile)(int(value1),0,0,0)=temporal[k];
    //        }
    //    }

    for (int k = 0; k < length; k++) {
        if (abs(g[2 * k + 1]) < sqrt(varianceS)) {
            double value1 =
                (g[2 * k + 1] - min_valuex) * (399 / (max_valuex - min_valuex));
            (*staticProfile)(int(value1), 0, 0, 0) += historyTemporal[k];
            numberPoints[int(value1)] += 1;
            //            (*staticProfile)(int(value1),0,0,0)=temporal[k];
        }
    }

    double auxSP[400];
    double auxSP2[400];

    for (int l = 0; l < 400; l++) {
        if (numberPoints[l] > 0)
            (*staticProfile)(l, 0, 0, 0) /= numberPoints[l];
        auxSP[l] = (*staticProfile)(l, 0, 0, 0);

        if ((*staticProfile)(l, 0, 0, 0) < min_valuey &&
            abs((*staticProfile)(l, 0, 0, 0)) > 0)
            min_valuey = (*staticProfile)(l, 0, 0, 0);
        if ((*staticProfile)(l, 0, 0, 0) > max_valuey &&
            abs((*staticProfile)(l, 0, 0, 0)) > 0)
            max_valuey = (*staticProfile)(l, 0, 0, 0);
    }

    (*staticProfile)(399, 0, 0, 0) = (*staticProfile)(398, 0, 0, 0);

    for (int l = 0; l < 400; l++) {
        auxSP2[l] = (l - 200) * ((max_valuex - min_valuex) / 399);
    }

    // Simple average of points
    int bin_size = 100;  // even number
    CImg<double> *newStaticProfile = new CImg<double>(400, 1, 1, 1, 0);

    for (int l = 0; l < 400; l++) {
        double accumulator = 0;

        if (l >= bin_size / 2 && l < 400 - bin_size / 2) {
            for (int k = 1; k < bin_size / 2 + 1; k++)
                accumulator += (*staticProfile)(l - k, 0, 0, 0);
            for (int k = 0; k < bin_size / 2; k++)
                accumulator += (*staticProfile)(l + k, 0, 0, 0);

            (*newStaticProfile)(l, 0, 0, 0) = accumulator / bin_size;
        } else if (l < bin_size / 2) {
            for (int k = 0; k < l + bin_size / 2; k++)
                accumulator += (*staticProfile)(k, 0, 0, 0);

            (*newStaticProfile)(l, 0, 0, 0) = accumulator / (l + bin_size / 2);
        } else {
            for (int k = l - bin_size / 2; k < 400; k++)
                accumulator += (*staticProfile)(k, 0, 0, 0);

            (*newStaticProfile)(l, 0, 0, 0) =
                accumulator / (400 - l + bin_size / 2);
        }
    }

    for (int l = 0; l < 400; l++) {
        (*staticProfile)(l, 0, 0, 0) = (*newStaticProfile)(l, 0, 0, 0);
        auxSP[l] = (*staticProfile)(l, 0, 0, 0);
    }

    // Remove and save new static NL
    //    std::string f1 = "SNL" + LNFile;
    //    std::string f2 = "SNL2" + LNFile;

    //    removeResultsFile(f1);
    //    removeResultsFile(f2);
    //    saveArray(auxSP, 400, f1);
    //    saveArray(auxSP2, 400, f2);

    // display results

    // code for parameter waitTime:
    // waitTime = -2 -> display is not shown

    if (waitTime > -2) {
        const unsigned char color1[] = {255, 0, 0};
        const unsigned char color2[] = {0, 0, 255};
        unsigned char back_color[] = {255, 255, 255};
        CImg<unsigned char> *profile =
            new CImg<unsigned char>(400, 256, 1, 3, 0);
        CImg<unsigned char> *nonlinearity =
            new CImg<unsigned char>(400, 256, 1, 3, 0);
        profile->fill(*back_color);
        nonlinearity->fill(*back_color);

        CImgDisplay *draw_disp =
            new CImgDisplay((*profile, *nonlinearity), "LN analysis averaged");
        profile->draw_graph(
            temporalProfile->get_crop(0, 0, 0, 0, segment - 1, 0, 0, 0) * 255 /
                    (max_value1 - min_value1) -
                min_value1 * 255 / (max_value1 - min_value1),
            color1, 1, 1, 4, 255, 0);
        nonlinearity->draw_graph(
            staticProfile->get_crop(0, 0, 0, 0, 400 - 1, 0, 0, 0) * 255 /
                    (max_valuey - min_valuey) -
                min_valuey * 255 / (max_valuey - min_valuey),
            color1, 1, 1, 4, 255, 0);
        profile->draw_text(320, 200, "time (ms)", color2, back_color, 1, 20)
            .display(*draw_disp);
        profile->draw_text(40, 5, "Linear Filter", color2, back_color, 1, 20)
            .display(*draw_disp);
        nonlinearity->draw_text(320, 200, "Input", color2, back_color, 1, 20)
            .display(*draw_disp);
        nonlinearity
            ->draw_text(10, 5, "Static NonLinearity", color2, back_color, 1, 20)
            .display(*draw_disp);

        (profile->draw_axes(0, segment * config->simulationStep, max_value1,
                            min_value1, color2, 1, -80, -80, 0, 0, ~0U, ~0U,
                            20),
         nonlinearity->draw_axes(min_valuex, max_valuex, max_valuey, min_valuey,
                                 color2, 1, -40, -20, 0, 0, ~0U, ~0U, 13))
            .display(*draw_disp);

        // move display
        // draw_disp->move(col, row);

        // wait for the user closes display
        while (!draw_disp->is_closed()) {
            draw_disp->wait();
        }
    }
}

void LNMultimeter::fft(double data[], int nn, int isign) {
    /*
     FFT/IFFT routine. (see pages 507-508 of Numerical Recipes in C)

     Inputs:
        data[] : array of complex* data points of size 2*NFFT+1.
            data[0] is unused,
            * the n'th complex number x(n), for 0 <= n <= length(x)-1, is
     stored
     as:
                data[2*n+1] = real(x(n))
                data[2*n+2] = imag(x(n))
            if length(Nx) < NFFT, the remainder of the array must be padded
     with
     zeros

        nn : FFT order NFFT. This MUST be a power of 2 and >= length(x).
        isign:  if set to 1,
                    computes the forward FFT
                if set to -1,
                    computes Inverse FFT - in this case the output values
     have
                    to be manually normalized by multiplying with 1/NFFT.
     Outputs:
        data[] : The FFT or IFFT results are stored in data, overwriting the
     input.
    */

    int n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            tempr = data[j];
            data[j] = data[i];
            data[i] = tempr;
            tempr = data[j + 1];
            data[j + 1] = data[i + 1];
            data[i + 1] = tempr;
        }
        m = n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax) {
        istep = 2 * mmax;
        theta = TWOPI / (isign * mmax);
        wtemp = sin(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m; i <= n; i += istep) {
                j = i + mmax;
                tempr = wr * data[j] - wi * data[j + 1];
                tempi = wr * data[j + 1] + wi * data[j];
                data[j] = data[i] - tempr;
                data[j + 1] = data[i + 1] - tempi;
                data[i] += tempr;
                data[i + 1] += tempi;
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }
}

//------------------------------------------------------------------------------//

void LNMultimeter::conj(double data[], double copy[], int NFFT) {
    for (int i = 0; i < NFFT; i++) {
        copy[2 * i + 1] = data[2 * i + 1];
        copy[2 * i + 2] = -data[2 * i + 2];
    }
}
