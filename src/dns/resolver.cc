/*
 *   pIrcd resolver subsystem source file                      src/main.c
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

#include "pircd.h"
#include "inet.h"

RCSID("$ID$: Copyright C 1998 Mysidia -- All rights reserved");
int dns_request[2];      /* for posting and receiving dns requests */
int dns_response[2];      /* for getting and sending dns responses */
int dnsonline = 0;	/* dns server is running */
int dns_start = 0;
static pid_t pDNS = 0;
static pid_t pMyPid = 0;

/*
 * ircd<-> resolver dialogue
 *
 *         ircd->resolver:    1 <host>
 *  (or)   ircd->resolver:    2                                     [SHUTDOWN]
 *  (or)   ircd->resolver:    3                                     [RECACHE]
 *
 *         resolver->ircd:    1 <host> <ip>
 *  (or)   resolver->ircd:    2 <host>                              [nonexistent data/host]
 *  (or)   resolver->ircd:    3 <host>                              [timeout]
 *  (or)   resolver->ircd:    4 <host>                              [non-recoverable errors]
 *  (or)   resolver->ircd:    5 <host>                              [NETDB ERROR]
 *  (or)   resolver->ircd:    6                                     [RESOLVER ERROR]
 *
 *
 */

static void dns_SIGHANDLER(int signo)
{
      fdprintf( dns_response[1], "6 GLOBAL\n");
      exit(0);
}

static void reply_dns(char *host)
{
      struct hostent *hp;
      extern int h_errno;

      errno = 0;
      h_errno = 0;
/*      hp2 = gethostbyname(host);*/
      if ( inet_addr( host ) != INADDR_NONE  )
      {
           hp = gethostbyaddr( iptoc( host ), sizeof(unsigned long) , AF_INET );
      }
      else
           hp = gethostbyname( host );
      if (hp && strlen( hp->h_name) > MAXHOST)
               hp = NULL;

      errno = 0;
      h_errno = 0;
      switch (h_errno)
      {
          case NETDB_INTERNAL:  
                 fdprintf( dns_response[1], "5 %s\n", host ); return;
          case HOST_NOT_FOUND: 
                 fdprintf( dns_response[1], "2 %s\n", host ); return;
          case NO_RECOVERY: 
                 fdprintf( dns_response[1], "4 %s\n", host ); return;
          case NO_DATA: 
                 fdprintf( dns_response[1], "3 %s\n", host ); return;
      }
      if (!hp)
       {
                 fdprintf( dns_response[1], "2 %s\n", host ); return;
       }
      if (hp)
       {
                 fdprintf( dns_response[1], "1 %s %s\n", host, hp && hp->h_name ? hp->h_name: "0");
/*                 fdprintf( dns_response[1], "1 %s %s\n", hp && hp->h_name ? hp->h_name: "0",
                           hp->h_addr_list && hp->h_addr_list[0] ? ctoip(hp->h_addr_list[0]) :"");*/
                 return;
       }
}

static void dns_daemon()
{
    FILE *fff = NULL;
    fd_set r;
    char buf[255] = "";

    signal(SIGINT, dns_SIGHANDLER);
    signal(SIGTERM, dns_SIGHANDLER);
    signal(SIGQUIT, dns_SIGHANDLER);
    signal(SIGSEGV, dns_SIGHANDLER);

     /* put the loop here*/
      while(1)
        {
             FD_ZERO(&r);
             FD_SET(dns_request[0], &r);
             if ( select(255, &r, NULL, NULL, NULL) < 0 )
                { usleep(3000);       continue; }
             if ( FD_ISSET( dns_request[0], &r ) )
               {
                    int request_code, no;
                    char request_string[256] = "";

                     if (!fff && !(fff = fdopen(dns_request[0], "r") ))         continue;
                     while ( fgets(buf, 255, fff) )
                     {
                         if (!*buf || *buf=='\n' || *buf == '\r') continue;
                         buf[strlen(buf)-1] = 0;
                         printf("%s\n", buf);
                     if ( ( no = sscanf(buf, "%d %s", &request_code, request_string) ) < 1)
                           log_error("invalid dns request '%s' from pIrcd", buf);
                     else if ( no == 1 )
                     {
                        if (request_code == 2)
                        {
                           logit("pIrcd dns process shutting down as instructed by pipe");
                           exit(0);
                        }
                        else if (request_code == 3) ;
                        else
                        {
                            logit("pIrcd made an invalid request of the dns process '%s'", buf);
                        }
                     }
                     else if ( no > 1 )
                     {
                         if (request_code == 1)
                         {
                            /* this we can identify with ! */
                            reply_dns(request_string);
                         }
                     }
                  }
               }
        }
    exit(0);
}

