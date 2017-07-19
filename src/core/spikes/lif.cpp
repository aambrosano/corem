#include <corem/spikes/lif.h>

double grand() {
    double r, v1, v2, fac;

    r = 2;
    while (r >= 1) {
        v1 = (2 * ((double)rand() / (double)RAND_MAX) - 1);
        v2 = (2 * ((double)rand() / (double)RAND_MAX) - 1);
        r = v1 * v1 + v2 * v2;
    }
    fac = sqrt(-2 * log(r) / r);

    return (v2 * fac);
}

LIF::LIF(std::string id, retina_config_t* conf, simulation_state_t* sim_state)
    : Module(id, conf), simulation_state(sim_state) {
    voltage = new CImg<double>(columns, rows, 1, 1, 0.0);
    spikeMap = new CImg<double>(columns, rows, 1, 1, 0.0);
    refract = new CImg<bool>(columns, rows, 1, 1, false);
    refract_time = new CImg<int>(columns, rows, 1, 1, 0);
    current = new CImg<double>(columns, rows, 1, 1, 0.0);

    for (int i = 0; i < 100; i++) {
        spikeRates.push_back(*(new CImg<double>(columns, rows, 1, 1, 0.0)));
    }
    round_idx = 0;
}

void LIF::update() {
    std::cout << "UPDATING LIF" << std::endl;
    if (this->source_ports.size() == 0) return;

    current->assign(*(source_ports[0].getData()));

    cimg_forXY(*current, x, y) {
        spikeRates[round_idx](x, y, 0) = 0.0;
        // 1ms step in any case
        double step_ms = 1.0;
        for (int i = 0; i < config->simulationStep / step_ms; i++) {
            if ((*refract)(x, y, 0)) {
                (*refract_time)(x, y, 0) -= 1;
                if ((*refract_time)(x, y, 0) == 0) {
                    (*refract)(x, y, 0) = false;
                }
            }
            if (!(*refract)(x, y, 0)) {
                (*voltage)(x, y, 0) += (*current)(x, y, 0);  // -
                // (1. / (0.02 * 1000.0)) * (*voltage)(x, y, 0);
            }

            if ((*voltage)(x, y, 0) >= 1.0) {
                spike_t s((double)(simulation_state->currentTime + i), x, y);
                this->spikes.push_back(s);
                (*voltage)(x, y, 0) = 0.0;
                (*refract)(x, y, 0) = true;
                spikeRates[round_idx](x, y, 0) += 1.0;
                // mean 3.0 std 1.0, no point in multiplying by 1.0 but it makes
                // clear what's happening there
                (*refract_time)(x, y, 0) =
                    std::max<int>(0, (int)((grand() + 3.0) * 1.0));
                if ((*refract_time)(x, y, 0) == 0) (*refract)(x, y, 0) = false;
            }
        }
    }

    round_idx = (round_idx + 1) % 100;
}

// spike_t* LIF::getSpikes() {
std::vector<spike_t> LIF::getSpikes() {
    return this->spikes;
    /* spike_t* ret = new spike_t[spikes.size()];
     std::copy(spikes.begin(), spikes.end(), ret);
     spikes.empty();
     return ret; */
}

const CImg<double>* LIF::getOutput() const {
    *spikeMap = 0.0;
    // spikeMap->assign(spikeRates[round_idx]);
    for (std::vector<CImg<double> >::const_iterator it = spikeRates.begin();
         it != spikeRates.end(); ++it) {
        *spikeMap += *it;
    }
    return this->spikeMap;
}
