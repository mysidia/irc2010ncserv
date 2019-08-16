/*
 *   Copyright (C) 1998-2001 Mysidia <***REMOVED***>
 *
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

#define IRCD_CONF	"ircd.cf"
#define	MAXNEST		4		/* maximum nested blocks in ircd.cf */
#define	OFILEBUF	1024
#define	OSOCKBUF	8192
#define MAXHOST		63		/* maximum length of a host (dont change)*/
#define CONFLINE	255		/* maximum length of a conf line*/
#undef	RFC1459				/* when defined pIrcd will be rfc1459 compliant */
#undef	DF467				/* when defined pIrcd will try to get along with df467 servers on the network */

 /*
    these should either be un-defined or point to 
    alternate dns resolver config files
 */
/* #define IRCD_HOSTCONF	"/etc/host.conf" */
#define IRCD_RESCONF		"/etc/resolv.conf"

#define MAXCONNECTIONS		150
#define LISTEN_SIZE		5
