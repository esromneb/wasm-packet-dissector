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

#include "wireshark-compat.h"
#include "epan.h"
#include "wireshark.h"

using namespace std;



std::vector<std::string> registered;

void proto_register_foo(void);

extern "C" {

void doCall(const unsigned i) {
    if( i == 0 ) {
        // epan_init(NULL, NULL, NULL);
    }
    if( i == 1 ) {
        // foo_bar_waz();
    }
}

void registerName(const std::string n) {
    registered.push_back(n);

    if (n == "foo") {
        proto_register_foo();
    }

}

void registerAll(void) {
    registerName("foo");
    // int val = wireshark_shared(x);

    // cout << "Wireshark link says " << val << "\n";
}

}







extern "C" {
void wrapper_wireshark_shared(const int x) {
    int val = wireshark_shared(x);

    cout << "Wireshark link says " << val << "\n";
}
}

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    cout << "Hello World\n";
     wrapper_wireshark_shared(0);


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






