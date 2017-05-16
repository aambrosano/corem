#include <COREM/core/displayWithBar.h>

#include <boost/lexical_cast.hpp>

double DisplayWithBar::findMin(CImg<double> *input) {
    double min = DBL_INF;
    cimg_forXY(*(input), x, y) {
        if ((*input)(x, y, 0, 0) < min) min = (*input)(x, y, 0, 0);
    }

    return min;
}

double DisplayWithBar::findMax(CImg<double> *input) {
    double max = -DBL_INF;
    cimg_forXY(*(input), x, y) {
        if ((*input)(x, y, 0, 0) > max) max = (*input)(x, y, 0, 0);
    }

    return max;
}

DisplayWithBar::DisplayWithBar(string name, int columns, int rows,
                               bool withBar /* = true */,
                               int barSize /* = 50 */,
                               bool trueColor /* = false */)
    : columns_(columns),
      rows_(rows),
      withBar(withBar),
      barSize(barSize),
      trueColor(trueColor) {
    bar = new CImg<double>(barSize, (int)rows_, 1, 1);

    if (withBar) {
        display.resize(columns_ + barSize, rows_);
    } else {
        display.resize(columns_, rows_);
    }

    display.set_title(name.c_str());
}

void DisplayWithBar::updateBar(double min, double max) {
    const unsigned char colorWhite[] = {255, 255, 255};

    bar->resize(barSize, rows_, 1, 1);
    cimg_forXY(*(bar), x, y) {
        (*bar)(x, (int)rows_ - y - 1, 0, 0) = 255 * ((double)y / rows_);
    }
    bar->map(CImg<double>::jet_LUT256());

    // Draw min label
    string minString;

    if (min < 100) {
        minString = boost::lexical_cast<string>(min).substr(0, 6);
    } else {
        minString = boost::lexical_cast<string>((int)min);
    }

    bar->draw_text(5, bar->height() - 20, minString.c_str(), colorWhite, 0, 1,
                   20);

    // Draw max label
    string maxString;

    if (max < 100) {
        maxString = boost::lexical_cast<string>(max).substr(0, 6);
    } else {
        maxString = boost::lexical_cast<string>((int)max);
    }

    bar->draw_text(5, 10, maxString.c_str(), colorWhite, 0, 1, 20);
}

void DisplayWithBar::update(CImg<double> *img) {
    //    std::cout << "updating displaywithbar " << this->display.title() <<
    //    std::endl;
    // find maximum and minimum values
    double min = findMin(img);
    double max = findMax(img);

    //    d.display(*img);
    CImg<double> displayImage = *img;

    //    min = img->min_max(max);

    if (withBar) updateBar(min, max);

    if (!trueColor) {
        // Crop image
        img->crop(margin_, margin_, 0, 0, img->width() - margin_ - 1,
                  img->height() - margin_ - 1, 0, 0, false);
        // displayImage = (img->normalize(min, max))
        if (max - min > DBL_EPSILON) {
            displayImage = (255 * (*img - min) / (max - min))
                               .resize(columns_, rows_)
                               .map(CImg<double>::jet_LUT256());
            //            displayImage = img->normalize(0, 255);
        } else {
            displayImage = (*img + 0)
                               .resize(columns_, rows_)
                               .map(CImg<double>::jet_LUT256());
        }
    } else {
        display.set_normalization(0);
        displayImage = *img;
        displayImage.resize(columns_, rows_);
    }

    if (withBar) {
        display.display((displayImage, *bar));
    } else {
        display.display(displayImage);
    }
}
