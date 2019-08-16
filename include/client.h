/*
 * Client Header file 
 *
 * Copyright C 1998-2001  Mysidia <***REMOVED***>       
 *
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

#ifndef __client_h__
#define __client_h__

#include "queue.h"
#define IRCLEN 1024
#define IRCBUF IRCLEN+2

#ifndef __struct_cc__
extern LIST_HEAD(,Connection)       inlinks;
#endif

typedef enum {
   CON_UNDEFINED, CON_UNKNOWN, CON_USER, CON_SERVER
} connection_type;

typedef enum {
	STAT_WOUT = (1 << 0),
	STAT_DEAD = (1 << 1)
} connection_status_cl;

class MsgBufBuffer {
     public:
     int len;
     char *buf;
     MsgBufBuffer() : len(0), buf(NULL) { };
     ~MsgBufBuffer() {;}
};

class MsgBuf {
     public:
     char *io;
     MsgBufBuffer *buffers;
     int nbuffers;
     MsgBuf() :
        io(NULL), buffers(NULL), nbuffers(0)
     {
     }
     ~MsgBuf() { 
         if (io) 
             delete io; 
     }
};

class Connection				
{						
	public:					
	char ident[USERLEN+1];			
	connection_type con_type;			
	flag_t status;
	time_t last_cmd, signon, ping_sent;	
	struct in_addr ip;
	Client *client;
	MsgBuf sendQ, recvQ;

	int fd;				
	LIST_ENTRY(Connection)   lp_con;

	Connection() :
          con_type(CON_UNDEFINED), last_cmd(0), signon(0), ping_sent(0)
        {
          ident[0] = '\0';
          ip.s_addr = 0;
	}
	void read();
};

class Client
{
	public:					
	char name[NICKLEN+1];
	char host[NICKLEN+1];
	char info[REALLEN+1];
	User *user;
	Server *server;
	unsigned char hops;			
	int inuse;				
	flag_t flags;

        struct in_addr ip;
	Client *prev, *next, *from;
	Client *hnext, *hprev;
};


class User
{
	public:					
	Client *cptr;
	char username[USERLEN+1];
	char *away;
	ChanMember *channel;
	Server *server;
	flag_t umode;

	User *next, *prev;
};

class Server
{
	public:					
	Client *cptr;
	Server *next, *prev;
        Server *uplink, *downlink, *leftlink, *rightlink;
};
#endif
