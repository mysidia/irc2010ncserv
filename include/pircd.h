#ifndef __pircd_h_include__
#define __pircd_h_include__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <time.h>

#include "struct.h"
#include "hash.h"
#include "sysdep.h"
#include "defs.h"
#include "types.h"
#include "macro.h"
#include "flags.h"
#include "queue.h"
#include "h.h"

#ifdef __cplusplus
using namespace pircd_alloc;
#endif

#if (defined(__GNUC__) || defined(__ANSI__) || defined(__STDC__)) && !defined(HAS_REGISTERS)
#define	Reg1		register
#define	Reg2		register
#define	Reg3		register
#define	Reg4		register
#define	Reg5		register
#define	Reg6		register
#else
#warning not attempting to use registers
#define register #warning reg
#define Reg1
#define Reg2
#define Reg3
#define Reg4
#define Reg5
#define Reg6
#endif

#define mycmp(ss1, ss2) pircd_mycmp(ss1, ss2)

#endif /* ! __pircd_h_include__ */
