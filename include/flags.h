/*
 *   Copyright (C) 1998-2001 Mysidia <mysidia at qmud dot org>
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

/*
** flags.h - defines flags
*/

#ifndef __flags_h_include__
#define	__flags_h_include__

#include <values.h>

#define	BIT01	0x00000001
#define	BIT02	0x00000002
#define	BIT03	0x00000004
#define	BIT04	0x00000008
#define	BIT05	0x00000010
#define	BIT06	0x00000020
#define	BIT07	0x00000040
#define	BIT08	0x00000080
#define	BIT09	0x00000100
#define	BIT10	0x00000200
#define	BIT11	0x00000400
#define	BIT12	0x00000800
#define	BIT13	0x00001000
#define	BIT14	0x00002000
#define	BIT15	0x00004000
#define	BIT16	0x00008000
#define	BIT17	0x00010000
#define	BIT18	0x00020000
#define	BIT19	0x00040000
#define	BIT20	0x00080000
#define	BIT21	0x00100000
#define	BIT22	0x00200000
#define	BIT23	0x00400000
#define	BIT24	0x00800000
#define	BIT25	0x01000000
#define	BIT26	0x02000000
#define	BIT27	0x04000000
#define	BIT28	0x08000000
#define	BIT29	0x10000000
#define	BIT30	0x20000000
#define	BIT31	0x40000000
#define	BIT32	0x80000000
#define BIT(x)	(1 << (x-1))

#define	M_NEEDPARAM_ADD	BIT01
#define	M_NEEDPARAM_DEL	BIT02
#define	M_NEEDPARAM	(BIT01|BIT02)
#define bitvector_t	basic_bitvector

/////////////////////////////////////////////////////////////////////////
// Bitvector class                                                     //
/////////////////////////////////////////////////////////////////////////

// Actually, for now just use a u_long
typedef unsigned long flag_t;

/////////////////////////////////////////////////////////////////////////

struct flag_list
	{
		flag_t flag;
		char modechar;
 		char needparam;
		char *name;          /* for purpose of flag functions */
		short xbit;	     /* extra flags regarding how the flag is set */
	};

#define FLFLAG_NOUNSET	BIT01	     /* flag cannot be unset by user */
#define FLFLAG_NOSET	BIT02	     /* flag cannot be set by user */
#define FLFLAG_INTERNAL	BIT03	     /* flag is used elsewhere not by flag routines */

#define FLAGS_DELETED	BIT01	     /* client deleted, free when inuse drops to 0 */

#define	UMODE_OPER	BIT01
#define	UMODE_LOCOP	BIT02
#define	UMODE_WALLOPS	BIT03
#define	UMODE_INVISIBLE	BIT04
#define	UMODE_GLOBOPS	BIT05
#define	UMODE_HELPOP	BIT06
#define	UMODE_DISGUISE	BIT07

#define FLAGS_INVALID	BIT01

#ifndef DECL_FLISTS
extern struct flag_list umode_list[];
#else
struct flag_list umode_list[] =
	{
		{	UMODE_OPER,		'o', 0,		"oper" },
		{	UMODE_LOCOP,		'O', 0,		"locop" },
		{	UMODE_WALLOPS,		'w', 0,		"wallops" },
		{	UMODE_INVISIBLE,	'i', 0,		"invis" },
		{	UMODE_GLOBOPS,		'g', 0,		"globops" },
		{	UMODE_HELPOP,		'h', 0,		"helpop" },
		{	UMODE_DISGUISE,		'd', 0,		"disguise" },
		{	0,			 0 , 0,		(char *)NULL }
	};
#endif

#define	CMODE_NOPRIVMSGS	BIT01
#define	CMODE_TOPICLIMIT	BIT02
#define	CMODE_SECRET		BIT03
#define	CMODE_PRIVATE		BIT04
#define	CMODE_INVITEONLY	BIT05
#define	CMODE_MODERATED		BIT06
#define	CMODE_LIMIT		BIT07
#define	CMODE_KEY		BIT08
#define	CMODE_BAN		BIT09
#define	CMODE_OP		BIT10
#define	CMODE_VOICE		BIT11

#ifndef DECL_FLISTS
extern struct flag_list cmode_list[];
#else
struct flag_list cmode_list[] =
	{
		{	CMODE_NOPRIVMSGS,	'n', 0,			"noext"},
		{	CMODE_TOPICLIMIT,	't', 0,			"topiclock" },
		{	CMODE_SECRET,		's', 0,			"secret" },
		{	CMODE_PRIVATE,		'p', 0,			"private" },
		{	CMODE_INVITEONLY,	'i', 0,			"inviteonly" },
		{	CMODE_MODERATED,	'm', 0,			"moderated" },
		{	CMODE_LIMIT,		'l', M_NEEDPARAM_ADD,	"userlimit" },
		{	CMODE_KEY,		'k', M_NEEDPARAM,	"key" },
		{	CMODE_BAN,		'b', M_NEEDPARAM,	"ban" },
		{	CMODE_OP,		'o', M_NEEDPARAM,	"op" },
		{	CMODE_VOICE,		'v', M_NEEDPARAM,	"voice" },
		{	0,			 0 , 0,			(char *)0 }
	};
#endif
#define	CHFL_OP		BIT01
#define	CHFL_VOICE	BIT02
#define	CHFL_BANNED	BIT04 /* no need for constant lookups */
#define	CHFL_NOTBANNED	BIT05

#endif /* _FLAGS_INCLUDE_ */
