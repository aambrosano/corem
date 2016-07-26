#include "module.h"

module::module(int columns, int rows, double temporal_step){
    step = temporal_step;
    columns_ = columns;
    rows_ = rows;
}

//------------------------------------------------------------------------------//

void module::setColumns(int columns){
    if (columns > 0) {
        columns_ = columns;
    }
}

void module::setRows(int rows) {
    if (rows > 0) {
        rows_ = rows;
    }
}

void module::set_step(double temporal_step) {
    if (temporal_step > 0) {
        step = temporal_step;
    }
}
