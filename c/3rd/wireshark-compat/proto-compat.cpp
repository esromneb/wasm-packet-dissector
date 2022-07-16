#include "wireshark-compat.h"
#include <proto.h>
#include <expert.h>

#include <iostream>
#include <vector>
#include <memory>

#include "register.h"



#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <float.h>
#include <errno.h>

// #include <wsutil/bits_ctz.h>
// #include <wsutil/bits_count_ones.h>
// #include <wsutil/sign_ext.h>
// #include <wsutil/utf8_entities.h>
// #include <wsutil/json_dumper.h>

// #include <ftypes/ftypes-int.h>

// #include "packet.h"
// #include "exceptions.h"
// #include "ptvcursor.h"
// #include "strutil.h"
// #include "addr_resolv.h"
#include "address_types.h"
// #include "oids.h"
// #include "proto.h"
// #include "epan_dissect.h"
// #include "tvbuff.h"
// #include "wmem/wmem.h"
// #include "charsets.h"
// #include "column-utils.h"
// #include "to_str-int.h"
// #include "to_str.h"
// #include "osi-utils.h"
// #include "expert.h"
// #include "show_exception.h"
// #include "in_cksum.h"
#include "register-int.h"




/* Ptvcursor limits */
#define SUBTREE_ONCE_ALLOCATION_NUMBER 8
#define SUBTREE_MAX_LEVELS 256

/* Throw an exception if our tree exceeds these. */
/* XXX - These should probably be preferences */
#define MAX_TREE_ITEMS (1 * 1000 * 1000)
#define MAX_TREE_LEVELS (5 * 100)

typedef struct __subtree_lvl {
    gint        cursor_offset;
    proto_item *it;
    proto_tree *tree;
} subtree_lvl;

gboolean wireshark_abort_on_dissector_bug = false;







































































































































static void proto_cleanup_base(void);











































































































static int proto_register_field_init(header_field_info *hfinfo, const int parent);

/* special-case header field used within proto.c */
static header_field_info hfi_text_only =
    { "Text item",  "text", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL };
int hf_text_only = -1;

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

/* List of all protocols */
static GList *protocols = NULL;

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

static void save_same_name_hfinfo(gpointer data)
{
    same_name_hfinfo = (header_field_info*)data;
}

/* Points to the first element of an array of bits, indexed by
   a subtree item type; that array element is TRUE if subtrees of
   an item of that type are to be expanded. */
static guint32 *tree_is_expanded;

/* Number of elements in that array. */
int     num_tree_types;

/* Name hashtables for fast detection of duplicate names */
static GHashTable* proto_names        = NULL;
static GHashTable* proto_short_names  = NULL;
static GHashTable* proto_filter_names = NULL;

static gint
proto_compare_name(gconstpointer p1_arg, gconstpointer p2_arg)
{
    const protocol_t *p1 = (const protocol_t *)p1_arg;
    const protocol_t *p2 = (const protocol_t *)p2_arg;

    return g_ascii_strcasecmp(p1->short_name, p2->short_name);
}

#ifdef HAVE_PLUGINS
static GSList *dissector_plugins = NULL;

void
proto_register_plugin(const proto_plugin *plug)
{
    if (!plug) {
        /* XXX print useful warning */
        return;
    }
    dissector_plugins = g_slist_prepend(dissector_plugins, (proto_plugin *)plug);
}

static void
call_plugin_register_protoinfo(gpointer data, gpointer user_data _U_)
{
    proto_plugin *plug = (proto_plugin *)data;

    if (plug->register_protoinfo) {
        plug->register_protoinfo();
    }
}

static void
call_plugin_register_handoff(gpointer data, gpointer user_data _U_)
{
    proto_plugin *plug = (proto_plugin *)data;

    if (plug->register_handoff) {
        plug->register_handoff();
    }
}
#endif /* HAVE_PLUGINS */


/* initialize data structures and register protocols and fields */
void
proto_init(GSList *register_all_plugin_protocols_list,
       GSList *register_all_plugin_handoffs_list,
       register_cb cb,
       gpointer client_data)
{
    proto_cleanup_base();

    proto_names        = g_hash_table_new(g_str_hash, g_str_equal);
    proto_short_names  = g_hash_table_new(g_str_hash, g_str_equal);
    proto_filter_names = g_hash_table_new(g_str_hash, g_str_equal);

    gpa_hfinfo.len           = 0;
    gpa_hfinfo.allocated_len = 0;
    gpa_hfinfo.hfi           = NULL;
    gpa_name_map             = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, save_same_name_hfinfo);
    gpa_protocol_aliases     = g_hash_table_new(g_str_hash, g_str_equal);
    deregistered_fields      = g_ptr_array_new();
    deregistered_data        = g_ptr_array_new();
    deregistered_slice       = g_ptr_array_new();

    /* Initialize the ftype subsystem */
    ftypes_initialize();

    /* Initialize the addres type subsystem */
    address_types_initialize();

    /* Register one special-case FT_TEXT_ONLY field for use when
       converting wireshark to new-style proto_tree. These fields
       are merely strings on the GUI tree; they are not filterable */
    hf_text_only = proto_register_field_init(&hfi_text_only, -1);

    /* Register the pseudo-protocols used for exceptions. */
    // register_show_exception();
    // register_type_length_mismatch();
    // register_number_string_decoding_error();
    // register_string_errors();

    /* Have each built-in dissector register its protocols, fields,
       dissector tables, and dissectors to be called through a
       handle, and do whatever one-time initialization it needs to
       do. */
    register_all_protocols(cb, client_data);

    /* Now call the registration routines for all epan plugins. */
    for (GSList *l = register_all_plugin_protocols_list; l != NULL; l = l->next) {
        ((void (*)(register_cb, gpointer))l->data)(cb, client_data);
    }

