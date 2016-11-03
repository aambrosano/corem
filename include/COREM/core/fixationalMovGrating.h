#ifndef FIXATIONALMOVGRATING_H
#define FIXATIONALMOVGRATING_H

/* BeginDocumentation
 * Name: fixationalMovGrating
 *
 * Description: jittered grating used to reproduce motion adaptation [1,2]
 *
 * [1] Ölveczky, Bence P., Stephen A. Baccus, and Markus Meister. "Segregation
 * of object and background motion in the retina." Nature 423.6938 (2003): 401-408.
 *
 * [2] Ölveczky, Bence P., Stephen A. Baccus, and Markus Meister. "Retinal
 * adaptation to object motion." Neuron 56.4 (2007): 689-700.
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso: GratingGenerator, impulse, whiteNoise
 */

#include <CImg.h>

#include <boost/random.hpp>
#include <boost/chrono.hpp>

using namespace cimg_library;
using namespace std;

class fixationalMovGrating {
protected:

    // Grating parameters
    int sizeX;
    int sizeY;
    int type1,type2;
    int tswitch;
    double circle_radius;
    double jitter_period;
    double spatial_period;
    double step_size;
    double Lum;
    double Cont;
    double theta;
    double r,g,b;

    double cos_theta;
    double sin_theta;
    double A;

    int x0,y0;

    // Normal distributions for each interval (one for centre and one for periphery)
    boost::random::normal_distribution<double> distribution1;
    boost::random::normal_distribution<double> distribution2;

    boost::random::linear_congruential_engine<unsigned long, 16807, 0, 2147483647> generator1;
    boost::random::linear_congruential_engine<unsigned long, 16807, 0, 2147483647> generator2;
    boost::random::linear_congruential_engine<unsigned long, 16807, 0, 2147483647> generator3;
    boost::random::linear_congruential_engine<unsigned long, 16807, 0, 2147483647> generator4;

    // aux variables to update the grating
    CImg <double> aux;
    double Pi;
    double jitter1;
    double jitter2;
    double radius;
    double value1;
    double value2;
    double value3;
    double j1;
    double j2;

public:
    // Constructor, copy, destructor.
    fixationalMovGrating(int X=25,int Y=25,double radius=1.0,double jitter = 15.0,double period = 10.0,double step = 1.0,double luminance = 100.0,double contrast = 0.5,double orientation = 0.0,double red_weight=1.0,double green_weigh=1.0, double blue_weight=1.0,int t1 = 0,int t2 = 0,int ts = 0);

    // update the grating
    CImg<double> *compute_grating(double t);
};

#endif // FIXATIONALMOVGRATING_H
