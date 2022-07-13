#include "config.h"

#include <epan/packet.h>

#include <stdio.h>

#define FOO_PORT 1234

static int proto_foo = -1;

static int hf_foo_pdu_type = -1;
static gint ett_foo = -1;

static int hf_foo_flags = -1;
static int hf_foo_sequenceno = -1;
static int hf_foo_initialip = -1;

static const value_string packettypenames[] = {
    { 1, "Initialise" },
    { 2, "Terminate" },
    { 3, "Data" },
    { 0, NULL }
};

#define FOO_START_FLAG      0x01
#define FOO_END_FLAG        0x02
#define FOO_PRIORITY_FLAG   0x04

static int hf_foo_startflag = -1;
static int hf_foo_endflag = -1;
static int hf_foo_priorityflag = -1;


void
proto_register_foo(void)
{
    // this array gets passed to proto_register_field_array()
    // it will set the static globals above with unique field id (I'm guessing)
    // then later we pass these unique ids to fns in dissect_foo()
    static hf_register_info hf[] = {
        { &hf_foo_pdu_type,
            { "FOO PDU Type", "foo.type",
            FT_UINT8, BASE_DEC,
            VALS(packettypenames), 0x0,
            NULL, HFILL }
        },
        { &hf_foo_flags,
            { "FOO PDU Flags", "foo.flags",
            FT_UINT8, BASE_HEX,
            NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_foo_sequenceno,
            { "FOO PDU Sequence Number", "foo.seqn",
            FT_UINT16, BASE_DEC,
            NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_foo_initialip,
            { "FOO PDU Initial IP", "foo.initialip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_foo_startflag,
            { "FOO PDU Start Flags", "foo.flags.start",
            FT_BOOLEAN, 8,
            NULL, FOO_START_FLAG,
            NULL, HFILL }
        },
        { &hf_foo_endflag,
            { "FOO PDU End Flags", "foo.flags.end",
            FT_BOOLEAN, 8,
            NULL, FOO_END_FLAG,
            NULL, HFILL }
        },
        { &hf_foo_priorityflag,
            { "FOO PDU Priority Flags", "foo.flags.priority",
            FT_BOOLEAN, 8,
            NULL, FOO_PRIORITY_FLAG,
            NULL, HFILL }
        }
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_foo
    };



    proto_foo = proto_register_protocol (
        "FOO Protocol", /* name       */
        "FOO",      /* short name */
        "foo"       /* abbrev     */
        );

    proto_register_field_array(proto_foo, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    // printf("End of proto_register_foo()\n");
    // printf("hf_foo_pdu_type : %d\n", hf_foo_pdu_type);
    // printf("hf_foo_sequenceno : %d\n", hf_foo_sequenceno);
    // printf("hf_foo_initialip : %d\n", hf_foo_initialip);
}

static int
dissect_foo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    (void)tree;
    (void)data;
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "FOO");
    
    // Clear out stuff in the info column
    col_clear(pinfo->cinfo, COL_INFO); 

    // do we need to construct this thing every run?
    static int* const bits[] = {
        &hf_foo_startflag,
        &hf_foo_endflag,
        &hf_foo_priorityflag,
        NULL
    };

    gint offset = 0;

    proto_item *ti = proto_tree_add_item(tree, proto_foo, tvb, 0, -1, ENC_NA);
    proto_tree *foo_tree = proto_item_add_subtree(ti, ett_foo);
    proto_tree_add_item(foo_tree, hf_foo_pdu_type, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    // this gets removed
    // proto_tree_add_item(foo_tree, hf_foo_flags, tvb, offset, 1, ENC_BIG_ENDIAN);
    // replaced by bitfield parser
    proto_tree_add_bitmask(foo_tree, tvb, offset, hf_foo_flags, ett_foo, bits, ENC_BIG_ENDIAN);
    offset += 1;

    proto_tree_add_item(foo_tree, hf_foo_sequenceno, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;
    proto_tree_add_item(foo_tree, hf_foo_initialip, tvb, offset, 4, ENC_BIG_ENDIAN);
    offset += 4;


    return tvb_captured_length(tvb);
}

void
proto_reg_handoff_foo(void)
{
    static dissector_handle_t foo_handle;

    foo_handle = create_dissector_handle(dissect_foo, proto_foo);
    dissector_add_uint("udp.port", FOO_PORT, foo_handle);
}

