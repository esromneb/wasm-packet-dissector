#include <stdio.h>
#include <stdint.h>


#include <iostream>
#include <vector>
#include <chrono>
#include <unistd.h> //usleep
#include <functional>
#include <cmath>


#include <SDL/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include "program_core.hpp"

using namespace std;



extern "C" {
void wrapper_core_shared(const int x) {
    int val = core_shared(x);

    cout << "Wireshark link says " << val << "\n";
}
}

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    cout << "Hello World from main()\n";
    wrapper_core_shared(0);


    return 0;
}




///
/// These are some of the Javascript -> C++ functions
///

extern "C" {

void printAThing(void) {
    cout << "a thing\n";
}

int addFromCpp(const int a, const int b) {
    return a+b;
}


} // extern C






