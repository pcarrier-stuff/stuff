/**
 * errno constants and strings
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Notes:
 * - Not thorough, please report missing E* constants I missed.
**/

/* Grab everything under OSX */
#define _DARWIN_C_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#define UNKNOWN "Unknown error"

void add_errconst(GHashTable * table, int nr, char *str)
{
    GList *list = g_hash_table_lookup(table, GINT_TO_POINTER(nr));
    list = g_list_append(list, str);
    g_hash_table_insert(table, GINT_TO_POINTER(nr), list);
}

GHashTable *build_errconsts()
{
    GHashTable *res = NULL;

    res = g_hash_table_new(g_direct_hash, g_direct_equal);

    if (res) {
#undef ERRNO_EXISTS
#define ERRNO_EXISTS(nr,str) add_errconst(res, nr, str);
#include "sys/errnos.h"

#ifdef __linux__
        /* Kernel-only errnos, from kernel.git/include/linux/errno.h */
        ERRNO_EXISTS(512, "ERESTARTSYS");
        ERRNO_EXISTS(513, "ERESTARTNOINTR");
        ERRNO_EXISTS(514, "ERESTARTNOHAND");
        ERRNO_EXISTS(515, "ENOIOCTLCMD");
        ERRNO_EXISTS(516, "ERESTART_RESTARTBLOCK");
        ERRNO_EXISTS(521, "EBADHANDLE");
        ERRNO_EXISTS(522, "ENOTSYNC");
        ERRNO_EXISTS(523, "EBADCOOKIE");
        ERRNO_EXISTS(524, "ENOTSUPP");
        ERRNO_EXISTS(525, "ETOOSMALL");
        ERRNO_EXISTS(526, "ESERVERFAULT");
        ERRNO_EXISTS(527, "EBADTYPE");
        ERRNO_EXISTS(528, "EJUKEBOX");
        ERRNO_EXISTS(529, "EIOCBQUEUED");
        ERRNO_EXISTS(530, "EIOCBRETRY");
#endif
    }

    return res;
}

int main()
{
    gint errnr, errmax = 1024;
    const char *errstr;
    GList *cur_errconst;
    gboolean has_strerror;
    GHashTable *errconsts = build_errconsts();

    for (errnr = 0; errnr <= errmax; errnr++) {
        errstr = strerror(errnr);

        has_strerror =
            strncmp(errstr, UNKNOWN, sizeof(UNKNOWN) - 1) ? TRUE : FALSE;
        cur_errconst =
            (GList *) g_hash_table_lookup(errconsts,
                                          GINT_TO_POINTER(errnr));

        if (!has_strerror && !cur_errconst)
            continue;

        printf("%i\t0x%x\t\"%s\"\n", errnr, errnr, errstr);
        while (cur_errconst) {
            printf("%i\t0x%x\t%s\n", errnr, errnr,
                   (char *) cur_errconst->data);
            cur_errconst = g_list_next(cur_errconst);
        }
        errmax = errnr + 1024;
    }

    fprintf(stderr, "Stopped looking at %i\n", errnr);
    return fclose(stdout);
}