#ifdef HAVE_PLUGINS
    /* Now call the registration routines for all dissector plugins. */
    if (cb)
        (*cb)(RA_PLUGIN_REGISTER, NULL, client_data);
    g_slist_foreach(dissector_plugins, call_plugin_register_protoinfo, NULL);
#endif

    /* Now call the "handoff registration" routines of all built-in
       dissectors; those routines register the dissector in other
       dissectors' handoff tables, and fetch any dissector handles
       they need. */
    register_all_protocol_handoffs(cb, client_data);

    /* Now do the same with epan plugins. */
    for (GSList *l = register_all_plugin_handoffs_list; l != NULL; l = l->next) {
        ((void (*)(register_cb, gpointer))l->data)(cb, client_data);
    }

#ifdef HAVE_PLUGINS
    /* Now do the same with dissector plugins. */
    if (cb)
        (*cb)(RA_PLUGIN_HANDOFF, NULL, client_data);
    g_slist_foreach(dissector_plugins, call_plugin_register_handoff, NULL);
#endif

    /* sort the protocols by protocol name */
    protocols = g_list_sort(protocols, proto_compare_name);

    /* We've assigned all the subtree type values; allocate the array
       for them, and zero it out. */
    tree_is_expanded = g_new0(guint32, (num_tree_types/32)+1);
}

static void
proto_cleanup_base(void)
{
    protocol_t *protocol;
    header_field_info *hfinfo;

    /* Free the abbrev/ID hash table */
    if (gpa_name_map) {
        g_hash_table_destroy(gpa_name_map);
        gpa_name_map = NULL;
    }
    if (gpa_protocol_aliases) {
        g_hash_table_destroy(gpa_protocol_aliases);
        gpa_protocol_aliases = NULL;
    }
    g_free(last_field_name);
    last_field_name = NULL;

    while (protocols) {
        protocol = (protocol_t *)protocols->data;
        PROTO_REGISTRAR_GET_NTH(protocol->proto_id, hfinfo);
        DISSECTOR_ASSERT(protocol->proto_id == hfinfo->id);

        g_slice_free(header_field_info, hfinfo);
        if (protocol->parent_proto_id != -1) {
            // pino protocol
            DISSECTOR_ASSERT(protocol->fields == NULL); //helpers should not have any registered fields
            DISSECTOR_ASSERT(protocol->heur_list == NULL); //helpers should not have a heuristic list
        } else {
            if (protocol->fields) {
                g_ptr_array_free(protocol->fields, TRUE);
            }
            g_list_free(protocol->heur_list);
        }
        protocols = g_list_remove(protocols, protocol);
        g_free(protocol);
    }

    if (proto_names) {
        g_hash_table_destroy(proto_names);
        proto_names = NULL;
    }

    if (proto_short_names) {
        g_hash_table_destroy(proto_short_names);
        proto_short_names = NULL;
    }

    if (proto_filter_names) {
        g_hash_table_destroy(proto_filter_names);
        proto_filter_names = NULL;
    }

    if (gpa_hfinfo.allocated_len) {
        gpa_hfinfo.len           = 0;
        gpa_hfinfo.allocated_len = 0;
        g_free(gpa_hfinfo.hfi);
        gpa_hfinfo.hfi           = NULL;
    }

    if (deregistered_fields) {
        g_ptr_array_free(deregistered_fields, TRUE);
        deregistered_fields = NULL;
    }

    if (deregistered_data) {
        g_ptr_array_free(deregistered_data, TRUE);
        deregistered_data = NULL;
    }

    if (deregistered_slice) {
        g_ptr_array_free(deregistered_slice, TRUE);
        deregistered_slice = NULL;
    }

    g_free(tree_is_expanded);
    tree_is_expanded = NULL;

    if (prefixes)
        g_hash_table_destroy(prefixes);
}

void
proto_cleanup(void)
{
    proto_free_deregistered_fields();
    proto_cleanup_base();

#ifdef HAVE_PLUGINS
    g_slist_free(dissector_plugins);
    dissector_plugins = NULL;
#endif
}





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

#if 0
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
#endif

protocol_t *
find_protocol_by_id(const int proto_id)
{
    header_field_info *hfinfo;

    if (proto_id < 0)
        return NULL;

    PROTO_REGISTRAR_GET_NTH(proto_id, hfinfo);
    if (hfinfo->type != FT_PROTOCOL) {
        DISSECTOR_ASSERT(hfinfo->display & BASE_PROTOCOL_INFO);
    }
    return (protocol_t *)hfinfo->strings;
}

