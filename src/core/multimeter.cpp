#include <corem/multimeter.h>

Multimeter::Multimeter(int columns, int rows, retina_config_t *conf,
                       std::string id, int timeToShow)
    : config(conf),
      columns(columns),
      rows(rows),
      timeToShow(timeToShow),
      id(id),
      showable(true),
      source_image(NULL),
      source_module(NULL) {
    assert(columns > 0);
    assert(rows > 0);
}

Multimeter::~Multimeter() {}

void Multimeter::saveArray(std::vector<double> array, std::string path) {
    fs::path resultsFile = Multimeter::getExecutablePath() / "results" / path;
    std::ifstream fin;
    std::ofstream fout;

    int idx = 0;
    if (fs::is_regular_file(resultsFile)) {
        // Update the values
        double tmp;

        fin.open(resultsFile.string().c_str());
        fin.precision(64);
        fin >> tmp;

        array[idx++] += tmp;
        fin.close();
    }

    // Save the values on file
    fout.open(resultsFile.string().c_str());
    fout.precision(64);

    for (int i = 0; i < (int)array.size() - 1; i++) {
        fout << array[i];
        fout << std::endl;
    }

    fout << array[array.size() - 1];
    fout.close();
}

int Multimeter::showTime() { return this->timeToShow; }

void Multimeter::setShowable(bool showable) { this->showable = showable; }

void Multimeter::setSourceModule(Module *m) {
    if (this->source_image != NULL)
        std::cerr << "Warning: this multimeter has already a source image, by "
                     "adding a source module, the source image will be ignored."
                  << std::endl;
    this->source_module = m;
}

void Multimeter::setSourceImage(const CImg<double> *img) {
    if (this->source_module != NULL)
        std::cerr << "Warning: this multimeter has already a source module, "
                     "the source image will be ignored."
                  << std::endl;
    this->source_image = img;
}

const fs::path Multimeter::getExecutablePath() {
    return fs::read_symlink("/proc/self/exe").parent_path().string();
}
