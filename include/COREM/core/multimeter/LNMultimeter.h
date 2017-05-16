#ifndef COREM_LN_MULTIMETER_H
#define COREM_LN_MULTIMETER_H

#include "COREM/core/multimeter.h"

class LNMultimeter : public Multimeter {
   public:
    LNMultimeter(unsigned int columns, unsigned int rows, retina_config_t* conf,
                 std::string id, unsigned int timeToShow, unsigned int x,
                 unsigned int y, double segment, double interval, double start,
                 double stop);

    virtual void show(int waitTime);
    virtual void showGP(unsigned int wat);
    virtual void record();
    virtual void dump(std::string mah);
    void showAvg(int waitTime);
    void showAvgGP();

   protected:
    unsigned int callCount;  // For calling showAvg

    unsigned int x;
    unsigned int y;
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
