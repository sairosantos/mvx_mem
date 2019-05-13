#include "memory.hpp"
#include "main.hpp"

mem_opt_t::mem_opt_t(){
    mem = new memory_t;
    print = 0;
    srand (time (NULL));
};

mem_opt_t::~mem_opt_t(){

};

void mem_opt_t::allocate (char const *trace_file){
    char file_name[512];
    file_name[0] = '\0';
    snprintf(file_name, sizeof(file_name), "%s.tid%d.mem.out.gz", trace_file, 0);
    this->gzMemoryTraceFile = gzopen(file_name, "ro");    /// Open the .gz group
};

/* Carrega até 32 instruções (RRW), retorna o número de instruções carregadas */
int mem_opt_t::loadOps(){
    int no_ops = 0;
    while (no_ops < 32) {
        ops[no_ops] = new operation_t;
        int r = nextLine(ops[no_ops]);
        if (r == -1) break;
        else if (r != 0) no_ops++;
    }
    if (print) printf ("Carregando %d operações.\n", no_ops);
    return no_ops;
}

/*Carrega a próxima instrução no arquivo. Retorna -1 ao fim do arquivo. */
int mem_opt_t::nextLine(operation_t* op) {
    uint64_t mem_address_read1;
    uint64_t mem_address_read2;
    uint64_t mem_address_write;
    uint32_t mem_size;
    bool mem_is_read;

    static char file_line[512];
    file_line[0] = '\0';

    if (gzeof(this->gzMemoryTraceFile)) {
        return -1;
    }
    char *buffer = gzgets(this->gzMemoryTraceFile, file_line, 512);
    if (buffer == NULL) return -1;

    /// Analyze the trace line
    if (file_line[0] == '\0' || file_line[0] == '#') {
        if (print) printf ("Memory trace line (empty/comment): %s\n", file_line);
        return 0;
    }
    else {
        char *sub_string = NULL;
        char *tmp_ptr = NULL;
        uint32_t count = 0, i = 0;
        while (file_line[i] != '\0') {
            count += (file_line[i] == ' ');
            i++;
        }
          
        sub_string = strtok_r(file_line, " ", &tmp_ptr);
        mem_is_read = strcmp(sub_string, "R") == 0;

        sub_string = strtok_r(NULL, " ", &tmp_ptr);
        mem_size = strtoull(sub_string, NULL, 10);

        sub_string = strtok_r(NULL, " ", &tmp_ptr);
        mem_address_read1 = strtoull(sub_string, NULL, 10);

        sub_string = strtok_r(NULL, " ", &tmp_ptr);
        mem_address_read2 = strtoull(sub_string, NULL, 10);

        sub_string = strtok_r(NULL, " ", &tmp_ptr);
        mem_address_write = strtoull(sub_string, NULL, 10);

        sub_string = strtok_r(NULL, " ", &tmp_ptr);

        op->read_address_1 = mem_address_read1;
        op->read_address_2 = mem_address_read2;
        op->write_address = mem_address_write;

        if (print) printf ("%lu %lu %lu\n", mem_address_read1, mem_address_read2, mem_address_write);
    }
    return 1;
};

/* Considera, dentre todas as operações que ainda não foram alocadas, qual 
adicionará o menor custo ao ser alocada a um dos vaults livres. */
int mem_opt_t::pickOp() {
    int lowest_cost = 9999;
    int lowest_op = -1;
    for (int i = 0; i < current_ops; i++){
        if (ops[i]->allocated == 0){
            for (int j = 0; j < 32; j++){
                int cost = mem->estimateTotalCost (ops[i], j);
                if (cost < lowest_cost && cost != -1){
                    lowest_cost = cost;
                    lowest_op = i;
                }
            }
        }
    }
    return lowest_op;
}

void mem_opt_t::allocateOp (int op){
    int best_cost = 9999;
    int best_allocation = -1;
    operation_t* aux_op = ops[op];
    for (int j = 0; j < 32; j++){
        int result = mem->estimateTotalCost(aux_op, j);
        if (result < best_cost && result != -1) {
            best_cost = result;
            best_allocation = j;
        }
    }
    mem->allocate (ops[op], best_allocation);
};

void mem_opt_t::bestAllocation(){
    for (int i = 0; i < current_ops; i++) allocateOp (pickOp());

    int total_ciclo = mem->evaluate();
    total_cost += total_ciclo;
};

void mem_opt_t::seqAllocation() {
    for (int i = 0; i < 32; i++) vaults[i] = -1;
    for (int i = 0; i < current_ops; i++){
        mem->allocate (ops[i], i);
    }

    int total_ciclo = mem->evaluate();
    total_cost += total_ciclo;
};

void mem_opt_t::randomAllocation() {
    for (int i = 0; i < current_ops; i++){
        int random;
        do {
            random = rand() % 32;    
        } while (mem->vaults[random]->allocated != 0);
        mem->allocate (ops[i], random);
    }

    int total_ciclo = mem->evaluate();
    total_cost += total_ciclo;
};

void mem_opt_t::naiveAllocation() {
    for (int i = 0; i < 32; i++) vaults[i] = -1;
    for (int i = 0; i < current_ops; i++){
        for (int j = 0; j < 32; j++){
            if ((ops[i]->vaultR1() == j || ops[i]->vaultR2() == j || ops[i]->vaultW() == j) && vaults[j] == -1){
                mem->allocate (ops[i], j);
                if (print) printf ("NAIVE: Alocamos a operação %d no vault %d. Custo até agora: %d.\n", i, j, mem->evaluate());
            }
        }
        
        if (ops[i]->allocated == 0) {
            for (int j = 0; j < 32; j++){
                if (vaults[j] == -1) {
                    mem->allocate (ops[i], j);
                    if (print) printf ("FIRST: Alocamos a operação %d no vault %d. Custo até agora: %d.\n", i, j, mem->evaluate());
                }
            }
        }
    }

    int total_ciclo = mem->evaluate();
    total_cost += total_ciclo;
};

void mem_opt_t::run() {
    char const *arquivo = "matmul.1MB.1t";
    allocate (arquivo);
    while ((current_ops = loadOps()) != 0) {
        bestAllocation();
        //mem->printLinks();
        mem->reset();
    }
    printf ("BEST -> Custo total: %d ciclos.\n", total_cost);

    total_cost = 0;
    allocate (arquivo);
    while ((current_ops = loadOps()) != 0) {
        randomAllocation();
        //mem->printLinks();
        mem->reset();
    }
    printf ("RAND -> Custo total: %d ciclos.\n", total_cost);
    total_cost = 0;
    allocate (arquivo);
    while ((current_ops = loadOps()) != 0) {
        seqAllocation();
        //mem->printLinks();
        mem->reset();
    }
    printf ("SEQU -> Custo total: %d ciclos.\n", total_cost);
    total_cost = 0;
    allocate (arquivo);
    while ((current_ops = loadOps()) != 0) {
        naiveAllocation();
        //mem->printLinks();
        mem->reset();
    }
    printf ("NAIV -> Custo total: %d ciclos.\n", total_cost);
}

int main(int argc, char **argv){
    mem_opt_t *reader = new mem_opt_t;
    reader->run();

    return 0;
}