int boot_dns(int argc, char *argv[], char *envp[])
{
      logit("Booting up dns...\n");
      dns_request[0] = 0;
      dns_request[1] = 0;
      dns_response[0] = 0;
      dns_response[1] = 0;
      dns_start = 1;

      if ( pipe(dns_request) < 0 )
      {
             log_error("pipe failed: %s", strerror(errno));
             return -1;
      }
      if (pipe(dns_response) < 0)
      {
             log_error("pipe failed: %s", strerror(errno));
             return -1;
      }

      /* the child process will need 2 file descriptors to talk with us */
      fcntl(dns_request[0], F_SETFD, 0 );
      fcntl(dns_response[1], F_SETFD, 0 );

      pMyPid = getpid();
      pDNS = fork();
      if ( pDNS < 0)
      {
             czerosocket(dns_request[0]);
             czerosocket(dns_request[1]);
             czerosocket(dns_response[0]);
             czerosocket(dns_response[1]);
             return -1;
      }

      if ( pDNS )
      { 
          /*   these two descriptors now belong to our child */
           czerosocket(dns_request[0]);
           czerosocket(dns_response[1]);
           fdprintf( dns_request[1], "1 localhost\n");
           fdprintf( dns_request[1], "1 127.0.0.1\n");
           fdprintf( dns_request[1], "1 rs.internic.net\n");
           return 0;
      }

      if ( !pDNS )
      {
        /* we're the child; let's not mess with our parent's fds */
           fdprintf(dns_response[1], "7 0 0\n");
           czerosocket(dns_request[1]);
           czerosocket(dns_response[0]);
           /*
            * Whoh! one second, we need to be
            * a trustworthy resolver. 
            */
           unsetenv( "HOSTALIASES" );
           unsetenv( "LOCALDOMAIN" );
           initsetproctitle(argc, argv, envp);
           setproctitle("dns");
           dns_daemon();
      }
      return -1;
}

int dnsline(char *buf)
{
   int answer_code = 0, res = 0;
   static char answer_buf[8192] = "", host_buf[8192];
#ifdef DNS_DEBUG2
   printf("from resolver: %s", buf);
#endif

   res = sscanf(buf, "%d %s %s", &answer_code, host_buf, answer_buf);
   if (answer_code == 7)
    {
         ++dnsonline;
         return 1;
    }

   if ( res < 2 ) {
        if	(answer_code < 6)  return -1;
    else if ( res < 3 )
    {
        if	(answer_code == 1)  return -1;
        else if	(answer_code == 2)  return -1;
        else if	(answer_code == 3)  return -1;
        else if	(answer_code == 4)  return -1;
        else if	(answer_code == 5)  return -1;
    }}

    if ( answer_code == 6 )
    {
       --dnsonline;
       dns_start = 0;
       czerosocket(dns_request[1]);
       czerosocket(dns_response[0]);
    }

    if (answer_code == 1)
    {
         logit("%s->%s", host_buf, answer_buf);
    }
    return 0;
}

int dnsloop(int timed)
{
    struct timeval tv;
    fd_set r;
    int (* parsefunc)(char *);

    FD_ZERO(&r);
    FD_SET(dns_response[0], &r);
    bzero(&tv, sizeof(struct timeval));

    if (!dnsonline && !dns_start) return 0;

    logit( "DNS update..." );
    select(dns_response[0]+2, &r, NULL, NULL, &tv);
    if ( FD_ISSET( dns_response[0] , &r ) )
    {
         char respbuf[8192] = "";

         parsefunc = dnsline;
        noblock(dns_response[0]);
        while ( read( dns_response[0], respbuf, sizeof(respbuf) - 1 ) > 0 )
          line_splitter(respbuf, parsefunc);
    }
return 0;
}

int dnsdie(int signo = 0)
{
      logit("shutting down dns...");
      fdprintf( dns_request[1], "2\n");
      usleep (10000);

      kill( pDNS, 15 );
      kill( pDNS, 10 );
      kill( pDNS, 9  );
      return 0;
}

