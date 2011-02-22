/**
 * memcpy2memmove: if your app relies on memcpy for overlapping memory zones,
 * fix it. If impossible, we can help.
 *
 * Copyright (c) 2010, Red Hat Inc.
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all copies.
 *
 * Maintainer: Pierre Carrier <prc@redhat.com>
 *
 * Compile with:
 * gcc -g -shared -fPIC -ldl -o /usr/local/lib/memcpy2memmove.so memcpy2memmove.c
 * To also log your app's "misbehaviours", add -DDEBUG
 * To log without correcting, add -DDEBUG -DDONTFIX
 *
 * Use in a shell with:
 * LD_PRELOAD=/usr/local/lib/memcpy2memmove.so foo bar
 *
 * To log misbehaviours, one can 
 *
 * Warning: This will impact performance!
**/

#define _GNU_SOURCE 1
#include <string.h>
#include <stdio.h>

void *memcpy(void *dest, const void *src, size_t n)
{
#ifdef DEBUG
    if ((src - dest < n) || (dest - src < n))
        fprintf(stderr,
                "[memcpy2memmove: overlap! 0x%x->0x%x (%i bytes)]\n", dest,
                src, n);
#endif
#ifdef DONTFIX
    return (memcpy(dest, src, n));
#else
    return (memmove(dest, src, n));
#endif
}
