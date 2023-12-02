#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <ostream>


#include <vector>


#include "run.hpp"
#include "sequential_barnes_hut.cpp"

using namespace std;

void read_arguments(int argc, char** argv, Params& params) {
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
                params.steps = atoi(optarg);
                break;
            case '?':
                std::cerr << "Usage: " << argv[0] << " -n <n> -i <i> -o <o> -s <s>" << std::endl;
        }
    }
}




int main (int argc, char** argv) {
    Params params;
    read_arguments(argc, argv, params);

    read_input_file()

    srand(0);

    run_seq(params);


}