#include "wireshark-compat.h"
#include <proto.h>

#include <iostream>
#include <vector>
#include <memory>

/* Structure for information about a protocol */
struct _protocol {
    const char *name;               /* long description */
    const char *short_name;         /* short description */
    const char *filter_name;        /* name of this protocol in filters */
    GPtrArray  *fields;             /* fields for this protocol */
    int         proto_id;           /* field ID for this protocol */
    gboolean    is_enabled;         /* TRUE if protocol is enabled */
    gboolean    enabled_by_default; /* TRUE if protocol is enabled by default */
    gboolean    can_toggle;         /* TRUE if is_enabled can be changed */
    int         parent_proto_id;    /* Used to identify "pino"s (Protocol In Name Only).
                                       For dissectors that need a protocol name so they
                                       can be added to a dissector table, but use the
                                       parent_proto_id for things like enable/disable */
    GList      *heur_list;          /* Heuristic dissectors associated with this protocol */
};


using namespace::std;

// consider https://stackoverflow.com/questions/22120413/using-a-vector-of-unique-pointers-to-an-employee-vector

std::vector<unique_ptr<protocol_t>> protocols;

int proto_register_protocol(const char *name, const char *short_name, const char *filter_name) {
    cout << "proto_register_protocol() " << name << "\n";


    /*
     * Add this protocol to the list of known protocols;
     * the list is sorted by protocol short name.
     */
    // protocol = g_new(protocol_t, 1);
    // protocol = (protol_t*) malloc(sizeof(protocol_t))
    unique_ptr<protocol_t> protocol(new protocol_t);
    protocol->name = name;
    protocol->short_name = short_name;
    protocol->filter_name = filter_name;
    protocol->fields = NULL; /* Delegate until actually needed */
    protocol->is_enabled = TRUE; /* protocol is enabled by default */
    protocol->enabled_by_default = TRUE; /* see previous comment */
    protocol->can_toggle = TRUE;
    protocol->parent_proto_id = -1;
    protocol->heur_list = NULL;

    protocols.push_back(std::move(protocol));
    // protocols.push_back(protocol);



    return 0;
}

/* for use with static arrays only, since we don't allocate our own copies
of the header_field_info struct contained within the hf_register_info struct */
void
proto_register_field_array(const int parent, hf_register_info *hf, const int num_records)
{
//    hf_register_info *ptr = hf;
//    protocol_t   *proto;
//    int       i;
//
//    proto = find_protocol_by_id(parent);
//
//    if (proto->fields == NULL) {
//        proto->fields = g_ptr_array_sized_new(num_records);
//    }
//
//    for (i = 0; i < num_records; i++, ptr++) {
//        /*
//         * Make sure we haven't registered this yet.
//         * Most fields have variables associated with them
//         * that are initialized to -1; some have array elements,
//         * or possibly uninitialized variables, so we also allow
//         * 0 (which is unlikely to be the field ID we get back
//         * from "proto_register_field_init()").
//         */
//        if (*ptr->p_id != -1 && *ptr->p_id != 0) {
//            fprintf(stderr,
//                "Duplicate field detected in call to proto_register_field_array: %s is already registered\n",
//                ptr->hfinfo.abbrev);
//            return;
//        }
//
//        *ptr->p_id = proto_register_field_common(proto, &ptr->hfinfo, parent);
//    }
}