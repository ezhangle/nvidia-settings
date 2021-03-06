/*
 * Copyright (C) 2004 NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses>.
 */

#ifndef __MSG_H__
#define __MSG_H__

#include <stdarg.h>
#include <stdio.h>


/*
 * Define a printf format attribute macro.  This definition is based on the one
 * from Xfuncproto.h, available in the 'xproto' package at
 * http://xorg.freedesktop.org/releases/individual/proto/
 */

#if defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 203)
# define NV_ATTRIBUTE_PRINTF(x,y) __attribute__((__format__(__printf__,x,y)))
#else /* not gcc >= 2.3 */
# define NV_ATTRIBUTE_PRINTF(x,y)
#endif


/*
 * NV_VSNPRINTF(): macro that assigns buf using vsnprintf().  This is
 * correct for differing semantics of the vsnprintf() return value:
 *
 * -1 when the buffer is not long enough (glibc < 2.1)
 *
 *   or
 *
 * the length the string would have been if the buffer had been large
 * enough (glibc >= 2.1)
 *
 * This macro allocates memory for buf; the caller should free it when
 * done.
 */

#define NV_FMT_BUF_LEN 256

#define NV_VSNPRINTF(buf, fmt)                                  \
do {                                                            \
    if (!fmt) {                                                 \
        (buf) = NULL;                                           \
    } else {                                                    \
        va_list ap;                                             \
        int len, current_len = NV_FMT_BUF_LEN;                  \
                                                                \
        while (1) {                                             \
            (buf) = nvalloc(current_len);                       \
                                                                \
            va_start(ap, fmt);                                  \
            len = vsnprintf((buf), current_len, (fmt), ap);     \
            va_end(ap);                                         \
                                                                \
            if ((len > -1) && (len < current_len)) {            \
                break;                                          \
            } else if (len > -1) {                              \
                current_len = len + 1;                          \
            } else {                                            \
                current_len += NV_FMT_BUF_LEN;                  \
            }                                                   \
                                                                \
            nvfree(buf);                                        \
        }                                                       \
    }                                                           \
} while (0)


/*
 * verbosity, controls output of errors, warnings and other
 * information.
 */

typedef enum {
    NV_VERBOSITY_NONE = 0,                    /* no errors, warnings or info */
    NV_VERBOSITY_ERROR,                       /* errors only */
    NV_VERBOSITY_DEPRECATED,                  /* errors and deprecation messages */
    NV_VERBOSITY_WARNING,                     /* errors and all warnings */
    NV_VERBOSITY_ALL,                         /* errors, all warnings and other info */
    NV_VERBOSITY_DEFAULT = NV_VERBOSITY_ALL
} NvVerbosity;

NvVerbosity nv_get_verbosity(void);
void        nv_set_verbosity(NvVerbosity level);


/*
 * Formatted I/O functions
 */

void reset_current_terminal_width(unsigned short new_val);

void nv_error_msg(const char *fmt, ...)                NV_ATTRIBUTE_PRINTF(1, 2);
void nv_deprecated_msg(const char *fmt, ...)           NV_ATTRIBUTE_PRINTF(1, 2);
void nv_warning_msg(const char *fmt, ...)              NV_ATTRIBUTE_PRINTF(1, 2);
void nv_info_msg(const char *prefix,
                 const char *fmt, ...)                 NV_ATTRIBUTE_PRINTF(2, 3);
void nv_info_msg_to_file(FILE *stream,
                         const char *prefix,
                         const char *fmt, ...)         NV_ATTRIBUTE_PRINTF(3, 4);
void nv_msg(const char *prefix, const char *fmt, ...)  NV_ATTRIBUTE_PRINTF(2, 3);
void nv_msg_preserve_whitespace(const char *prefix,
                                const char *fmt, ...)  NV_ATTRIBUTE_PRINTF(2, 3);


/*
 * TextRows structure and helper functions
 */

typedef struct {
    char **t; /* the text rows */
    int n;    /* number of rows */
    int m;    /* maximum row length */
} TextRows;

TextRows *nv_format_text_rows(const char *prefix, const char *str, int width,
                              int word_boundary);
void nv_text_rows_append(TextRows *t, const char *msg);
void nv_concat_text_rows(TextRows *t0, TextRows *t1);
void nv_free_text_rows(TextRows *t);


#endif /* __MSG_H__ */
