#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef int gint;
typedef unsigned guint;
typedef char gchar;
typedef unsigned char guchar;
typedef float gfloat;
typedef double gdouble;

typedef uint8_t guint8;
typedef int8_t gint8;

typedef uint16_t guint16;
typedef int16_t gint16;

typedef uint32_t guint32;
typedef int32_t gint32;

typedef bool gboolean;

typedef uint64_t guint64;
typedef int64_t gint64;

typedef size_t gsize;



// maybe should not put this here
// typedef uint8_t* GByteArray;


#define FALSE false
#define TRUE true

#define G_GNUC_WARN_UNUSED_RESULT
#define WS_NORETURN

#define G_GNUC_PRINTF(...)
#define proto_report_dissector_bug(...)

#define G_LITTLE_ENDIAN (1234)
#define G_BIG_ENDIAN (4321)

// make an assumtion about wasm here
#define G_BYTE_ORDER G_LITTLE_ENDIAN
