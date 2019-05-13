#include <inttypes.h> 

class operation_t {
    private:

    public:
        operation_t();
        ~operation_t();
        int vaultR1();
        int vaultR2();
        int vaultW();
        void reset();
        
        uint64_t read_address_1;
        uint64_t read_address_2;
        uint64_t write_address;
        int allocated;
};