#include "situation.hpp"

#include <iostream>

situation_t::situation_t(){

};

situation_t::~situation_t(){

};

void situation_t::addOperation (operation_t op){
    for (int i = 0; i < 32; i++){
        if (!this->op[i]) this->op[i] = op;
    }
}