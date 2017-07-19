#include <corem/module/highpass.h>
#include <cassert>

HighPass::HighPass(std::string id, retina_config_t *conf, double w, double tau)
    : Module(id, conf) {
    assert(w >= 0 && w <= 1);
    assert(tau > 0);

    this->w = w;
    this->tau = tau;
    this->N = 10;

    last_inputs = new CImg<double> *[this->N];
    a = new double[this->N];
    for (int i = 0; i < this->N; i++) {
        a[i] = -this->w * std::exp(-(config->simulationStep * i) / this->tau) /
               this->tau;
    }
    a[0] += 1;

    double sum = 0;
    for (int i = 0; i < this->N; i++) sum += a[i];
    std::cout << "Highpass sum: " << sum << std::endl;
    for (int i = 0; i < this->N; i++) a[i] /= sum;

    for (int i = 0; i < this->N; i++) {
        this->last_inputs[i] = new CImg<double>(columns, rows, 1, 1, 0.1);
    }
    this->output = new CImg<double>(columns, rows, 1, 1, 0.1);
}

HighPass::~HighPass() {
    delete[] a;
    for (int i = 0; i < this->N; i++) {
        delete last_inputs[i];
    }
    delete[] last_inputs;

    delete output;
}

void HighPass::update() {
    c_begin = clock();
    b_begin = boost::chrono::system_clock::now();
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    //    std::cout << "Highpass::update " <<
    //    this->source_ports[0].getData()->size()
    //              << std::endl;
    (last_inputs[0])->assign(*(this->source_ports[0].getData()));

    // Calculating new value of filter recursively:
    *output = a[0] * (*(last_inputs[0]));
    for (int j = 1; j < this->N; j++) {
        *output += (a[j] * (*(last_inputs[j])));
    }

    CImg<double> *tmp = last_inputs[this->N - 1];
    for (int i = 1; i < this->N; ++i) {
        last_inputs[this->N - i] = last_inputs[this->N - i - 1];
    }
    last_inputs[0] = tmp;

    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *HighPass::getOutput() const { return output; }
