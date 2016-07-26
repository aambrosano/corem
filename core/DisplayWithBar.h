#ifndef DISPLAYWITHBAR_H
#define DISPLAYWITHBAR_H

#include "../CImg-1.6.0_rolling141127/CImg.h"
#include <string>

#include "constants.h"

using namespace std;
using namespace cimg_library;

class DisplayWithBar {
private:
    CImgDisplay display;
    CImg<double>* bar;

    int columns_, rows_;
    int margin_;
    bool withBar;
    int barSize;
    bool trueColor;

    double findMin(CImg<double> *input);
    double findMax(CImg<double> *input);
    void updateBar(double min, double max);

public:
    DisplayWithBar(string name, int columns, int rows, bool withBar=true, int barSize=50, bool trueColor=false);

    void setMargin(int margin) { margin_ = margin; }

    void update(CImg<double> *input);
    void hide() { display.close(); }
    void show() { display.show(); }
    void move(int x, int y) { display.move(x, y); }
    void wait(unsigned int delay) { display.wait(delay); }
};

#endif // DISPLAYWITHBAR_H
