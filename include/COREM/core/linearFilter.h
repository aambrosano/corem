#ifndef COREM_LINEARFILTER_H
#define COREM_LINEARFILTER_H

/* BeginDocumentation
 * Name: LinearFilter
 *
 * Description: temporal recursive linear filtering:
 * y(k) =( -( a(1).y(k-1)+..+a(N).y(k-N) )  + b(0).x(k)+...+b(M-1).x(k-M+1) )/a(0)
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * Source code adapted from Virtual Retina [1]
 *
 * [1] Wohrer, Adrien, and Pierre Kornprobst. "Virtual Retina: a biological retina
 * model and simulator, with contrast gain control." Journal of computational
 * neuroscience 26.2 (2009): 219-249.
 *
 * SeeAlso: Module
 */

#ifdef DEBUG
#include <boost/chrono.hpp>

namespace bchrono = boost::chrono;
#endif

#include "module.h"

class EXPORT LinearFilter : public Module {
public:
    LinearFilter(std::string id, unsigned int columns, unsigned int rows, double temporalStep,
                 std::map<std::string, double> parameters);
    LinearFilter(const LinearFilter &copy);
    ~LinearFilter();

    virtual void update();

    virtual CImg<double>* getOutput();

    // Exponential and gamma filter
    void exponential(double tau);
    void gamma(double tau,int n);

    // Combinatorials of gamma function
    inline unsigned int arrangement(unsigned int n, unsigned int k);
    inline unsigned int factorial(unsigned int n);
    inline unsigned int combination(unsigned int n, unsigned int k);

protected:
    // filter parameters
    int M;
    int N;
    double* a;
    double* b;

    // recursion buffers
    CImg<double>** last_inputs;
    CImg<double>** last_values;
};

#endif // COREM_LINEARFILTER_H
