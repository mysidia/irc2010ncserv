/*
 *   pIrcd ports source file                            src/structs/hash.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 Mysidia <***REMOVED***> <Mysidia>
 *   Copyright (C) 1999 Garry Boyer <binary>
 *   see file 'AUTHORS' for a comprehensive list of pIrcd authors
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public license Version 2
 *   as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#define __list_cc__
#include "pircd.h"
#include "h.h"
__RCSID("$Id: list.cc,v 1.2 2001/03/18 20:47:49 mysidia Exp $");


void free_listener(aListener *listen)
{
      if (listen->addy)  free(listen->addy);
      listen->addy = NULL;
      free(listen);
}

idecl void use_client(Client *cptr)
{
     if ((cptr->inuse < 0))
           return;
     if (cptr->inuse >= 0)
           ++cptr->inuse;
}

void rel_client(Client *cptr)
{
     if ((cptr->inuse < 0))
           return;
     if (cptr->inuse > 0)
           --cptr->inuse;
}

void free_client(Client *cptr)
{
     if ((cptr->inuse < 0))
     {
              log_error("free_client: (debug) inuse = %d", cptr->inuse);
              return;
     }
     if ((cptr->flags & FLAGS_DELETED))
              log_error("free_client: client was already flagged deleted");
     if (cptr->inuse == 0)
          free(cptr);
     else if (cptr->inuse >= 0) cptr->flags |= FLAGS_DELETED;
}

