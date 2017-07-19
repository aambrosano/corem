#include <corem/displayManager.h>

DisplayManager::DisplayManager() {
    config = NULL;
    last_row = 0;
    last_col = 0;
    imagesPerRow = 4;
}

DisplayManager::~DisplayManager() {
    std::vector<DisplayWithBar *>::iterator dwb;
    for (dwb = displays.begin(); dwb != displays.end(); ++dwb) {
        delete *dwb;
    }
}

void DisplayManager::reset() {
    last_row = 0;
    last_col = 0;
    imagesPerRow = 0;
    imagesPerRow = 4;
}

void DisplayManager::setImagesPerRow(int numberI) { imagesPerRow = numberI; }

void DisplayManager::setIsShown(bool value, int pos) {
    std::cout << "Is shown set to " << value << " for display "
              << displays[pos]->display.title() << std::endl;
    isShown[pos] = value;
    displays[pos]->show();
}

void DisplayManager::setMargin(double m, int pos) { margin[pos] = m; }

void DisplayManager::setConfig(retina_config_t *conf) { this->config = conf; }

// TODO: this should happen always, so this method should eventually be removed
void DisplayManager::addInputDisplay() {
#if cimg_display != 0
    double newX = (double)config->columns * config->displayZoom;
    double newY = (double)config->rows * config->displayZoom;

    isShown.push_back(false);
    margin.push_back(0.0);

    DisplayWithBar *inputDisplay =
        new DisplayWithBar("Input", newX, newY, false, 50, true);
    inputDisplay->move(0, 0);
    inputDisplay->setMargin(margin[0]);
    inputDisplay->hide();

    displays.push_back(inputDisplay);
#endif
}

void DisplayManager::addDisplay(string ID) {
#if cimg_display != 0
    // black image
    double newX = (double)config->columns * config->displayZoom;
    double newY = (double)config->rows * config->displayZoom;

    isShown.push_back(false);
    margin.push_back(0.0);

    DisplayWithBar *dwb = new DisplayWithBar(ID, newX, newY);
    dwb->setMargin(margin[margin.size() - 1]);

    if (isShown[isShown.size() - 1]) {
        // new row of the display
        int capacity =
            int((CImgDisplay::screen_width() - newY - 100) / (newY + 50));

        if (last_col < capacity && last_col < imagesPerRow) {
            last_col++;
        } else {
            last_col = 1;
            last_row++;
        }

        // move display
        dwb->move((int)last_col * (newY + 80.0), (int)last_row * (newX + 80.0));
    } else {
        dwb->hide();
    }

    // Save display
    displays.push_back(dwb);
#endif
}

void DisplayManager::show(std::string id) {
    for (int i = 0; i < (int)displays.size(); i++) {
        if (displays[i]->id == id) {
            isShown[i] = true;
            displays[i]->show();
        }
    }
}

void DisplayManager::updateDisplays(const CImg<double> *input,
                                    CImg<double> *photoreceptors[4],
                                    std::vector<Module *> modules,
                                    simulation_state_t *sim_state) {
#if cimg_display != 0
    // Display input
    std::string id;
    for (int i = 0; i < (int)displays.size(); i++) {
        if (!isShown[i]) continue;
        id = displays[i]->id;
        if (id == "Input") {
            displays[i]->update(input);
        } else if (id == "L_cones") {
            displays[i]->update(photoreceptors[0]);
        } else if (id == "M_cones") {
            displays[i]->update(photoreceptors[1]);
        } else if (id == "S_cones") {
            displays[i]->update(photoreceptors[2]);
        } else if (id == "rods") {
            displays[i]->update(photoreceptors[3]);
        } else {
            for (unsigned int j = 0; j < modules.size(); j++) {
                if (modules[j]->id() == id) {
                    displays[i]->update(modules[j]->getOutput());
                }
            }
        }
    }
#endif
}
