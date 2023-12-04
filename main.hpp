#include <stdio.h>
#include <stdlib.h>


#include <vector>
#include <string>

using namespace std;

struct Params {
    int readers = 1;
    int writers = 1;
    int reads = 100000;
    int writes = 100000;
    void* data_structure;
};

void read_arguments(int, char **, Params&);

void read_input_file(string);