#include <stdio.h>
#include <stdint.h>


#include <iostream>
#include <vector>
#include <chrono>
#include <unistd.h> //usleep
#include <functional>
#include <cmath>


#include <stdio.h>
#include <SDL/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include "wireshark.h"

using namespace std;

extern "C" {
void wrapper(void) {
    int val = _wireshark_shared(0);

    cout << "Wireshark link says" << val << "\n";
}
}

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    cout << "Hello World\n";
     wrapper();


    return 0;
}




///
/// These are some of the Javascript -> C++ functions
///

extern "C" {

void printAThing(void) {
    cout << "a thing\n";
}

} // extern C





