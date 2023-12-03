#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <ostream>


#include <vector>


#include "main.hpp"
#include "FreeStack.cpp"

using namespace std;

int main (int argc, char** argv) {

    FreeStack <int> free_stack;

    free_stack.push(1);

    cout << *(free_stack.pop()) << endl;
    cout << "end" << endl;

}