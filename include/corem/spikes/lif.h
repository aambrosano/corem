#ifndef LIF_H
#define LIF_H

#include <corem/constants.h>
#include <corem/module.h>
#include <corem/spikes.h>

#include <CImg.h>

#include <string>

using namespace cimg_library;

class LIF : public Module, public SpikeSource {
   public:
    LIF(std::string, retina_config_t*, simulation_state_t*);

    virtual void update();
    virtual const CImg<double>* getOutput() const;  // Spike map
    virtual std::vector<spike_t> getSpikes();

   protected:
    // For compatibility with spinnaker & similars
    std::vector<spike_t> spikes;
    std::vector<CImg<double> > spikeRates;
    unsigned int round_idx;

    // For internal computation (1:1 mapping with pixels in other layers)
    CImg<double>* voltage;
    CImg<double>* current;
    CImg<double>* spikeMap;
    CImg<bool>* refract;
    CImg<int>* refract_time;

    simulation_state_t* simulation_state;
};

#endif  // NOISYLIF_H
