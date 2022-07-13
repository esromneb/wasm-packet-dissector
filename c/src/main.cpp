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

using namespace std;

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    cout << "Hello World\n";

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





