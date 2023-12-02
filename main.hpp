#pragma once

#include <stdio.h>
#include <stdlib.h>


#include <vector>
#include <string>

using namespace std;

struct Params {
    int num_threads = 1;
};

void read_arguments(int, char **, Params&);

void read_input_file(string);