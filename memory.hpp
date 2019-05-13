#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <inttypes.h> 
#include <time.h>

#include "operation.hpp"

class memory_t {
    private:
        

    public:
        memory_t();
        ~memory_t();
        void allocate (operation_t* op, int vault);
        void deallocate (int vault);
        int estimateCost (operation_t* op, int vault);
        int estimateTotalCost (operation_t* op, int vault);
        int distancia (int x, int y);
        int evaluate();
        void reset();
        void printLinks();
        void updateLinks (int x, int y);

        operation_t* vaults[32];
        int links[8];
};