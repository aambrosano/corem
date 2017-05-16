#include <COREM/core/displayManager.h>

DisplayManager::DisplayManager() {
    config = NULL;
    last_row = 0;
    last_col = 0;
    imagesPerRow = 4;

    valuesAllocated = false;
}

void DisplayManager::reset() {
    last_row = 0;
    last_col = 0;
    imagesPerRow = 0;
    imagesPerRow = 4;

    valuesAllocated = false;
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

void DisplayManager::updateDisplay(Input *input, vector<Module *> &modules,
                                   unsigned int switchTime,
                                   unsigned int currentTime,
                                   unsigned int trial) {
#if cimg_display != 0
    // Display input
    if (isShown[0]) {
        displays[0]->update(input->getData(currentTime));
    }

    // show modules
    for (int k = 0; k < modules.size() - 1; k++) {
        if (isShown[k + 1]) {
            //            if (k == 0) d.display(*(modules[k + 1]->getOutput()));
            displays[k + 1]->update(modules[k + 1]->getOutput());
        }
    }

    // Show displays if there's an input display
    if (isShown[0]) {
        displays[0]->wait(config->displayDelay);
    }
#endif
}
