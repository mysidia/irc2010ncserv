/*
 *   pIrcd socket layer                                        src/socket/socket.c
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
#define error(x, y, z, a) //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pircd.h"
#include "sockstuff.h"
#include "sysdep.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* aListener queued_listen; */
LIST_HEAD(,sListener)       firstqlport;
LIST_HEAD(,sListener)       listenlist;

void want_to_listen(char *addy, int port)
{
        aListener *newlistener;
        newlistener = (aListener *)calloc(1, sizeof(aListener));
        newlistener->addy = strdup(addy);
        newlistener->port = port;
        newlistener->fd = -1;
        LIST_ENTRY_INIT(newlistener, portent);
        LIST_INSERT_HEAD(&firstqlport, newlistener, portent);
}

void lqueue_delete(char *addy, int port)
{
        aListener *tmp;
        for (tmp = LIST_FIRST(&firstqlport) ;
                    tmp ; tmp = LIST_NEXT(tmp, portent))
         if (!mycmp(tmp->addy, addy) && (tmp->port == port))
          {
              LIST_REMOVE(tmp, portent);
              free_listener(tmp);
          }
}

void update_listeners()
{
        aListener *tmp, *tmp2;
        int t = 0;
        for (tmp = LIST_FIRST(&firstqlport) ;
                    tmp ; tmp = LIST_NEXT(tmp, portent))
          {
              t = 0;
              for (tmp2 = LIST_FIRST(&listenlist) ;
                         tmp2 ; tmp2 = LIST_NEXT(tmp2, portent))
              {
                     if (!mycmp(tmp->addy, tmp2->addy) && (tmp2->port == tmp->port))
                          {t++; break;}
              }
              if (!t)
                      start_listening(tmp);
          }


        for (tmp = LIST_FIRST(&listenlist) ;
                    tmp ; tmp = LIST_NEXT(tmp, portent))
          {
              t = 0;
              for (tmp2 = LIST_FIRST(&firstqlport) ;
                         tmp2 ; tmp2 = LIST_NEXT(tmp2, portent))
              {
                     if (!mycmp(tmp->addy, tmp2->addy) && (tmp2->port == tmp->port))
                          {t++; break;}
              }
              if (!t)
                      stop_listening(tmp);
          }

}

void stop_listening(aListener *tmp)
{
        if (tmp->fd > -1)
        {
		close(tmp->fd);
		DebugLog(8, "Listening on port %d stopped.", tmp->port);
	}
	tmp->fd = -1;
	LIST_REMOVE(tmp, portent);
	free_listener(tmp);
}

void start_listening(aListener *start)
{
        aListener *newlistener;
        struct sockaddr_in sai;

        if ( start->fd > -1 ) {
             log_error("start_listening: err start already open");
             return;
        }
        start->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if ((start->fd < 0)) {
            log_error("socket: %s", strerror(errno));
            return;
        }
        sai.sin_addr.s_addr = inet_addr((start->addy));
        sai.sin_family = AF_INET;
        sai.sin_port = htons( start->port );
        noblock( start->fd );
        set_sock_opts( start->fd );
        if ( bind( start->fd, (struct sockaddr *)&sai, sizeof(struct sockaddr) ) < 0 )
        {
            log_error("bind: %s", strerror(errno));
            return;
        }
        if ( listen( start->fd, LISTEN_SIZE ) < 0 )
        {
            log_error("listen: %s", strerror(errno));
            return;
        }
        newlistener = (aListener *)calloc(1, sizeof(aListener));
        memcpy( newlistener, start, sizeof(aListener));
        newlistener->addy = strdup(start->addy);
        LIST_ENTRY_INIT(newlistener, portent);
        LIST_INSERT_HEAD(&listenlist, newlistener, portent);
	DebugLog(8, "Now Listening on port %d.", newlistener->port);
}


