#include "wireshark-compat.h"
#include <proto.h>
#include <expert.h>

#include <iostream>
#include <vector>
#include <memory>

gboolean wireshark_abort_on_dissector_bug = false;

/* Structure stored for deregistered g_slice */
struct g_slice_data {
    gsize    block_size;
    gpointer mem_block;
};

/* Deregistered fields */
static GPtrArray *deregistered_fields = NULL;
static GPtrArray *deregistered_data = NULL;
static GPtrArray *deregistered_slice = NULL;

/* indexed by prefix, contains initializers */
static GHashTable* prefixes = NULL;

/* Contains information about a field when a dissector calls
 * proto_tree_add_item.  */
#define FIELD_INFO_NEW(pool, fi)  fi = wmem_new(pool, field_info)
#define FIELD_INFO_FREE(pool, fi) wmem_free(pool, fi)

/* Contains the space for proto_nodes. */
#define PROTO_NODE_INIT(node)           \
    node->first_child = NULL;       \
    node->last_child = NULL;        \
    node->next = NULL;

#define PROTO_NODE_FREE(pool, node)         \
    wmem_free(pool, node)

/* String space for protocol and field items for the GUI */
#define ITEM_LABEL_NEW(pool, il)            \
    il = wmem_new(pool, item_label_t);
#define ITEM_LABEL_FREE(pool, il)           \
    wmem_free(pool, il);

#define PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo)                        \
    if((guint)hfindex >= gpa_hfinfo.len && wireshark_abort_on_dissector_bug)    \
        g_error("Unregistered hf! index=%d", hfindex);                  \
    DISSECTOR_ASSERT_HINT((guint)hfindex < gpa_hfinfo.len, "Unregistered hf!"); \
    DISSECTOR_ASSERT_HINT(gpa_hfinfo.hfi[hfindex] != NULL, "Unregistered hf!"); \
    hfinfo = gpa_hfinfo.hfi[hfindex];

/* List which stores protocols and fields that have been registered */
typedef struct _gpa_hfinfo_t {
    guint32             len;
    guint32             allocated_len;
    header_field_info **hfi;
} gpa_hfinfo_t;

static gpa_hfinfo_t gpa_hfinfo;

/* Hash table of abbreviations and IDs */
static GHashTable *gpa_name_map = NULL;
static header_field_info *same_name_hfinfo;

/* Hash table protocol aliases. const char * -> const char * */
static GHashTable *gpa_protocol_aliases = NULL;

/*
 * We're called repeatedly with the same field name when sorting a column.
 * Cache our last gpa_name_map hit for faster lookups.
 */
static char *last_field_name = NULL;
static header_field_info *last_hfinfo;




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



/* chars allowed in field abbrev: alphanumerics, '-', "_", and ".". */
static
const guint8 fld_abbrev_chars[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x00-0x0F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x10-0x1F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, /* 0x20-0x2F '-', '.'      */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, /* 0x30-0x3F '0'-'9'       */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x40-0x4F 'A'-'O'       */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /* 0x50-0x5F 'P'-'Z', '_' */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x60-0x6F 'a'-'o'       */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* 0x70-0x7F 'p'-'z'       */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x80-0x8F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x90-0x9F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xA0-0xAF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xB0-0xBF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xC0-0xCF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xD0-0xDF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xE0-0xEF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xF0-0xFF */
};



void proto_report_dissector_bug(const char *format, ...)
{
    va_list args;

    if (wireshark_abort_on_dissector_bug) {
        /*
         * Try to have the error message show up in the crash
         * information.
         */
        // va_start(args, format);
        // ws_vadd_crash_info(format, args);
        // va_end(args);

        /*
         * Print the error message.
         */
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        putc('\n', stderr);

        /*
         * And crash.
         */
        abort();
    } else {
        // va_start(args, format);
        // VTHROW_FORMATTED(DissectorError, format, args);
        // va_end(args);

        // va_start(args, format);
        // ws_vadd_crash_info(format, args);
        // va_end(args);

        /*
         * Print the error message.
         */
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        putc('\n', stderr);
    }
}





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

protocol_t *
find_protocol_by_id(const int proto_id)
{
    // protocol_t* q;
    // q = &*(protocols[0]);

    return &*(protocols[0]);

    // return protocols[0];
    // header_field_info *hfinfo;

    // if (proto_id < 0)
    //     return NULL;

    // PROTO_REGISTRAR_GET_NTH(proto_id, hfinfo);
    // if (hfinfo->type != FT_PROTOCOL) {
    //     DISSECTOR_ASSERT(hfinfo->display & BASE_PROTOCOL_INFO);
    // }
    // return (protocol_t *)hfinfo->strings;
}






