#include <COREM/core/input/sequence.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const std::map<std::string, param_val_t> Sequence::defaultParams =
    Sequence::createDefaults();

std::map<std::string, param_val_t> Sequence::createDefaults() {
    std::map<std::string, param_val_t> ret;

    ret["path"].s = new std::string(".");
    ret["steps_per_image"].u = 1;

    return ret;
}

Sequence::Sequence(unsigned int columns_, unsigned int rows_,
                   std::map<std::string, param_val_t> params)
    : Input(columns_, rows_, params, "Sequence") {
    std::vector<std::string> result;

    std::string path = *getParam("path", params, defaultParams).s;
    time_per_image = getParam("time_per_image", params, defaultParams).u;

    if (!fs::exists(path) || !fs::is_directory(path))
        throw std::runtime_error(
            (new std::string("Directory " + path + " not found or invalid."))
                ->c_str());

    fs::directory_iterator end_iter;
    for (fs::directory_iterator dir_iter(path); dir_iter != end_iter;
         ++dir_iter) {
        if (fs::is_regular_file(dir_iter->status())) {
            result.push_back(dir_iter->path().string());
        }
    }
    sort(result.begin(), result.end());

    inputSeq = new CImg<double> *[result.size()];
    numberImages = result.size();

    for (unsigned int i = 0; i < numberImages; i++) {
        inputSeq[i] = new CImg<double>();
        inputSeq[i]->assign(result[i].c_str());
    }

    this->columns = inputSeq[0]->width();
    this->rows = inputSeq[0]->height();

    if (verbose)
        std::cout << numberImages << " images read from " << path << std::endl;
}

CImg<double> *Sequence::getData(unsigned int t) {
    return inputSeq[std::min(numberImages - 1, t / time_per_image)];
}

void Sequence::getSize(unsigned int &columns, unsigned int &rows) {
    columns = this->columns;
    rows = this->rows;
}