void   noblock(int fd)
{
        int     nblockfl = 0;


#ifdef  NBLOCK_BSD
        nblockfl |= O_NDELAY;
#elif  NBLOCK_POSIX
        nblockfl |= O_NONBLOCK;
#elif  !defined(FIONBIO)
#warning May have trouble making non-blocking sockets 
#endif

#ifdef  NBLOCK_SYSV
     {
        int optval = 1;
        if (ioctl (fd, FIONBIO, &res) < 0)
                error(1, "(e=%d) ioctl(FIONBIO): %s", errno, strerror(errno));

     }
#else
# if !defined(_WIN32)
     {
        int optval = 0;
        if ((optval = fcntl(fd, F_GETFL, 0)) == -1)
                error(1, "(e=%d) fcntl(F_GETFL): %s", errno, strerror(errno));

        else if (fcntl(fd, F_SETFL, optval | nblockfl) == -1)
                error(1, "(e=%d) fcntl(F_SETFL): %s", errno, strerror(errno));
     }
# else
        nblockfl=1;
        if (ioctlsocket(fd, FIONBIO, &nblockfl) < 0)
                   error(1, "ioctlsocket(%d): %s", errno, strerror(errno));
# endif
#endif
        return;
}


void    set_sock_opts(int fd)
{
        int     opt = 0;

#ifdef SO_REUSEADDR
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (OPT_TYPE *)&opt, sizeof((++opt))) < 0)
                   error(1, "(e=%d) setsockopt(SO_REUSEADDR): %s", errno, strerror(errno));
#endif
#if  defined(SO_DEBUG) && defined(DEBUGMODE) && 0
#if !defined(SOL20) || defined(USE_SYSLOG)
        opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_DEBUG, (OPT_TYPE *)&opt, sizeof(opt)) < 0)
                   error(1, "(e=%d) setsockopt(SO_DEBUG): %s", errno, strerror(errno));
        opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_DEBUG, (OPT_TYPE *)&opt, sizeof(opt)) < 0)
                   error(1, "(e=%d) setsockopt(SO_DEBUG): %s", errno, strerror(errno));
#endif /* SOL20 */
#endif

#if defined(SO_USELOOPBACK) && !defined(_WIN32)
        opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_USELOOPBACK, (OPT_TYPE *)&opt, sizeof(opt)) < 0)
                error(1, "(e=%d) setsockopt(SO_USELOOPBACK): %s", errno, strerror(errno));
#endif
#ifdef  SO_RCVBUF
        opt = 8192;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (OPT_TYPE *)&opt, sizeof(opt)) < 0)
                error(1, "(e=%d) setsockopt(SO_RCVBUF): %s", errno, strerror(errno));
#endif
#ifdef  SO_SNDBUF
# ifdef _SEQUENT_
        opt = 8192;
# else
        opt = 8192;
# endif
        if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (OPT_TYPE *)&opt, sizeof(opt)) < 0)
                error(1, "(e=%d) setsockopt(SO_SNDBUF): %s", errno, strerror(errno));
#endif

#if defined(IP_OPTIONS) && defined(IPPROTO_IP) && !defined(_WIN32)
        {
        char    *s = readbuf, *t = readbuf + sizeof(readbuf) / 2;

        opt = sizeof(readbuf) / 8;
        if (getsockopt(fd, IPPROTO_IP, IP_OPTIONS, (OPT_TYPE *)t, &opt) < 0)
                error(1, "(e=%d) getsockopt(IP_OPTIONS): %s", errno, strerror(errno));
        else if (opt > 0 && opt != sizeof(readbuf) / 8)
            {
                for (*readbuf = '\0'; opt > 0; opt--, s+= 3)
                        (void)sprintf(s, "%02.2x:", *t++);
                *s = '\0';
               /* sendto_ops("Connection %s using IP opts: (%s)",
                           get_client_name(cptr, TRUE), readbuf);*/
            }
        if (setsockopt(fd, IPPROTO_IP, IP_OPTIONS, (OPT_TYPE *)NULL, 0) < 0)
                error(1, "(e=%d) setsockopt(IP_OPTIONS): %s", errno, strerror(errno));
        }
#endif
}


