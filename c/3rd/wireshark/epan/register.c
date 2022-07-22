/* register.c
 * Definitions for protocol registration
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "register-int.h"
#include "ws_attributes.h"

#include <glib.h>
#include "epan/dissectors/dissectors.h"
#include <stdio.h>

// const gulong dissector_reg_proto_count = {0};
// const gulong dissector_reg_handoff_count = {1};

static const char *cur_cb_name = NULL;
// We could use g_atomic_pointer_set/get instead of a mutex, but that's
// currently (early 2018) invisible to TSAN.
static GMutex cur_cb_name_mtx;
static GAsyncQueue *register_cb_done_q;

#define CB_WAIT_TIME (150 * 1000) // microseconds

static void set_cb_name(const char *proto) {
    // g_mutex_lock(&cur_cb_name_mtx);
    cur_cb_name = proto;
    // g_mutex_unlock(&cur_cb_name_mtx);
}

static void *
register_all_protocols_worker(void *arg _U_)
{
    for (gulong i = 0; i < dissector_reg_proto_count; i++) {
        printf("register_all_protocols_worker() %ld\n", i);
        set_cb_name(dissector_reg_proto[i].cb_name);
        dissector_reg_proto[i].cb_func();
    }

    // g_async_queue_push(register_cb_done_q, GINT_TO_POINTER(TRUE));
    return NULL;
}

void
register_all_protocols(register_cb cb, gpointer cb_data)
{
    printf("register_all_protocols()\n");
    const char *cb_name;
    register_cb_done_q = g_async_queue_new();
    gboolean called_back = FALSE;
    // GThread *rapw_thread;

    register_all_protocols_worker(NULL);

    // rapw_thread = g_thread_new("register_all_protocols_worker", &register_all_protocols_worker, NULL);
    // while (!g_async_queue_timeout_pop(register_cb_done_q, CB_WAIT_TIME)) {
        g_mutex_lock(&cur_cb_name_mtx);
        cb_name = cur_cb_name;
        // g_mutex_unlock(&cur_cb_name_mtx);
        if (cb && cb_name) {
            cb(RA_REGISTER, cb_name, cb_data);
            called_back = TRUE;
        }
    // }
    // g_thread_join(rapw_thread);
    if (cb && !called_back) {
        cb(RA_REGISTER, "finished", cb_data);
    }
}

void
register_all_protocols2(register_cb cb, gpointer cb_data)
{
    printf("register_all_protocols()\n");
    const char *cb_name;
    register_cb_done_q = g_async_queue_new();
    gboolean called_back = FALSE;
    GThread *rapw_thread;

    rapw_thread = g_thread_new("register_all_protocols_worker", &register_all_protocols_worker, NULL);
    while (!g_async_queue_timeout_pop(register_cb_done_q, CB_WAIT_TIME)) {
        g_mutex_lock(&cur_cb_name_mtx);
        cb_name = cur_cb_name;
        g_mutex_unlock(&cur_cb_name_mtx);
        if (cb && cb_name) {
            cb(RA_REGISTER, cb_name, cb_data);
            called_back = TRUE;
        }
    }
    g_thread_join(rapw_thread);
    if (cb && !called_back) {
        cb(RA_REGISTER, "finished", cb_data);
    }
}

static void *
register_all_protocol_handoffs_worker(void *arg _U_)
{
    for (gulong i = 0; i < dissector_reg_handoff_count; i++) {
        set_cb_name(dissector_reg_handoff[i].cb_name);
        dissector_reg_handoff[i].cb_func();
    }

    // g_async_queue_push(register_cb_done_q, GINT_TO_POINTER(TRUE));
    return NULL;
}

void
register_all_protocol_handoffs(register_cb cb, gpointer cb_data)
{
    cur_cb_name = NULL;
    const char *cb_name;
    gboolean called_back = FALSE;
    GThread *raphw_thread;

    register_all_protocol_handoffs_worker(NULL);

    // raphw_thread = g_thread_new("register_all_protocol_handoffs_worker", &register_all_protocol_handoffs_worker, NULL);
    // while (!g_async_queue_timeout_pop(register_cb_done_q, CB_WAIT_TIME)) {
        // g_mutex_lock(&cur_cb_name_mtx);
        cb_name = cur_cb_name;
        // g_mutex_unlock(&cur_cb_name_mtx);
        if (cb && cb_name) {
            cb(RA_HANDOFF, cb_name, cb_data);
            called_back = TRUE;
        }
    // }
    // g_thread_join(raphw_thread);
    if (cb && !called_back) {
        cb(RA_HANDOFF, "finished", cb_data);
    }
    // g_async_queue_unref(register_cb_done_q);
}

void
register_all_protocol_handoffs2(register_cb cb, gpointer cb_data)
{
    cur_cb_name = NULL;
    const char *cb_name;
    gboolean called_back = FALSE;
    GThread *raphw_thread;

    raphw_thread = g_thread_new("register_all_protocol_handoffs_worker", &register_all_protocol_handoffs_worker, NULL);
    while (!g_async_queue_timeout_pop(register_cb_done_q, CB_WAIT_TIME)) {
        g_mutex_lock(&cur_cb_name_mtx);
        cb_name = cur_cb_name;
        g_mutex_unlock(&cur_cb_name_mtx);
        if (cb && cb_name) {
            cb(RA_HANDOFF, cb_name, cb_data);
            called_back = TRUE;
        }
    }
    g_thread_join(raphw_thread);
    if (cb && !called_back) {
        cb(RA_HANDOFF, "finished", cb_data);
    }
    g_async_queue_unref(register_cb_done_q);
}

gulong register_count(void)
{
    return dissector_reg_proto_count + dissector_reg_handoff_count;
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
