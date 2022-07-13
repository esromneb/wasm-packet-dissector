/* packet.h
 * Definitions for packet disassembly structures and routines
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __PACKET_H__
#define __PACKET_H__

#include "proto.h"
// #include "tvbuff.h"
// #include "epan.h"
// #include "value_string.h"
// #include "frame_data.h"
#include "packet_info.h"
// #include "column-utils.h"
// #include "guid-utils.h"
// #include "tfs.h"
// #include "unit_strings.h"
#include "ws_symbol_export.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct epan_range;

/** @defgroup packet General Packet Dissection
 *
 * @{
 */

#define hi_nibble(b) (((b) & 0xf0) >> 4)
#define lo_nibble(b) ((b) & 0x0f)

/* Useful when you have an array whose size you can tell at compile-time */
#define array_length(x)	(sizeof x / sizeof x[0])

/* Check whether the "len" bytes of data starting at "offset" is
 * entirely inside the captured data for this packet. */
#define	BYTES_ARE_IN_FRAME(offset, captured_len, len) \
	((guint)(offset) + (guint)(len) > (guint)(offset) && \
	 (guint)(offset) + (guint)(len) <= (guint)(captured_len))

extern void packet_init(void);
extern void packet_cache_proto_handles(void);
extern void packet_cleanup(void);

/* Handle for dissectors you call directly or register with "dissector_add_uint()".
   This handle is opaque outside of "packet.c". */
struct dissector_handle;
typedef struct dissector_handle *dissector_handle_t;

/* Hash table for matching unsigned integers, or strings, and dissectors;
   this is opaque outside of "packet.c". */
struct dissector_table;
typedef struct dissector_table *dissector_table_t;

/*
 * Dissector that returns:
 *
 *	The amount of data in the protocol's PDU, if it was able to
 *	dissect all the data;
 *
 *	0, if the tvbuff doesn't contain a PDU for that protocol;
 *
 *	The negative of the amount of additional data needed, if
 *	we need more data (e.g., from subsequent TCP segments) to
 *	dissect the entire PDU.
 */
typedef int (*dissector_t)(tvbuff_t *, packet_info *, proto_tree *, void *);

/* Same as dissector_t with an extra parameter for callback pointer */
typedef int (*dissector_cb_t)(tvbuff_t *, packet_info *, proto_tree *, void *, void *);






/* Add an entry to a uint dissector table. */
WS_DLL_PUBLIC void dissector_add_uint(const char *name, const guint32 pattern,
    dissector_handle_t handle);








/** Create an anonymous handle for a dissector. */
WS_DLL_PUBLIC dissector_handle_t create_dissector_handle(dissector_t dissector,
    const int proto);










#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* packet.h */
