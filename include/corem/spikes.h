#ifndef SPIKES_H
#define SPIKES_H

#include <string>
#include <vector>

typedef struct spike_t {
    double time;
    int x;
    int y;
    std::string id;

    spike_t(double t, int x, int y) : time(t), x(x), y(y) {}
    spike_t() : time(-1.0), x(0), y(0) {}
} spike_t;

class SpikeSource {
   public:
    virtual std::vector<spike_t> getSpikes() = 0;
};

#endif  // SPIKES_H
