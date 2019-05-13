#include "operation.hpp"

#include <iostream>

operation_t::operation_t() {
    allocated = 0;
};

operation_t::~operation_t(){

};

int operation_t::vaultR1() {
    int aux = -1;
    if (read_address_1 != 0) aux  = ((read_address_1 >> 6) & 0b11111);
    return aux;
}

int operation_t::vaultR2() {
    int aux = -1;
    if (read_address_2 != 0) aux  = ((read_address_2 >> 6) & 0b11111);
    return aux;
}

int operation_t::vaultW() {
    int aux = -1;
    if (write_address!= 0) aux  = ((write_address >> 6) & 0b11111);
    return aux;
}

void operation_t::reset(){
    read_address_1 = 0;
    read_address_2 = 0;
    write_address = 0;
    allocated = 0;
};