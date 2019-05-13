#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <inttypes.h> 
#include <time.h>

class mem_opt_t {
    private:
        gzFile gzStaticTraceFile;
        gzFile gzDynamicTraceFile;
        gzFile gzMemoryTraceFile;
        int max_ops_window;
        int current_ops;
        int total_cost;
        int vaults[32];
        int print;
        memory_t* mem;
        operation_t* ops[32];

    public:
        mem_opt_t();
        ~mem_opt_t();
        void allocate(char const *trace_file_name);
        int nextLine(operation_t* op);
        int loadOps();
        void bestAllocation();
        void seqAllocation();
        void randomAllocation();
        void naiveAllocation();
        void allocateOp (int op);
        int pickOp();
        void run();
};