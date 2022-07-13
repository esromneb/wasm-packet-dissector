/* value_string.h
 * Definitions for value_string structures and routines
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __VALUE_STRING_H__
#define __VALUE_STRING_H__


#include "ws_symbol_export.h"
// #include "wmem/wmem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* VALUE TO STRING MATCHING */

typedef struct _value_string {
    guint32      value;
    const gchar *strptr;
} value_string;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VALUE_STRING_H__ */

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
