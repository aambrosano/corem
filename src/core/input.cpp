#include "COREM/core/input.h"

Input::Input(unsigned int columns, unsigned int rows,
             std::map<std::string, param_val_t> params, std::string input_type)
    : columns(columns), rows(rows), input_type(input_type) {}
