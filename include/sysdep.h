#ifndef __sysdep_include__
/*
 *      sort of like ircd's sys.h
 */
#include "../src/port/port.h"

#ifdef ISC202
#include <net/errno.h>
#else
# ifndef _WIN32
#include <sys/errno.h>
# else
#include <errno.h>
# endif
#endif
#endif

#ifdef SOL20
#define OPT_TYPE char   /* opt type for get/setsockopt */
#else
#define OPT_TYPE void
#endif

#include <stdio.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/param.h>
#else
#include <stdarg.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#define stricmp		strcasecmp
#define strcmp		strcmp
#define   index   strchr
#define   rindex  strrchr
#define bcopy(x,y,z)    memcpy(y,x,z)
#define bcmp(x,y,z)     memcmp(x,y,z)
#define bzero(p,s)      memset(p,0,s)
#include <time.h>
#include <sys/time.h>
#ifdef AIX
#include <sys/time.h>
#endif

#if !defined(DEBUGMODE) || defined(CLIENT_COMPILE)
//#define free(x)         dfree(x)

#ifdef NEXT
#define VOIDSIG int     /* whether signal() returns int of void */
#else
#define VOIDSIG void    /* whether signal() returns int of void */
#endif

#ifdef SOL20
#define OPT_TYPE char   /* opt type for get/setsockopt */
#else
#define OPT_TYPE void
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__bsdi__)
#define dn_skipname  __dn_skipname
#endif

#ifndef _WIN32
extern  VOIDSIG dummy();
#endif

#ifdef  DYNIXPTX
#define NO_U_TYPES
typedef unsigned short n_short;         /* short as received from the net */
typedef unsigned long   n_long;         /* long as received from the net */
typedef unsigned long   n_time;         /* ms since 00:00 GMT, byte rev */
#define _NETINET_IN_SYSTM_INCLUDED
#endif

#ifdef  NO_U_TYPES
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned long   u_long;
typedef unsigned int    u_int;
#endif

#ifdef USE_DES
#include <des.h>
#endif

#ifndef _WIN32
#define closesocket(x) close(x)
#define closefile(x) close(x)
#endif

#ifndef __GNUC__
#define idecl
#define iextern
#else
#define idecl inline
#define iextern extern inline
#endif

#endif /* __sysdep_include__ */

