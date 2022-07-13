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

// #include "proto.h"
// #include "tvbuff.h"
// #include "epan.h"
// #include "value_string.h"
// #include "frame_data.h"
#include "packet_info.h"
// #include "column-utils.h"
// #include "guid-utils.h"
// #include "tfs.h"
// #include "unit_strings.h"
// #include "ws_symbol_export.h"

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


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* packet.h */
