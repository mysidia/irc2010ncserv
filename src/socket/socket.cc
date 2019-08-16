/*
 *   pIrcd socket layer                                        src/socket/socket.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 Mysidia <***REMOVED***> <Mysidia>
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
#define error(x, y, z, a) //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <fcntl.h>
#include "pircd.h"
#include "sockstuff.h"
#include "sysdep.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#undef USE_SELECT
#define USE_POLL

namespace socketio {
  int highest_fd = 0, num_sockets = 0;
  void update_highfd();
}

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
	LIST_REMOVE(tmp, portent);
	if (tmp->fd == socketio::highest_fd)
	    socketio::update_highfd();
	tmp->fd = -1;
	free_listener(tmp);
	socketio::num_sockets--;
}

void start_listening(aListener *start)
{
        aListener *newlistener;
        struct sockaddr_in sai;

        if ( start->fd > -1 ) {
             log_error("start_listening: err start already open");
             return;
        }
        start->fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
	if (start->fd > socketio::highest_fd)
	    socketio::highest_fd = start->fd;
	socketio::num_sockets++;
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


namespace socketio {

  void listener_accept(aListener *lst) {
       Connection *tmp;
       struct sockaddr_in addr;
       unsigned int addrlen = sizeof(struct sockaddr_in);
       int nfd, i;

       if ((nfd = accept(lst->fd, (struct sockaddr *)&addr, &addrlen)) < 0) {
           DebugLog(8, "Unable to accept connection (port=%d, errno=%d).", lst->port, errno);
           return;
       }

       // Set it non-blocking
       set_sock_opts(nfd);

       // Create the new connection
       tmp = new_inlink();
       tmp->fd = nfd;
       tmp->ip = addr.sin_addr;
       tmp->client = new_client(NULL);
       tmp->con_type = CON_UNKNOWN;

       // Store them on the 'local' fd hash
       if (!local) {
           if (tmp->fd > socketio::highest_fd)
               socketio::highest_fd = tmp->fd;
           local = (Connection **)malloc(sizeof(Connection *) * (highest_fd + 2));
           for(i = 0; i < (highest_fd + 2); i++)
               local[i] = (Connection *)0;
       }
       else {
           int diff = (tmp->fd - socketio::highest_fd);

           if (tmp->fd > socketio::highest_fd)
               socketio::highest_fd = tmp->fd;   
           else diff = 0;

           local = (Connection **)realloc(local, sizeof(Connection *) * (highest_fd + 2));
           for(i = (highest_fd - diff + 1); i >= 0 && i < (highest_fd + 2); i++)
               local[i] = (Connection *)0;
       }
       local[tmp->fd] = tmp;
  }

  int pollio(time_t caltime = 0) {
       struct timeval tv;
       aListener *ltmp;
       Connection *link;
#if defined(USE_SELECT)
       fd_set readfd, writefd, xceptfd;
#elif defined(USE_POLL)
       struct pollfd *pollfds;
       int x = 0;
#endif
       int nfds = 0;

       if (!caltime)
           caltime = time(NULL);
       tv.tv_sec = time(NULL) - caltime;
       tv.tv_usec = 0;
#if defined(USE_SELECT)
       FD_ZERO(&readfd);
       FD_ZERO(&writefd);
       xceptfd = readfd;

       for (ltmp = LIST_FIRST(&listenlist) ;
            ltmp ; ltmp = LIST_NEXT(ltmp, portent))
            FD_SET(ltmp->fd, &readfd);
       for (link = LIST_FIRST(&inlinks) ;
            link ; link = LIST_NEXT(link, lp_con))
             if (link->fd != -1) {
                 FD_SET(link->fd, &readfd);
                 if (!(link->statu & STAT_WOUT))
                     FD_SET(link->fd, &writefd);
             }

       xceptfd = readfd;
       nfds = select(socketio::highest_fd+1, &readfd, &writefd, &xceptfd, &tv);
       if (nfds == -1) {
           perror("select");
           sleep(1);
       }
       else if (nfds < 1)
           return;
#elif defined(USE_POLL)
       pollfds = new pollfd [sizeof(struct pollfd) * (num_sockets + 1)];
       memset(pollfds, 0, sizeof(struct pollfd) * (num_sockets + 1));
       for (x = 0, ltmp = LIST_FIRST(&listenlist) ;
            ltmp ; ltmp = LIST_NEXT(ltmp, portent))
       {
            pollfds[x].fd = ltmp->fd;
            pollfds[x].events = POLLIN | POLLERR | POLLHUP;
            x++;
       }
       for (link = LIST_FIRST(&inlinks) ;
            link ; link = LIST_NEXT(link, lp_con))
       {
             if (link->fd == -1)
                 continue;
            pollfds[x].fd = link->fd;
            pollfds[x].events = POLLIN | POLLERR | POLLHUP;
            if (!(link->status & STAT_WOUT))
                pollfds[x].events |= POLLOUT;
            x++;
       }
       nfds = poll(pollfds, x+1, 1000 * tv.tv_sec);
#endif

#if defined(USE_SELECT)
       for(; nfds >= 0; nfds--) {
#elif defined(USE_POLL)
       for(; x >= 0; x--) {
#endif
           for (ltmp = LIST_FIRST(&listenlist) ;
                ltmp ; ltmp = LIST_NEXT(ltmp, portent))
           {
#if defined(USE_SELECT)
                if (FD_ISSET(ltmp->fd, read_set))
#elif defined(USE_POLL)
                if (pollfds[x].revents & POLLIN)
#endif
                    listener_accept(ltmp);
           }

           for (link = LIST_FIRST(&inlinks) ;
                link ; link = LIST_NEXT(link, lp_con))
           {
#if defined(USE_SELECT)
                if (FD_ISSET(link->fd, writefd))
#elif defined(USE_POLL)
                if (pollfds[x].revents & POLLOUT)
#endif
                    link->status |= STAT_WOUT;
#if defined(USE_SELECT)
                if (FD_ISSET(link->fd, readfd))
#elif defined(USE_POLL)
                if (pollfds[x].revents & POLLIN)
#endif
                    link->read();
#if defined(USE_SELECT)
                if (FD_ISSET(link->fd, xceptfd))
#elif defined(USE_POLL)
                if (pollfds[x].revents & POLLHUP)
#endif
                    link->status |= STAT_DEAD;
           }

       }

#if !defined(USE_SELECT) && defined(USE_POLL)
       delete pollfds;
#endif
       return nfds > 0 ? 1 : 0;
  }

  void update_highfd()
  {
          aListener *tmp;
          Connection *link;

	  socketio::highest_fd = 0;
          for (tmp = LIST_FIRST(&firstqlport) ;
                      tmp ; tmp = LIST_NEXT(tmp, portent))
 	       if (tmp->fd > socketio::highest_fd)
		   socketio::highest_fd = tmp->fd;
          for (link = LIST_FIRST(&inlinks) ;
                    link ; link = LIST_NEXT(link, lp_con))
	       if (link->fd > socketio::highest_fd)
		   socketio::highest_fd = link->fd;
  }
}

//////////////////////////////////////////////////////////////////////////
// Input routines
//////////////////////////////////////////////////////////////////////////

void msgbuf_put(MsgBuf *q, char *s1, char *s2 = NULL)
{
    MsgBufBuffer mbb;
    int len;

    if (!q || !s1)
        return;
    if (!s2)
        len = strlen(s1);
    else len = strlen(s1) + strlen(s2);
    mbb.buf = new char [len + 1];
    if (!mbb.buf) 
        out_of_memory();

    strcpy(mbb.buf, s1);
    if (s2)
        strcat(mbb.buf, s2);
    mbb.len = len;
    if (q->buffers)
        q->buffers = (MsgBufBuffer *)realloc(q->buffers, sizeof(MsgBufBuffer *) * (q->nbuffers+1));
    else
        q->buffers = (MsgBufBuffer *)malloc(sizeof(MsgBufBuffer *) * (q->nbuffers+1));
    q->buffers[q->nbuffers] = mbb;
    q->nbuffers++;
}

void Connection::read()
{
   static char recvbuffer[16384];
   struct sockaddr sa;
   socklen_t namelen;
   char *cp, *cp2, *s;
   int r, i;

   if (getpeername(fd, &sa, &namelen) < 0) {
       DebugLog(8, "getpeername(%d) failed: closing connection.", fd);       
       status |= STAT_DEAD;
       return;
   }

   while((r = recvfrom(fd, recvbuffer, sizeof(recvbuffer), 0, &sa, &namelen)) != -1) {
     cp = NULL;

     if (r == 0) {
         status |= STAT_DEAD;
         break;
     }

     do
     {
       s = cp ? cp + 1 : recvbuffer;

       for(cp = s; cp; cp++)
           if (*cp == '\r' || *cp == '\n')
               break;
       if (!cp) {
           if (!recvQ.io) {
               recvQ.io = new char [IRCBUF];

               for(cp2 = s, i = 0; cp2; cp2++) {
                   if (i >= (IRCLEN - 1))
                       break;
                   recvQ.io[i++] = *cp2;
               }
               if (cp2) {
                   recvQ.io[i++] = '\n';
                   recvQ.io[i++] = '\0';
               }
           }
           break;
       }
       else {
           if (recvQ.io) {
               char a = *cp;

               *cp = '\0';
               msgbuf_put(&recvQ, recvQ.io, s);
               *cp = a;

               delete recvQ.io;
           }
           else {
               char a = *cp;

               *cp = '\0';
               msgbuf_put(&recvQ, s);
               *cp = a;
           }
       }
     } while(1);
   }
}



