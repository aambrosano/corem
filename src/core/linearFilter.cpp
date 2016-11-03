#include <COREM/core/linearFilter.h>
#include <cassert>

LinearFilter::LinearFilter(std::string id, unsigned int columns, unsigned int rows,
                           double temporalStep, std::map<std::string, double> parameters)
    : Module(id, columns, rows, temporalStep, parameters) {

    double tau = 0.0;
    int n = 0;
    int type = 0;

    for(std::map<std::string, double>::const_iterator entry = parameters_.begin();
        entry != parameters_.end(); ++entry) {
        if (entry->first == "tau") tau = entry->second;
        else if (entry->first == "n") n = (int)entry->second;
        else if (entry->first == "Exp") type = 0;
        else if (entry->first == "Gamma") type = 1;
        else std::cerr << "LinearFilter(): You used an unrecognized parameter." << std::endl;
    }

    assert(type == 0 || type == 1);
    (!type) ? exponential(tau) : gamma(tau, n);

    last_inputs = new CImg<double>*[M];
    last_values = new CImg<double>*[N+1];

    // Note: The initial_value parameter used to initialize last_inputs from 1 to M
    // was always set to 0, so it was removed
    for (int i = 0; i < M; i++)
        last_inputs[i] = new CImg<double>(columns, rows, 1, 1, 0.0);
    for (int j = 0; j < N+1;j++)
        last_values[j] = new CImg<double>(columns, rows, 1, 1, 0.0);
}

LinearFilter::~LinearFilter() {
    delete[] a;
    delete[] b;
    for (int i = 0; i < M; i++) delete last_inputs[i];
    delete[] last_inputs;
    for (int j = 0; j < N+1; j++) delete last_values[j];
    delete[] last_values;
}

unsigned int LinearFilter::arrangement(unsigned int n, unsigned int k) {
    int res = 1;
    for (unsigned int i = n; i > n-k; --i)
        res *= i;

    return res;
}

inline unsigned int LinearFilter::factorial(unsigned int n) {
    return arrangement(n, n);
}

inline unsigned int LinearFilter::combination(unsigned int n, unsigned int k) {
    return arrangement(n, k)/factorial(k);
}

void LinearFilter::exponential(double tau) {
    if (tau > 0) {
        M = 1;
        N = 1;
        a = new double[N+1];
        b = new double[M];
        a[0] = 1;
        a[1] = -std::exp(-temporalStep_/tau);
        b[0] = 1 - std::exp(-temporalStep_/tau);
    }
    else std::cerr << "LinearFilter::exponential: invalid value for tau, not initializing." << std::endl;
}

void LinearFilter::gamma(double tau, int n) {
    if (tau > 0 && n >= 0) {
       M = 1;
       N = n + 1;
       double tauC = (n) ? tau/n : tau;
       double c = std::exp(-temporalStep_/tauC);

       b = new double[1];
       b[0] = std::pow(1-c,N);
       a = new double[N+1];

       for(int i = 0; i <= N; ++i)
           a[i] = pow(-c,i)*combination(N,i);
    }
    else std::cerr << "LinearFilter::gamma: invalid value for tau or n, not initializing." << std::endl;
}

void LinearFilter::update() {
#ifdef DEBUG
    bchrono::time_point<bchrono::system_clock> start = bchrono::system_clock::now();
#endif
    // The module is not connected
    if (this->source_ports.size() == 0)
        return;

    *(last_inputs[0]) = this->source_ports[0].getData();
    // Rotation on addresses of the last_values.
    CImg<double>* fakepoint = last_values[N]; // ??
    for(int i = 1; i < N+1; ++i)
        last_values[N+1-i] = last_values[N-i];
    last_values[0] = fakepoint;

    // Calculating new value of filter recursively:
    *(last_values[0]) = b[0]* (*(last_inputs[0]));
    for(int j = 1; j < M; j++)
        *(last_values[0]) += ( b[j] * (*(last_inputs[j])) );
    for(int k = 1; k < N + 1; k++)
        *(last_values[0]) -= ( a[k] * (*(last_values[k])) );
    if(a[0] != 1)
        ( *(last_values[0]) ) /= a[0];

    if (M > 1) {
        for(int i = 1; i < M; ++i)
            last_inputs[M-i] = last_inputs[M-i-1];
    }

    // last_inputs[0]->fill(0.0);

#ifdef DEBUG
    bchrono::duration<double> diff = bchrono::system_clock::now()-start;
    constants::timesLinear.push_back(diff.count());
#endif

}

CImg<double>* LinearFilter::getOutput() {
    return last_values[0];
}
