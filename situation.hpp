#include <inttypes.h> 

#include "operation.hpp"

using namespace pmr;

class situation_t {
    private:

    public:
        situation_t();
        ~situation_t();
        void addOperation (operation_t op);
    
        std::list<operation_t> ops;
};