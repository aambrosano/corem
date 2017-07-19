#ifndef COREM_LN_MULTIMETER_H
#define COREM_LN_MULTIMETER_H

#include <corem/multimeter.h>

class LNMultimeter : public Multimeter {
   public:
    LNMultimeter(int columns, int rows, retina_config_t* conf, std::string id,
                 int timeToShow, int x, int y, double segment, double interval,
                 double start, double stop);

    virtual void show(int waitTime);
    virtual void showGP(int wat);
    virtual void record();
    virtual void dump(std::string mah);
    void showAvg(int waitTime);
    void showAvgGP();

   protected:
    int callCount;  // For calling showAvg

    int x;
    int y;
    double segment;
    double interval;
    double start;
    double stop;
    std::vector<double> input_data;
    double* F;

    // Stores the retinaConfig.repetitions * ((stop - start) /
    // retinaConfig.timeStep) values for the input and the temporal input
    std::vector<double> historyInput;
    std::vector<double> historyTemporal;

   private:
    static void conj(double data[], double copy[], int NFFT);
    static void fft(double data[], int nn, int isign);
};

#endif
