

int wireshark_shared(const int x) {
    return 42+x;
}

#include <glib.h>








// compat stuff
// glib wants this, I hope it never gets called
#include <pthread.h>

int pthread_attr_setinheritsched(pthread_attr_t *attr,
                                int inheritsched) {

    return 0;
}