/*
 *   pIrcd ports source file                            src/structs/struct.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 ***REMOVED*** <Mysidia>
 *   Copyright (C) 1999 Garry Boyer <binary>
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
/*
** src/struct.c
**
**  Handles client/user/server/channel/member lists
*/

#include "pircd.h"

/* Constant list pointers to be able to keep list */

Client *firstClient, *lastClient;
User *firstUser, *lastUser;
Server *firstServer, *lastServer;
Chan *firstChan, *lastChan;
Connection *local[MAXCONNECTIONS];
Client me;
LIST_HEAD(,Connection)       inlinks;


/* Initialize the lists */

void	init_lists()
{
	firstClient = lastClient = &me;
	firstChan = lastChan = (Chan *)NULL;
	LIST_INIT(&inlinks);

	init_hash();
}


/* Client structures */

Client *new_client(char *name)
{
	Client *cptr;
	
	cptr = (Client *)malloc(sizeof(Client));
	if (!cptr)
		out_of_memory();
	bzero(cptr, sizeof(Client));
	cptr->prev = lastClient;
        if (lastClient)
 	    lastClient->next = cptr;
        else
            firstClient = lastClient = cptr;

	cptr->next = (Client *)NULL;
	lastClient = cptr;
        if (name)
 	    strncpyzt(cptr->name, name, NICKLEN);
	else *cptr->name = '\0';
	cptr->user = (User *)NULL;
	cptr->server = (Server *)NULL;
	cptr->hnext = cptr->from = (Client *)NULL;
	if (cptr->server)
	    cptr->hops = cptr->server->cptr->hops + 1;
	add_client_hash_entry(cptr);
	return cptr;
}

void del_client(Client *cptr)
{
	if (cptr->prev)
		cptr->prev->next = cptr->next;
	else
		firstClient = cptr->next;
	if (cptr->next)
		cptr->next->prev = cptr->prev;
	else
		lastClient = cptr->prev;
	del_client_hash_entry(cptr);
	free_client(cptr);
	return;
}

/* User structures */

User *new_user()
{
	User *uptr;
	
	uptr = (User *)malloc(sizeof(User));
	if (!uptr)
		out_of_memory(); /* out of memory ?!?!? */
	bzero(uptr, sizeof(User));
	uptr->prev = lastUser;
	if (lastUser)
	    lastUser->next = uptr;
	else
	    lastUser = firstUser = uptr;
	uptr->next = (User *)NULL;
	lastUser = uptr;
	uptr->away = (char *)NULL;
	uptr->channel = (UClist *)NULL;
	return uptr;
}

User *make_user(Client *cptr, Server *svptr)
{
	User *uptr;
	
	uptr = new_user();
	uptr->cptr = cptr;
	uptr->server = svptr;
	cptr->user = uptr;
	return(uptr);
}

void remove_user(User *uptr)
{
	if (uptr->prev)
		uptr->prev->next = uptr->next;
	else
		firstUser = uptr->next;
	if (uptr->next)
		uptr->next->prev = uptr->prev;
	else
		lastUser = uptr->prev;
	free(uptr);
	return;
}

/* Server structures */

Server *new_server()
{
	Server *svptr;
	
	svptr = (Server *)malloc(sizeof(Server));
	if (!svptr)
		out_of_memory();
	bzero(svptr, sizeof(Server));
	svptr->prev = lastServer;
        if (lastServer)
 	    lastServer->next = svptr;
        else
	    lastServer = firstServer = svptr;
	svptr->next = (Server *)NULL;
	lastServer = svptr;
	svptr->uplink = svptr->downlink = svptr->leftlink = svptr->rightlink =
			(Server *)NULL;
	return svptr;
}

void del_server(Server *svptr)
{
	if (svptr->prev)
		svptr->prev->next = svptr->next;
	else
		firstServer = svptr->next;
	if (svptr->next)
		svptr->next->prev = svptr->prev;
	else
		lastServer = svptr->prev;
	(void)free(svptr);
	return;
}

Server *make_server(Client *cptr, Server *uplink)
{
	Server *svptr, *sv2ptr;
	
	svptr = new_server();
	svptr->uplink = uplink;
	if (uplink && uplink->downlink)
	{
		for (sv2ptr = uplink->downlink; sv2ptr->rightlink;
				sv2ptr = sv2ptr->rightlink)
			;
		sv2ptr->rightlink = svptr;
		svptr->leftlink = sv2ptr;
	}
	else if (uplink)
		uplink->downlink = svptr;
	return(svptr);
}

void remove_server(Server *svptr)
{
	
	if (svptr->uplink->downlink == svptr)
		svptr->uplink->downlink = svptr->rightlink;
	if (svptr->rightlink)
		svptr->rightlink->leftlink = svptr->leftlink;
	if (svptr->leftlink)
		svptr->leftlink->rightlink = svptr->rightlink;
	del_server(svptr);
	return;
}

/* Channel linked list handling */

