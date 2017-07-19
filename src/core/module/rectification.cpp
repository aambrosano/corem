#include <corem/module/rectification.h>

Rectification::Rectification(std::string id, retina_config_t *conf, double T0,
                             double lambda, double V_th)
    : Module(id, conf), T0(T0), lambda(lambda), V_th(V_th) {
    this->image = new CImg<double>(columns, rows, 1, 1, 0.01);
}

Rectification::~Rectification() { delete this->image; }

void Rectification::update() {
    c_begin = clock();
    b_begin = boost::chrono::system_clock::now();
    image->assign(*(source_ports[0].getData()));

    cimg_forXY(*image, x, y) {
        double tmp = 0.0;
        if ((*image)(x, y, 0, 0) < V_th) {
            tmp = T0 * T0;
            tmp /= T0 - lambda * ((*image)(x, y, 0, 0) - V_th);
        } else {
            tmp = T0 + lambda * ((*image)(x, y, 0, 0) - V_th);
        }
        (*image)(x, y, 0, 0) = tmp;
    }
    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *Rectification::getOutput() const { return this->image; }