// protocol_t *
// find_protocol_by_id(const int proto_id)
// {
//     // protocol_t* q;
//     // q = &*(protocols[0]);

//     return &*(protocols[0]);

//     // return protocols[0];
//     // header_field_info *hfinfo;

//     // if (proto_id < 0)
//     //     return NULL;

//     // PROTO_REGISTRAR_GET_NTH(proto_id, hfinfo);
//     // if (hfinfo->type != FT_PROTOCOL) {
//     //     DISSECTOR_ASSERT(hfinfo->display & BASE_PROTOCOL_INFO);
//     // }
//     // return (protocol_t *)hfinfo->strings;
// }






// how big is this, and what do we realistically use?
// #define PROTO_PRE_ALLOC_HF_FIELDS_MEM (220000+PRE_ALLOC_EXPERT_FIELDS_MEM)
#define PROTO_PRE_ALLOC_HF_FIELDS_MEM (1000+PRE_ALLOC_EXPERT_FIELDS_MEM)
static int
proto_register_field_init(header_field_info *hfinfo, const int parent)
{
    // return 0;

    // tmp_fld_check_assert(hfinfo);

    hfinfo->parent         = parent;
    hfinfo->same_name_next = NULL;
    hfinfo->same_name_prev_id = -1;

    cout << gpa_hfinfo.len << "\n";
    cout << gpa_hfinfo.allocated_len << "\n";

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
    cout << "proto_register_field_array()\n";
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



























































































static void
check_valid_filter_name_or_fail(const char *filter_name)
{
    gboolean found_invalid = proto_check_field_name(filter_name);

    /* Additionally forbid upper case characters. */
    if (!found_invalid) {
        for (guint i = 0; filter_name[i]; i++) {
            if (g_ascii_isupper(filter_name[i])) {
                found_invalid = TRUE;
                break;
            }
        }
    }

    if (found_invalid) {
        g_error("Protocol filter name \"%s\" has one or more invalid characters."
            " Allowed are lower characters, digits, '-', '_' and non-repeating '.'."
            " This might be caused by an inappropriate plugin or a development error.", filter_name);
    }
}

int
proto_register_protocol(const char *name, const char *short_name,
            const char *filter_name)
{
    protocol_t *protocol;
    header_field_info *hfinfo;

    cout << "proto_register_protocol()\n";


    /*
     * Make sure there's not already a protocol with any of those
     * names.  Crash if there is, as that's an error in the code
     * or an inappropriate plugin.
     * This situation has to be fixed to not register more than one
     * protocol with the same name.
     */

    cout << "AAAAAA" << std::endl;
    if (g_hash_table_lookup(proto_names, name)) {
        /* g_error will terminate the program */
        g_error("Duplicate protocol name \"%s\"!"
            " This might be caused by an inappropriate plugin or a development error.", name);
    }

    if (g_hash_table_lookup(proto_short_names, short_name)) {
        g_error("Duplicate protocol short_name \"%s\"!"
            " This might be caused by an inappropriate plugin or a development error.", short_name);
    }

    check_valid_filter_name_or_fail(filter_name);

    if (g_hash_table_lookup(proto_filter_names, filter_name)) {
        g_error("Duplicate protocol filter_name \"%s\"!"
            " This might be caused by an inappropriate plugin or a development error.", filter_name);
    }

    /*
     * Add this protocol to the list of known protocols;
     * the list is sorted by protocol short name.
     */
    protocol = g_new(protocol_t, 1);
    protocol->name = name;
    protocol->short_name = short_name;
    protocol->filter_name = filter_name;
    protocol->fields = NULL; /* Delegate until actually needed */
    protocol->is_enabled = TRUE; /* protocol is enabled by default */
    protocol->enabled_by_default = TRUE; /* see previous comment */
    protocol->can_toggle = TRUE;
    protocol->parent_proto_id = -1;
    protocol->heur_list = NULL;

    /* List will be sorted later by name, when all protocols completed registering */
    protocols = g_list_prepend(protocols, protocol);
    g_hash_table_insert(proto_names, (gpointer)name, protocol);
    g_hash_table_insert(proto_filter_names, (gpointer)filter_name, protocol);
    g_hash_table_insert(proto_short_names, (gpointer)short_name, protocol);

    /* Here we allocate a new header_field_info struct */
    hfinfo = g_slice_new(header_field_info);
    hfinfo->name = name;
    hfinfo->abbrev = filter_name;
    hfinfo->type = FT_PROTOCOL;
    hfinfo->display = BASE_NONE;
    hfinfo->strings = protocol;
    hfinfo->bitmask = 0;
    hfinfo->ref_type = HF_REF_TYPE_NONE;
    hfinfo->blurb = NULL;
    hfinfo->parent = -1; /* This field differentiates protos and fields */

    protocol->proto_id = proto_register_field_init(hfinfo, hfinfo->parent);
    return protocol->proto_id;
}