// how big is this, and what do we realistically use?
// #define PROTO_PRE_ALLOC_HF_FIELDS_MEM (220000+PRE_ALLOC_EXPERT_FIELDS_MEM)
#define PROTO_PRE_ALLOC_HF_FIELDS_MEM (1000+PRE_ALLOC_EXPERT_FIELDS_MEM)
static int
proto_register_field_init(header_field_info *hfinfo, const int parent)
{
    return 0;

    // tmp_fld_check_assert(hfinfo);

    hfinfo->parent         = parent;
    hfinfo->same_name_next = NULL;
    hfinfo->same_name_prev_id = -1;

    /* if we always add and never delete, then id == len - 1 is correct */
    if (gpa_hfinfo.len >= gpa_hfinfo.allocated_len) {
        if (!gpa_hfinfo.hfi) {
            gpa_hfinfo.allocated_len = PROTO_PRE_ALLOC_HF_FIELDS_MEM;
            gpa_hfinfo.hfi = (header_field_info **)g_malloc(sizeof(header_field_info *)*PROTO_PRE_ALLOC_HF_FIELDS_MEM);
        } else {
            gpa_hfinfo.allocated_len += 1000;
            gpa_hfinfo.hfi = (header_field_info **)g_realloc(gpa_hfinfo.hfi,
                           sizeof(header_field_info *)*gpa_hfinfo.allocated_len);
            /*g_warning("gpa_hfinfo.allocated_len %u", gpa_hfinfo.allocated_len);*/
        }
    }
    gpa_hfinfo.hfi[gpa_hfinfo.len] = hfinfo;
    gpa_hfinfo.len++;
    hfinfo->id = gpa_hfinfo.len - 1;

    /* if we have real names, enter this field in the name tree */
    if ((hfinfo->name[0] != 0) && (hfinfo->abbrev[0] != 0 )) {

        header_field_info *same_name_next_hfinfo;
        guchar c;

        /* Check that the filter name (abbreviation) is legal;
         * it must contain only alphanumerics, '-', "_", and ".". */
        c = proto_check_field_name(hfinfo->abbrev);
        if (c) {
            if (c == '.') {
                fprintf(stderr, "Invalid leading, duplicated or trailing '.' found in filter name '%s'\n", hfinfo->abbrev);
            } else if (g_ascii_isprint(c)) {
                fprintf(stderr, "Invalid character '%c' in filter name '%s'\n", c, hfinfo->abbrev);
            } else {
                fprintf(stderr, "Invalid byte \\%03o in filter name '%s'\n", c, hfinfo->abbrev);
            }
            DISSECTOR_ASSERT_NOT_REACHED();
        }

        /* We allow multiple hfinfo's to be registered under the same
         * abbreviation. This was done for X.25, as, depending
         * on whether it's modulo-8 or modulo-128 operation,
         * some bitfield fields may be in different bits of
         * a byte, and we want to be able to refer to that field
         * with one name regardless of whether the packets
         * are modulo-8 or modulo-128 packets. */

        same_name_hfinfo = NULL;

        g_hash_table_insert(gpa_name_map, (gpointer) (hfinfo->abbrev), hfinfo);
        /* GLIB 2.x - if it is already present
         * the previous hfinfo with the same name is saved
         * to same_name_hfinfo by value destroy callback */
        if (same_name_hfinfo) {
            /* There's already a field with this name.
             * Put the current field *before* that field
             * in the list of fields with this name, Thus,
             * we end up with an effectively
             * doubly-linked-list of same-named hfinfo's,
             * with the head of the list (stored in the
             * hash) being the last seen hfinfo.
             */
            same_name_next_hfinfo =
                same_name_hfinfo->same_name_next;

            hfinfo->same_name_next = same_name_next_hfinfo;
            if (same_name_next_hfinfo)
                same_name_next_hfinfo->same_name_prev_id = hfinfo->id;

            same_name_hfinfo->same_name_next = hfinfo;
            hfinfo->same_name_prev_id = same_name_hfinfo->id;
#ifdef ENABLE_CHECK_FILTER
            while (same_name_hfinfo) {
                if (_ftype_common(hfinfo->type) != _ftype_common(same_name_hfinfo->type))
                    fprintf(stderr, "'%s' exists multiple times with NOT compatible types: %s and %s\n", hfinfo->abbrev, ftype_name(hfinfo->type), ftype_name(same_name_hfinfo->type));
                same_name_hfinfo = same_name_hfinfo->same_name_next;
            }
#endif
        }
    }

    return hfinfo->id;
}





static int
proto_register_field_common(protocol_t *proto, header_field_info *hfi, const int parent)
{
    if (proto != NULL) {
        g_ptr_array_add(proto->fields, hfi);
    }

    return proto_register_field_init(hfi, parent);
}

/* for use with static arrays only, since we don't allocate our own copies
of the header_field_info struct contained within the hf_register_info struct */
void
proto_register_field_array(const int parent, hf_register_info *hf, const int num_records)
{
   hf_register_info *ptr = hf;
   protocol_t   *proto;
   int       i;

   proto = find_protocol_by_id(parent);

   if (proto->fields == NULL) {
       proto->fields = g_ptr_array_sized_new(num_records);
   }

   for (i = 0; i < num_records; i++, ptr++) {
//        /*
//         * Make sure we haven't registered this yet.
//         * Most fields have variables associated with them
//         * that are initialized to -1; some have array elements,
//         * or possibly uninitialized variables, so we also allow
//         * 0 (which is unlikely to be the field ID we get back
//         * from "proto_register_field_init()").
//         */
       if (*ptr->p_id != -1 && *ptr->p_id != 0) {
           fprintf(stderr,
               "Duplicate field detected in call to proto_register_field_array: %s is already registered\n",
               ptr->hfinfo.abbrev);
           return;
       }

       *ptr->p_id = proto_register_field_common(proto, &ptr->hfinfo, parent);
   }
}


guchar
proto_check_field_name(const gchar *field_name)
{
    const char *p = field_name;
    guchar c = '.', lastc;

    do {
        lastc = c;
        c = *(p++);
        /* Leading '.' or substring ".." are disallowed. */
        if (c == '.' && lastc == '.') {
            break;
        }
    } while (fld_abbrev_chars[c]);

    /* Trailing '.' is disallowed. */
    if (lastc == '.') {
        return '.';
    }
    return c;
}