Chan *new_channel(char *name)
{
	Chan *chptr;
	
	chptr = (Chan *)malloc(sizeof(Chan));
	if (!chptr)
		out_of_memory();
	bzero(chptr, sizeof(Chan));
	chptr->banlist = (Ban *)NULL;
	chptr->members = (Member *)NULL;
	chptr->prev = lastChan;
	if (lastChan)
		lastChan->next = chptr;
	chptr->next = (Chan *)NULL;
	lastChan = chptr;
	chptr->members = (Member *)NULL;
	strncpyzt(chptr->chname, name, CHANLEN);
	add_channel_hash_entry(chptr);
	return chptr;
}

void del_channel(Chan *chptr)
{
	if (chptr->prev)
		chptr->prev->next = chptr->next;
	else
		firstChan = chptr->next;
	if (chptr->next)
		chptr->next->prev = chptr->prev;
	else
		lastChan = chptr->prev;
	del_channel_hash_entry(chptr);
	free(chptr);
	return;
}

/* Member list handling */

Member	*find_member(Chan *chptr, Client *cptr)
{
	register Member *mptr;
	
	for (mptr = cptr->user->channel; mptr && mptr->chptr != chptr;
			mptr = mptr->nextc)
		;
	return(mptr);
}

Member	*add_user_to_channel(Chan *chptr, Client *cptr)
{
	register Member *mptr;

	mptr = (Member *)malloc(sizeof(Member));
	if (!mptr)
		out_of_memory();
	bzero(mptr, sizeof(Member));
	mptr->nextu = chptr->members;
	mptr->nextc = cptr->user->channel;
	mptr->chptr = chptr;
	mptr->cptr = cptr;
	chptr->occupants++;
	return (mptr);
}

int rem_user_from_channel(Member *mptr)
{
	register Member *m2ptr;
	register Chan *chptr;
	User *uptr;
	
	if (!mptr)
		return(-1);
	chptr = mptr->chptr;
	uptr = mptr->cptr->user;
	if (mptr == chptr->members)
		chptr->members = mptr->nextu;
	else
	{
		for (m2ptr = chptr->members; m2ptr->nextu
				&& m2ptr->nextu != mptr; m2ptr = m2ptr->nextu)
			;
		m2ptr->nextu = mptr->nextu;
	}
	if (mptr == uptr->channel)
		uptr->channel = mptr->nextc;
	else
	{
		for (m2ptr = uptr->channel; m2ptr->nextc
				&& m2ptr->nextc != mptr; m2ptr = m2ptr->nextc)
			;
		m2ptr->nextu = mptr->nextu;
	}
	chptr->occupants--;
	free(mptr);
	return(0);
}


/* Ban list handling of channels */


/* NOTE: Only specify either the channel and client OR the member */

int is_banned(Channel *chptr, Client *cptr, Member *mptr)
{
	register Ban *bptr;
	register char *nus;
	
	if (mptr)
	{
		if (mptr->flags & CHFL_BANNED)
			return(1);
		else if (mptr->flags & CHFL_NOTBANNED)
			return(0);
		cptr = mptr->cptr;
		chptr = mptr->chptr;
	}
	
	if (!chptr->banlist)
		return(0); /* ban list is empty, this is common */
		
	nus = get_userhost(cptr);

	for (bptr = chptr->banlist; bptr; bptr = bptr->next)
	{
		if (match(bptr->banmask, nus))
			return(1);
	}
	return(0);
}

char *get_userhost(Client *acptr)
{
	static char res[NICKLEN+USERLEN+HOSTLEN+5];
	
	if (!acptr || !IsPerson(acptr))
		return NULL;
	
	sprintf(res, "%s!%s@%s", acptr->name, acptr->user->username, acptr->host);
	return res;
}

int ban_is_redundant(Channel *chptr, char *ban)
{
	register Ban *bptr;
	
	for (bptr = chptr->banlist; bptr; bptr = bptr->next)
	{
		if (match(bptr->banmask, ban))
			return(1);
	}
	return(0);
}

int add_ban_to_channel(Chan *chptr, char *banmask)
{
	Ban *bptr;
	Member *mptr;

	if (ban_is_redundant(chptr, banmask))
		return(0);

	bptr = (Ban *)malloc(sizeof(Ban));
	if (!bptr)
		out_of_memory();
	
	bptr->next = chptr->banlist;
	chptr->banlist = bptr;
	bptr->banmask = banmask;
	for (mptr = chptr->members; mptr; mptr = mptr->nextu)
		mptr->flags &= ~(CHFL_BANNED|CHFL_NOTBANNED);
	return(1);
}

void del_ban_from_channel(Chan *chptr, Ban *bptr)
{
	register Ban *b2ptr;
	
	if (chptr->banlist == bptr)
		chptr->banlist = bptr->next;
	else
	{
		for (b2ptr = chptr->banlist; b2ptr->next != bptr;
				b2ptr = b2ptr->next)
			;
		b2ptr->next = bptr->next;
	}
	free(bptr);
	return;
}

/* Inbound socket connection structures */

Connection *new_inlink()
{
	Connection *link;
	
	link = new Connection;
	if (!link)
		out_of_memory();
	LIST_ENTRY_INIT(link, lp_con);
	LIST_INSERT_HEAD(&inlinks, link, lp_con);

	return link;
}

void del_inlink(Connection *link)
{
	LIST_REMOVE(link, lp_con);
	delete link;
}
