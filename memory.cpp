#include "memory.hpp"

#include <iostream>

memory_t::memory_t() {
    for (int i = 0; i < 32; i++){
        vaults[i] = new operation_t;
    }
    for (int i = 0; i < 8; i++){
        links[i] = 0;
    }
};

memory_t::~memory_t(){

};

void memory_t::reset() {
    for (int i = 0; i < 32; i++){
        vaults[i]->reset();
    }

    for (int i = 0; i < 8; i++){
        links[i] = 1;
    }
}

int memory_t::evaluate() {
    int custo = 0;
    for (int i = 0; i < 32; i++) {
        if (vaults[i]->allocated){
            int r1, r2, w;
            r1 = vaults[i]->vaultR1();
            r2 = vaults[i]->vaultR2();
            w = vaults[i]->vaultW();

            if (r1 != -1) custo += distancia (r1, i);
            if (r2 != -1) custo += distancia (r2, i);
            if (w != -1) custo += distancia (w, i);
        }
    }
    return custo;
};

int memory_t::estimateTotalCost (operation_t* op, int vault){
    int custo = -1;
    if (!vaults[vault]->allocated){
        int r1, r2, w;
        r1 = op->vaultR1();
        r2 = op->vaultR2();
        w = op->vaultW();
        custo = 0;

        if (r1 != -1) custo += this->distancia (r1, vault);
        if (r2 != -1) custo += this->distancia (r2, vault);
        if (w != -1) custo += this->distancia (w, vault);

        custo += this->evaluate();
    }

    return custo;
};

int memory_t::estimateCost (operation_t* op, int vault){
    int r1, r2, w, custo;
    r1 = op->vaultR1();
    r2 = op->vaultR2();
    w = op->vaultW();
    custo = 0;

    if (r1 != -1) custo += this->distancia (r1, vault);
    if (r2 != -1) custo += this->distancia (r2, vault);
    if (w != -1) custo += this->distancia (w, vault);
    return custo;
};

void memory_t::allocate (operation_t* op, int vault){
    if (!vaults[vault]->allocated){
        vaults[vault] = op;
        vaults[vault]->allocated = 1;

        int r1, r2, w;
        r1 = vaults[vault]->vaultR1();
        r2 = vaults[vault]->vaultR2();
        w = vaults[vault]->vaultW();

        if (r1 != -1) updateLinks (r1, vault);
        if (r2 != -1) updateLinks (r2, vault);
        if (w != -1) updateLinks (w, vault);

        //printf ("%d %d %d, %d\n", r1, r2, w, vault);
        /*for (int i = 0; i < 8; i++){
            printf ("link %d: %d, ", i, links[i]);
        }
        printf ("\n");*/

        //printf ("%d. %d. ", vault, evaluate());
    }
}

void memory_t::printLinks() {
    for (int i = 0; i < 8; i++){
        printf ("%d ", links[i]);
    }
    printf ("\n");
};

void memory_t::updateLinks (int x, int y){
    int start = x/4;
    int end = y/4;
    int diff = start - end;
    
    if ((diff >= -4 && diff < 0) || (diff > 4 && diff <= 7)){
        int i = start;
        while (i != end){
            links[i]++;
            i++;
            if (i > 7) i = 0;
        }
    } else {
        int j = start;  
        while (j != end){
            j--;
            if (j < 0) j = 7;
            links[j]++;
        }
    }
}

int memory_t::distancia (int x, int y){
    int resultado = 0;
    int start = x/4;
    int end = y/4;
    int diff = start - end;
    
    if ((diff >= -4 && diff < 0) || (diff > 4 && diff <= 7)){
        int i = start;
        while (i != end){
            resultado += links[i];
            i++;
            if (i > 7) i = 0;
        }
    } else {
        int j = start;  
        while (j != end){
            j--;
            if (j < 0) j = 7;
            resultado += links[j];
        }
    }
    
    return resultado;
};