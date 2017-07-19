#include <corem/module/linearFilter.h>
#include <cassert>

LinearFilter::LinearFilter(std::string id, retina_config_t *conf, double tau,
                           double n, std::string type)
    : Module(id, conf) {
    double _tau = tau;
    double _n = n;
    int _type = -1;

    if (type == "Exp") {
        _type = 0;
    } else if (type == "Gamma") {
        _type = 1;
    } else {
        std::cerr << "LinearFilter(): invalid value \"" + type +
                         "\" for param type."
                  << std::endl;
        throw std::runtime_error("LinearFilter(): invalid value \"" + type +
                                 "\" for param type.");
    }

    // This sets M, N, a and b
    (!_type) ? exponential(_tau) : gamma(_tau, _n);

    last_inputs = new CImg<double> *[M];
    last_values = new CImg<double> *[N + 1];

    // Note: The initial_value parameter used to initialize last_inputs from 1
    // to M was always set to 0, so it was removed
    // TODO: why 0.1?
    for (int i = 0; i < M; i++) {
        last_inputs[i] = new CImg<double>(columns, rows, 1, 1, 0.1);
    }
    for (int j = 0; j < N + 1; j++) {
        last_values[j] = new CImg<double>(columns, rows, 1, 1, 0.1);
    }
}

LinearFilter::~LinearFilter() {
    delete[] a;
    delete[] b;
    for (int i = 0; i < M; i++) {
        delete last_inputs[i];
    }
    delete[] last_inputs;
    for (int j = 0; j < N + 1; j++) {
        delete last_values[j];
    }
    delete[] last_values;
}

unsigned int LinearFilter::arrangement(int n, int k) {
    assert(n >= k);
    int res = 1;
    for (int i = n; i > n - k; --i) {
        res *= i;
    }

    return res;
}

inline unsigned int LinearFilter::factorial(int n) { return arrangement(n, n); }

inline unsigned int LinearFilter::combination(int n, int k) {
    return arrangement(n, k) / factorial(k);
}

void LinearFilter::exponential(double tau) {
    if (tau > 0) {
        M = 1;
        N = 1;
        a = new double[N + 1];
        b = new double[M];
        a[0] = 1;
        a[1] = -std::exp(-config->simulationStep / tau);
        b[0] = 1 - std::exp(-config->simulationStep / tau);
    } else {
        std::cerr << "LinearFilter::exponential: invalid value for tau, not "
                     "initializing."
                  << std::endl;
        throw std::runtime_error(
            "LinearFilter::exponential: invalid value for tau, not "
            "initializing.");
    }
}

void LinearFilter::gamma(double tau, double n) {
    if (tau > 0 && n >= 0) {
        M = 1;
        // std::ceil(n) ?
        N = n + 1;
        double tauC = (n) ? tau / n : tau;
        double c = std::exp(-config->simulationStep / tauC);

        b = new double[1];
        b[0] = std::pow(1 - c, N);
        a = new double[N + 1];

        for (int i = 0; i <= N; ++i) {
            a[i] = pow(-c, i) * combination(N, i);
        }
    } else {
        std::cerr << "LinearFilter::gamma: invalid value for tau or n, not "
                     "initializing."
                  << std::endl;
        throw std::runtime_error(
            "LinearFilter::gamma: invalid value for tau or n, not "
            "initializing.");
    }
}

void LinearFilter::update() {
    c_begin = clock();
    b_begin = boost::chrono::system_clock::now();
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    (last_inputs[0])->assign(*(this->source_ports[0].getData()));

    // Rotation on addresses of the last_values.
    CImg<double> *fakepoint = last_values[N];
    for (int i = 1; i < N + 1; ++i) {
        last_values[N + 1 - i] = last_values[N - i];
    }
    last_values[0] = fakepoint;

    last_values[0]->fill(0.0);
    // Calculating new value of filter recursively:

    for (int k = 0; k < M; k++) {
        for (int i = 0; i < (int)last_values[0]->size(); i++) {
            last_values[0]->_data[i] += b[k] * last_inputs[k]->_data[i];
        }
    }

    for (int k = 1; k < N + 1; k++) {
        for (int i = 0; i < (int)last_values[0]->size(); i++) {
            last_values[0]->_data[i] -= a[k] * last_values[k]->_data[i];
        }
    }

    for (int i = 0; i < (int)last_values[0]->size(); i++) {
        last_values[0]->_data[i] *= 1 / a[0];
    }

    //*(last_values[0]) = b[0] * (*(last_inputs[0]));
    //    for (int j = 0; j < M; j++) {
    //        *(last_values[0]) += (b[j] * (*(last_inputs[j])));
    //    }
    //    //#pragma omp parallel for num_threads(2)
    //    for (int k = 1; k < N + 1; k++) {
    //        *(last_values[0]) -= (a[k] * (*(last_values[k])));
    //    }
    //    (*(last_values[0])) /= a[0];

    CImg<double> *tmp = last_inputs[M - 1];
    for (int i = 1; i < M; ++i) {
        last_inputs[M - i] = last_inputs[M - i - 1];
    }
    last_inputs[0] = tmp;

    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *LinearFilter::getOutput() const { return last_values[0]; }
