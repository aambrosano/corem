/* BeginDocumentation
 * Name: main
 *
 * Description: fixed time-step simulation of the retina script specified
 * in "constants.cpp" or passed through arguments.
 *
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso:InterfaceNEST
 */

#include <corem/constants.h>
#include <corem/retina.h>

#include <CImg.h>

#include <boost/filesystem.hpp>
#include <boost/python.hpp>

using namespace cimg_library;
using namespace std;

namespace fs = boost::filesystem;
namespace py = boost::python;
namespace bchrono = boost::chrono;

CImg<double> *custom_img(int currTime, int width, int height) {
    CImg<double> *retval = new CImg<double>(width, height, 1, 3);
    for (int i = 0; i < retval->width(); i++) {
        for (int j = 0; j < retval->height(); j++) {
            (*retval)(i, j, 0, 0) = (*retval)(i, j, 0, 1) =
                (*retval)(i, j, 0, 2) = 255;
        }
    }

    (*retval)(currTime % retval->width(), currTime % retval->height(), 0, 0) =
        255;
    (*retval)(currTime % retval->width(), currTime % retval->height(), 0, 1) =
        0;
    (*retval)(currTime % retval->width(), currTime % retval->height(), 0, 2) =
        0;
    return retval;
}

CImg<double> *impulse_image(int width, int height) {
    CImg<double> *retval = new CImg<double>(width, height, 1, 3);
    for (int i = 0; i < retval->width(); i++) {
        for (int j = 0; j < retval->height(); j++) {
            if (i == 0) {
                (*retval)(i, j, 0, 0) = 255;
                (*retval)(i, j, 0, 1) = 255;
                (*retval)(i, j, 0, 2) = 255;
            } else {
                (*retval)(i, j, 0, 0) = (*retval)(i, j, 0, 1) =
                    (*retval)(i, j, 0, 2) = 0;
            }
        }
    }
    return retval;
}

CImg<double> put_sin_ball(CImg<double> *img, double freq, double ampl, double t,
                          int x0, int y0, int size) {
    int yt = y0;
    int xt = x0 + std::sin((freq * t) * (2 * PI)) * ampl;

    CImg<double> ret = *img;
    for (int i = xt - size / 2; i < xt + size / 2; i++) {
        for (int j = yt - size / 2; j < yt + size / 2; j++) {
            if (std::sqrt((std::pow((j - yt), 2) + std::pow((i - xt), 2))) <
                size / 2) {
                ret(i, j, 0, 0) = 0;
                ret(i, j, 0, 1) = 255;
                ret(i, j, 0, 2) = 0;
            }
        }
    }

    return ret;
}

void draw_ball(CImg<double> &img, int x, int y, int sz, int r, int g, int b) {
    for (int i = x - sz / 2; i < x + sz / 2; i++) {
        for (int j = y - sz / 2; j < y + sz / 2; j++) {
            if (j >= 0 && j < img.height() && i >= 0 && i < img.width()) {
                if (std::sqrt((std::pow((j - y), 2) + std::pow((i - x), 2))) <
                    sz / 2) {
                    img(i, j, 0, 0) = r;
                    img(i, j, 0, 1) = g;
                    img(i, j, 0, 2) = b;
                }
            }
        }
    }
}

void fill_bg(CImg<double> &img, int r, int g, int b) {
    for (int row = 0; row < img.height(); row++) {
        for (int col = 0; col < img.width(); col++) {
            img(col, row, 0, 0) = r;
            img(col, row, 0, 1) = g;
            img(col, row, 0, 2) = b;
        }
    }
}

CImg<double> put_pulse_ball(CImg<double> *img, double freq, double t,
                            double rmin, double rmax) {
    int yt = 120;
    int xt = 160;
    int size =
        (int)(rmin + (rmax - rmin) * std::abs(std::sin((freq * t) * (2 * PI))));

    CImg<double> ret = *img;
    for (int i = xt - size / 2; i < xt + size / 2; i++) {
        for (int j = yt - size / 2; j < yt + size / 2; j++) {
            if (std::sqrt((std::pow((j - yt), 2) + std::pow((i - xt), 2))) <
                size / 2) {
                ret(i, j, 0, 0) = 0;
                ret(i, j, 0, 1) = 255;
                ret(i, j, 0, 0) = 0;
            }
        }
    }

    return ret;
}

int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    Retina retina;
    //    retina.loadCircuit(
    //        "/home/alessandro/NRP/Experiments/retina_icub/"
    //        "green_tracking_retina.py");

    retina.loadCircuit((string)argv[1]);
    boost::chrono::system_clock::time_point start =
        boost::chrono::system_clock::now();
    clock_t begin = clock();
    for (int i = 0; i < 50; i++) {
        retina.step();
    }

    clock_t end = clock();
    std::cout << "Total CPU time: " << (double)(end - begin) / CLOCKS_PER_SEC
              << std::endl;
    boost::chrono::duration<double> sec =
        boost::chrono::system_clock::now() - start;
    std::cout << "Total wall clock time:  " << sec.count() << " seconds\n";

    retina.stats();
    return 0;
    //    std::vector<unsigned int> qq;
    //    for (unsigned int i = 0; i < 255; i++) qq.push_back(i);
    //    std::vector<unsigned int> qq2;
    //    for (unsigned int i = 0; i < 255; i++) qq2.push_back(255 - i);
    //    Gnuplot plot;
    //    plot << "set xrange [0:255]" << std::endl;
    //    plot << "set yrange [0:255]" << std::endl;
    //    plot << "plot '-' with lines title 'aaaa'" << std::endl;
    //    plot.send1d(boost::make_tuple(qq, qq));

    //    Gnuplot plot2;
    //    plot2 << "set xrange [0:255]" << std::endl;
    //    plot2 << "set yrange [0:255]" << std::endl;
    //    plot2 << "plot '-' with lines title 'aaaa'" << std::endl;
    //    plot2.send1d(boost::make_tuple(qq, qq2));
    //    return 1;

    //    boost::filesystem::path
    //    currentDirRoot(boost::filesystem::current_path());

    //    // delete files in results folder (if any)
    //    fs::path resultsDir = currentDirRoot / "results";
    //    fs::remove_all(resultsDir);

    //    // Create retina interface
    //    fs::path retinaPath;

    //    // read arguments or default script
    //    if (argc == 1) {
    //        retinaPath = currentDirRoot / constants::retinaScript;
    //    } else {
    //        retinaPath = (string)argv[1];
    //        if (!fs::exists(retinaPath))
    //            retinaPath = currentDirRoot / (string)argv[1];
    //        if (!fs::exists(retinaPath)) {
    //            std::cout << "Wrong retina file path" << std::endl;
    //            return EXIT_FAILURE;
    //        }
    //    }

    //    Retina retina;
    //    retina.loadCircuit(retinaPath.string());

    //#ifdef DEBUG
    //    bchrono::time_point<bchrono::system_clock> start =
    //        bchrono::system_clock::now();
    //#endif

    //    retina.run();

    //#ifdef DEBUG
    //    bchrono::duration<double> diff = bchrono::system_clock::now() - start;
    //    std::cout << "main(): total time: " << diff.count() << ", frame rate:
    //    "
    //              << (retina.getConfig()->simulationTime /
    //                  retina.getConfig()->simulationStep) /
    //                     diff.count()
    //              << std::endl;
    //#endif

    //    return 0;
}
