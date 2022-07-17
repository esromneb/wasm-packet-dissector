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
















































































































typedef struct _value_string_ext value_string_ext;
typedef const value_string *(*_value_string_match2_t)(const guint32, value_string_ext*);

struct _value_string_ext {
    _value_string_match2_t _vs_match2;
    guint32                _vs_first_value; /* first value of the value_string array       */
    guint                  _vs_num_entries; /* number of entries in the value_string array */
                                            /*  (excluding final {0, NULL})                */
    const value_string    *_vs_p;           /* the value string array address              */
    const gchar           *_vs_name;        /* vse "Name" (for error messages)             */
};

#define VALUE_STRING_EXT_VS_P(x)           (x)->_vs_p
#define VALUE_STRING_EXT_VS_NUM_ENTRIES(x) (x)->_vs_num_entries
#define VALUE_STRING_EXT_VS_NAME(x)        (x)->_vs_name

















#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VALUE_STRING_H__ */
