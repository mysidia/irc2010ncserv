/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

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
