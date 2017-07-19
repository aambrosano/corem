#include <corem/input.h>

Input::Input(int columns, int rows, std::map<std::string, param_val_t> params)
    : columns(columns), rows(rows) {
    UNUSED(params)  // Will be use by subclasses
}

Input::~Input() {}
