#include <stdio.h>
#include <stdlib.h>


#include <vector>
#include <string>

using namespace std;

struct Params {
    int readers = 1;
    int writers = 7;
    int reads = 1000000;
    int writes = 1000000;
    void* data_structure;
};

void read_arguments(int, char **, Params&);

void read_input_file(string);