#include <corem/multimeter/temporalMultimeter.h>

TemporalMultimeter::TemporalMultimeter(int columns, int rows,
                                       retina_config_t *conf, std::string id,
                                       int timeToShow, int x, int y,
                                       int sim_step)
    : Multimeter(columns, rows, conf, id, timeToShow),
      x(x),
      y(y),
      sim_step(sim_step) {}

void TemporalMultimeter::record(/* CImg<double> *image */) {
    const CImg<double> *image;
    if (this->source_module != NULL)
        image = this->source_module->getOutput();
    else
        image = this->source_image;

    assert(image->width() == this->columns);
    assert(image->height() == this->rows);

    double val = image->operator()(x, y, 0, 0);
    //    assert(val != std::numeric_limits<double>::infinity());
    //    assert(val != -std::numeric_limits<double>::infinity());
    if (val == std::numeric_limits<double>::infinity() ||
        val == -std::numeric_limits<double>::infinity()) {
        std::cerr << "Warning: recorded an infinity value, defaulting to 0"
                  << std::endl;
        val = 0;
    }
    this->data.push_back(val);
}

void TemporalMultimeter::showGP(int start_from) {
    assert(start_from >= 0);

    if (!this->showable) {
        std::cerr << "Warning: this multimeter is not showable, terminating."
                  << std::endl;
        return;
    }

    if (start_from > 0) {
        this->data = std::vector<double>(this->data.begin() + start_from,
                                         this->data.end());
    }

    Gnuplot plot;

    std::vector<double>::iterator min =
        std::min_element(this->data.begin(), this->data.end());
    std::vector<double>::iterator max =
        std::max_element(this->data.begin(), this->data.end());
    std::cout << *min << " " << *max << std::endl;
    plot << "set xrange [" << 0 << ":" << this->data.size() * this->sim_step
         << "]" << std::endl;
    double extra = 0.05 * ((*max) - (*min));
    plot << "set yrange [" << (*min) - extra << ":" << (*max) + extra << "]"
         << std::endl;
    plot << "plot '-' with lines title '" << this->id << "'" << std::endl;

    std::vector<unsigned int> index;
    for (unsigned int i = 0; i < this->data.size(); i++) {
        index.push_back(i * this->sim_step);
    }
    plot.send1d(boost::make_tuple(index, this->data));
    getchar();
}

void TemporalMultimeter::dump(std::string filename) {
    if (filename == "") {
        filename = "./results " + this->id + ".txt";
    }

    std::ofstream fs;
    fs.open(filename.c_str(), std::ofstream::out);
    fs.precision(64);
    for (unsigned int i = 0; i < this->data.size(); i++) {
        fs << this->data[i] << std::endl;
    }
    fs.close();
}

void TemporalMultimeter::show(int waitTime) {
    if (!this->showable) {
        std::cerr << "Warning: this multimeter is not showable, terminating."
                  << std::endl;
        return;
    }

    std::cerr << "Warning: this way to plot multimeter is deprecated, use "
                 "showGP() instead."
              << std::endl;

    CImg<double> *temporalProfilet =
        new CImg<double>(this->data.size(), 1, 1, 1, 0);

    double max_value = DBL_EPSILON;
    double min_value = DBL_INF;

    for (unsigned int k = 0; k < this->data.size(); k++) {
        (*temporalProfilet)(k, 0, 0, 0) = this->data[k];

        if (this->data[k] > max_value) {
            max_value = this->data[k];
        }
        if (this->data[k] < min_value) {
            min_value = this->data[k];
        }
    }

    if (max_value == min_value) {
        max_value += 1;
        min_value -= 1;
    }

    if (min_value > 0) {
        min_value = 0;
    }
    if (max_value < 0) {
        max_value = 0;
    }

    double range = max_value - min_value;

    std::string filename = this->id + ".txt";
    // remove file and save new file
    fs::remove(filename);
    Multimeter::saveArray(this->data, filename);

    // plot
    if (waitTime > -2) {
        const unsigned char color1[] = {255, 0, 0};
        const unsigned char color2[] = {0, 0, 255};
        unsigned char back_color[] = {255, 255, 255};
        CImg<unsigned char> *profile =
            new CImg<unsigned char>(400, 256, 1, 3, 0);
        profile->fill(*back_color);
        const char *titleChar = id.c_str();
        CImgDisplay *draw_disp = new CImgDisplay(*profile, titleChar);

        profile
            ->draw_graph(temporalProfilet->get_crop(
                             0, 0, 0, 0, this->data.size() - 1, 0, 0, 0) *
                                 255 / range -
                             min_value * 255 / range,
                         color1, 1, 1, 1, 255, 0)
            .display(*draw_disp);
        profile
            ->draw_axes(0.0, this->data.size() * this->sim_step, max_value,
                        min_value, color2, 1, -80, -80, 0, 0, ~0U, ~0U, 20)
            .display(*draw_disp);
        profile->draw_text(320, 200, "time (ms)", color2, back_color, 1, 20)
            .display(*draw_disp);
        profile->draw_text(40, 5, "Output", color2, back_color, 1, 20)
            .display(*draw_disp);

        // move display
        //        draw_disp->move(col, row);

        if (waitTime == -1) {
            while (!draw_disp->is_closed()) {
                draw_disp->wait();
            }
        }
    }

    //    delete temp;
}
