#include <corem/multimeter/spatialMultimeter.h>

SpatialMultimeter::SpatialMultimeter(int columns, int rows,
                                     retina_config_t *conf, std::string id,
                                     int timeToShow, Orientation orientation,
                                     int row_or_column)
    : Multimeter(columns, rows, conf, id, timeToShow) {
    this->or_ = orientation;
    this->line = row_or_column;

    assert(or_ == HORIZONTAL || or_ == VERTICAL);
}

void SpatialMultimeter::dump(std::string filename) {
    std::cerr << "Warning: this feature has not been implemented yet."
              << std::endl;
}

void SpatialMultimeter::record(/* CImg<double> *image */) {
    const CImg<double> *image;
    if (this->source_module != NULL)
        image = this->source_module->getOutput();
    else
        image = this->source_image;

    assert(image->width() == this->columns);
    assert(image->height() == this->rows);

    unsigned int dim;

    if (or_ == HORIZONTAL) {
        dim = image->width();
    } else {  // (or_ == VERTICAL)
        dim = image->height();
    }

    this->data.clear();
    for (unsigned int i = 0; i < dim; i++) {
        if (or_ == HORIZONTAL) {
            this->data.push_back((*image)(i, this->line, 0, 0));
        } else {
            this->data.push_back((*image)(this->line, i, 0, 0));
        }
    }
}

void SpatialMultimeter::showGP(int start_from) {
    if (!this->showable) {
        std::cerr << "Warning: this multimeter is not showable, terminating."
                  << std::endl;
        return;
    }

    Gnuplot plot;

    if (start_from > 0) {
        this->data = *(new std::vector<double>(this->data.begin() + start_from,
                                               this->data.end()));
    }

    std::vector<double>::iterator min =
        std::min_element(this->data.begin(), this->data.end());
    std::vector<double>::iterator max =
        std::max_element(this->data.begin(), this->data.end());

    double extra = 0.05 * ((*max) - (*min));
    plot << "set xrange [" << 0 << ":" << this->data.size() << "]" << std::endl;
    plot << "set yrange [" << (*min) - extra << ":" << (*max) + extra << "]"
         << std::endl;
    plot << "plot '-' with lines title '" << this->id << "'" << std::endl;

    std::vector<unsigned int> index;
    for (unsigned int i = 0; i < this->data.size(); i++) {
        index.push_back(i);
    }
    plot.send1d(boost::make_tuple(index, this->data));

    getchar();
}

void SpatialMultimeter::show(int waitTime) {
    if (!this->showable) {
        std::cerr << "Warning: this multimeter is not showable, terminating."
                  << std::endl;
        return;
    }

    std::cerr << "Warning: this way to plot multimeter is deprecated, use "
                 "showGP() instead."
              << std::endl;

    unsigned int dim = this->data.size();
    CImg<double> *SpatialProfile1 = new CImg<double>(dim, 1, 1, 1, 0);

    // Normalize data
    double max_val = DBL_EPSILON;
    double min_val = DBL_INF;

    cimg_forXY(*SpatialProfile1, x, y) {
        if ((*SpatialProfile1)(x, y, 0, 0) > max_val)
            max_val = (*SpatialProfile1)(x, y, 0, 0);
        if ((*SpatialProfile1)(x, y, 0, 0) < min_val)
            min_val = (*SpatialProfile1)(x, y, 0, 0);
    }

    if (max_val == min_val) {
        max_val += 1.0;
        min_val -= 1.0;
    }

    if (min_val > 0) min_val = 0.0;
    if (max_val < 0) max_val = 0.0;

    double range = max_val - min_val;

    // plot
    if (waitTime > -2) {
        const unsigned char color1[] = {255, 0, 0};
        const unsigned char color2[] = {0, 0, 255};
        unsigned char back_color[] = {255, 255, 255};
        CImg<unsigned char> *profile =
            new CImg<unsigned char>(400, 256, 1, 3, 0);
        profile->fill(*back_color);
        CImgDisplay *draw_disp = new CImgDisplay(*profile, id.c_str());

        profile
            ->draw_graph(
                SpatialProfile1->get_crop(0, 0, 0, 0, dim - 1, 0, 0, 0) * 255 /
                        range -
                    min_val * 255 / range,
                color1, 1, 1, 4, 255, 0)
            .display(*draw_disp);
        profile
            ->draw_axes(0.0, dim, max_val, min_val, color2, 1, -80, -80, 0, 0,
                        ~0U, ~0U, 20)
            .display(*draw_disp);
        profile->draw_text(320, 200, "pixels", color2, back_color, 1, 20)
            .display(*draw_disp);
        profile->draw_text(40, 5, "Output", color2, back_color, 1, 20)
            .display(*draw_disp);

        //        // move display
        //        draw_disp->move(col, row);

        if (waitTime == -1) {
            while (!draw_disp->is_closed()) {
                draw_disp->wait();
            }
        }
    }
}
