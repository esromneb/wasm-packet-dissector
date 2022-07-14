#include "wireshark-compat.h"
#include <proto.h>

#include <iostream>

using namespace::std;

int proto_register_protocol(const char *name, const char *short_name, const char *filter_name) {

    cout << "proto_register_protocol() " << name << "\n";
    return 0;
}
