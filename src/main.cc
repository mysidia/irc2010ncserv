/*
 *   pIrcd startup source file                                 src/main.c
 *   Copyright C 1998  Mysidia <mysidia at qmud dot org>  (mysidia @ irc.dal.net)
 *   Mysidia on QMUD   mysidia at qmud.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the file 'LICENSE' included with this package
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#define __main_cc__
#include "pircd.h"
#include "hash.h"
#include <signal.h>

time_t		NOW		= 0;
time_t		nextmaint	= 0;

extern		time_t		imaint(int);
extern          void initsetproctitle(int, char **, char **);

static int bootup_server()
{
         return 0;
}

static void s_die(int x)
{
    dnsdie(0);
    logit("Shutting down...");
    exit(0);
}

static void s_panic(int x)
{
    dnsdie(0);
    logit("PANIC! death");
    abort();
}

int main(int argc, char *argv[], char *envp[])
{
     mm_boot();
     signal(SIGINT, s_die);
     signal(SIGTERM, s_die);
     signal(SIGSEGV, s_panic);
     signal(SIGQUIT, s_panic);

     logit("Initializing hash tables...");
     init_hash();
     initsetproctitle(argc, argv, envp);
     setproctitle("server");

     if (read_config(IRCD_CONF) < 0)
           exit(0);
     if (boot_dns(argc, argv, envp) < 0 )
     {
         fprintf(stderr, "Fatal! Unable to bootup DNS subsystem.\n");
         return -1;
     }

     if (bootup_server() < 0)
           exit(0);
      init_tabs();
      update_listeners();

     while(1)
     { 
       NOW = time(NULL);
       if ((nextmaint<=NOW))
            nextmaint = imaint(1);
       dnsloop(1);
       if ( socketio::pollio(nextmaint) ) {
//          msgbuf::process_input();
       }
       sleep(2);
     }
     exit(0);
}

time_t imaint(int timed)
{
   return (time(NULL)+10);
}

