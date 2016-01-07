#include "module.h"

module::module(int x, int y, double temporal_step){
    step = temporal_step;
    sizeX=x;
    sizeY=y;
}

//------------------------------------------------------------------------------//

void module::setSizeX(int x){
    if (x>0){
        sizeX = x;
    }
}

void module::setSizeY(int y){
    if (y>0){
        sizeY = y;
    }
}

void module::set_step(double temporal_step) {
    if (temporal_step>0){
        step = temporal_step;
    }
}
