/*
 *   pIrcd utilities                                           src/misc/alloc.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 ***REMOVED*** <Mysidia>
 *   see file 'AUTHORS' for a comprehensive list of pIrcd authors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the file 'LICENSE' included with this package
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "pircd.h"
#include "h.h"
__RCSID("$Id: alloc.cc,v 1.1 2001/03/12 03:12:05 mysidia Exp $");

void mm_boot() {}

namespace pircd_alloc
{
unsigned long ramKB = 0;
long ramB  = 0;

#undef malloc
#undef free

#ifdef TRACK_RAM
#warning TRACK_RAM wont work
#undef TRACK_RAM
#endif

void ofree(void *stuff)
{
#ifdef DEBUGLOG_RAM
       DebugLog(10, "freeing memory at area %X", stuff);
#endif
       (void) ::free(stuff);
}

void *oalloc(size_t amount)
{
    void *mem;
#ifdef DEBUGLOG_RAM
    static int v = 0;
#endif

    mem = ::malloc(amount); 
    if (!mem)
            out_of_memory();
#ifdef DEBUGLOG_RAM
    if (!v)
    {
       v=1;DebugLog(10, "allocating %d bytes starting at %X", amount, mem);v=0;
    }
#endif
#ifdef TRACK_RAM
{
	ramB += amount;
	if ( ramB > 1024) 
           while (ramB >= 1024)
           {
                 ramB -= 1024;
                 ramKB += 1;
           }
}
#endif
    return mem;
}

}
