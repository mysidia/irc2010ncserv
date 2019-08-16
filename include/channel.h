/*
 *   Copyright (C) 1997-2001 Mysidia <***REMOVED***>
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

#ifndef __channel_h__
#define __channel_h__

class Ban
{
	public:
	Ban *next;
	char *banmask;
	char *nick;
	time_t time;
};

class ChanMode
{
	public:
	unsigned long mode, limit;
	char key[KEYLEN+1];
};

class ChanMember
{
	public:
	ChanMember *nextu, *nextc;
	Channel *chptr;
	Client *cptr;
	char flags;
};

class Channel
{
	public:
	Channel *prev, *next, *hnext;
	ChanMode mode;
	ChanMember *members;
	int occupants;
	Ban *banlist;
	char chname[CHANLEN+1];
};

#endif

