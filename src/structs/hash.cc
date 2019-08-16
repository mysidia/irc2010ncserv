/*
 *   pIrcd ports source file                            src/structs/hash.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 Mysidia <***REMOVED***> <Mysidia>
 *   Copyright (C) 1999 Garry Boyer <binary>
 *   see file 'AUTHORS' for a comprehensive list of pIrcd authors
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public license Version 2
 *   as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
** hash.c
**
**  does main hash table maintenance functions
*/

#define __hash_cc__
#include "pircd.h"
#include "hash.h"

/* hash buckets */

/* With 4096 buckets, a network with 20,000 users would on average have to
** do str compares with 5 entries.  Also, removing the high 4 bits when
** determining the bucket helps even out the load.
*/

Client	*client_buckets[4096];
Channel	*channel_buckets[4096];
iextern void init_hash2();

/*!
 * set up the hash tables
 */

void init_hash()
{
	int x;
	
	for (x = 0; x < 4096; x++)
	{
		client_buckets[x] = (Client *)NULL;
		channel_buckets[x] = (Chan *)NULL;
	}
	init_hash2();
}


/*
** find_bucket - used to find the bucket # for a name
**
** We want to use the same bucket-finding mechanism every time without
** the wasted CPU cycles needed for a function call, so a #define is used.
**
** Also note that we are penny-pinching for CPU time.
*/

#define	find_bucket(__x, equadv) {					\
	equadv.q = *(unsigned long *)(__x) & 0x0F0F0F0F;		\
	bucketnum = (equadv.w.w1|(equadv.b.b3 << 4));			\
		      }
			  
/*
** OK, what was that code above?
** First, the first four-byte chunk of name is moved into quadv.
** Next, all of the single chars at the same time undergo "& 0x0F", which is
**	essentially the same as "% 16" but less CPU-intensive.
** Bucketnum will be the first word (format 0x0F0F) plus quadv.b.b3 shifted
**	4 bits to the left (format 0x00F0).  When the 0x0F0F and 0x00F0
**	are added together, they fill in a complete 0x0FFF.
*/



/*********** Client hashing ***********/

void add_client_hash_entry(Client *cptr)
{
	int bucketnum;
	register quadchar quadv;
	
	find_bucket(cptr->name, quadv);
	cptr->hnext = client_buckets[bucketnum];
	client_buckets[bucketnum] = cptr;
}

void del_client_hash_entry(Client *cptr)
{
	int bucketnum;
	register quadchar quadv;
	register Client *c2ptr, *c3ptr;
	
	find_bucket(cptr->name, quadv);
	for (c2ptr = client_buckets[bucketnum]; c2ptr != cptr;
			c3ptr = c2ptr = c2ptr->hnext)
		;
	c3ptr->hnext = c2ptr->hnext;
	c2ptr->hnext = (Client *)NULL;
}

Client *find_client_hash_entry(char *name)
{
	register quadchar quadv;
	register Client *cptr;
	register Client *c2ptr = (Client *)NULL;
	char *nam;
	char buf[4];
	int bucketnum;
	
	if (!name[1])		/* nick is 1 char */
	{
		buf[0] = *name;
		buf[1] = '\0';
		buf[2] = '\0';
		buf[3] = '\0';
		nam = buf;
	}
	else if (!name[2])	/* nick is 2 chars */
	{
		buf[0] = *name;
		buf[1] = name[1];
		buf[2] = '\0';
		buf[3] = '\0';
		nam = buf;	
	}			/* 3-char nicks already have the \0 */
	else
		nam = name;
	find_bucket(nam, quadv);
	cptr = client_buckets[bucketnum];
	for (cptr = client_buckets[bucketnum]; cptr;
		c2ptr = cptr, cptr = cptr->hnext)
	{
		if (!mycmp((u_char *)cptr->name, (u_char *)name))
		{
			if (c2ptr)
			{	/* move to top of list... */
				c2ptr->hnext = cptr->hnext;
				cptr->hnext = client_buckets[bucketnum];
			}
			return(cptr);
		}
	}
	return((Client *)NULL);
}


/************* Channel hashing **************/

void add_channel_hash_entry(Chan *chptr)
{
	register int bucketnum;
	register quadchar quadv;
	
	find_bucket(chptr->chname+1, quadv);
	chptr->hnext = channel_buckets[bucketnum];
	channel_buckets[bucketnum] = chptr;
}

void del_channel_hash_entry(Chan *chptr)
{
	register Chan *ch2ptr, *ch3ptr;
	register quadchar quadv;
	int bucketnum;
	
	find_bucket(chptr->chname+1, quadv);
	for (ch2ptr = channel_buckets[bucketnum]; ch2ptr != chptr;
			ch3ptr = ch2ptr = ch2ptr->hnext)
		;
	ch3ptr->hnext = ch2ptr->hnext;
	ch2ptr->hnext = (Channel *)NULL;
}

Chan *find_channel_hash_entry(char *name)
{
	register quadchar quadv;
	register Chan *chptr;
	register Chan *ch2ptr = (Chan *)NULL;
	char buf[4], *nam;
	int bucketnum;

	/* Can we be sure that there isn't garbage after a \0 for short names?
	** Well, we could try to use secure string functions, or we could do
	** quad-byte string handling.
	** For now, we'll just do this.
	*/
	
	if (!name[1]) /* channel name is "#" */
	{
		buf[0] = '\0';
		buf[1] = '\0';
		buf[2] = '\0';
		buf[3] = '\0';
		nam = buf;
	}
	else if (!name[2]) /* channel name is "#x" */
	{
		buf[0] = name[1];
		buf[1] = '\0';
		buf[2] = '\0';
		buf[3] = '\0';
		nam = buf;
	}
	else if (!name[3]) /* channel name is "#xx" */
	{
		buf[0] = name[1];
		buf[1] = name[2];
		buf[2] = '\0';
		buf[3] = '\0';
		nam = buf;
	}
	else
		nam = name+1;
	find_bucket(nam, quadv);
	chptr = channel_buckets[bucketnum];
	for (chptr = channel_buckets[bucketnum]; chptr;
		ch2ptr = chptr, chptr = chptr->next)
	{
		if (!mycmp(chptr->chname, name))
		{
			if (ch2ptr)
			{	/* move to top of list... */
				ch2ptr->next = chptr->next;
				chptr->next = channel_buckets[bucketnum];
			}
			return(chptr);
		}
	}
	return((Chan *)NULL);
}

/***************************************************************************************
 * generic hashing
 ***************************************************************************************/
iextern int gethashkey(const char *xhname);

#define H2
#include "hash.h"

void zero_hash(hashlist *t)
{
	int i = 0;
	hasharray *h, *h_next;

	for (i = 0 ; i < t->buckets; i ++)
	{
		for (h = t->hash[i] ; h; h = h_next)
		{
			h_next = h->next;
			free(h);
		}
		t->hash[i] = NULL;
   	}
}

idecl void init_hash2()
{
	int i = 0, x = 0;

	for (x = 0; hashtab[x].table != NULL; x++)
	{
		hashlist *d = hashtab[x].table;
		if (d->buckets <= hashtab[x].buckets)
		d->buckets = hashtab[x].buckets;
		d->hash = (hasharray **)realloc(d->hash,
				(sizeof(hasharray **)*(d->buckets+2)));
		for (i = 0 ; i < d->buckets; i ++)
			d->hash[i] = NULL;
	}
}


idecl int gethashkey(const char *xhname)
{       
	int    i = 0;
	int     hash = 0x5555;
	
	for (i=0; xhname[i] && i < 4; i++)
		hash = (hash<<2) ^ tolower(xhname[i]);
	if (hash < 0)
		hash = -hash;
	return (hash);
}

void *hash_lookup(hashlist *hashl, const char *key)
{
	hasharray *ins;
	int hashkey = gethashkey(key)%FTRUE(hashl->buckets);
	
	for (ins = hashl->hash[hashkey];ins; ins = ins->next)
	{
		if (strcasecmp(key, ins->key)==0)
			return ins->data;
	}
	return NULL;
}

void hash_insert(hashlist *hashl, const char *key, void *instruct)
{
	hasharray *ins, *tmp;
	int hashkey = gethashkey(key)%FTRUE(hashl->buckets);
	tmp = (hasharray *)calloc(1, sizeof(hasharray));
	tmp->key = strdup(key);
	tmp->data = instruct;
	if ( hashl->hash[hashkey] )
	{
		ins = hashl->hash[hashkey];
		hashl->hash[hashkey] = tmp;
		tmp->next = ins;
	}
	else
		hashl->hash[hashkey] = tmp;
}

/* delete by key */
void hash_delete(hashlist *hashl, char *key)
{
	hasharray *ins, *tmp;
	int hashkey = gethashkey(key)%FTRUE(hashl->buckets);

	for (ins = hashl->hash[hashkey], tmp = NULL;ins; ins = ins->next)
	{
		if (strcasecmp(key, ins->key)==0)
		{
			if (tmp)
				tmp->next = ins->next;
			else
				hashl->hash[hashkey] = ins->next;
			if (ins->key)
				free(ins->key);
			free(ins);
			return;
		}
		tmp = ins;
	}
	return;
}

/* delete by datum */
int hash_delete_bydatum(hashlist *hashl, char *key, void *datum)
{
	hasharray *ins, *tmp;
	int hashkey = gethashkey(key)%FTRUE(hashl->buckets);

	for (ins = hashl->hash[hashkey], tmp = NULL;ins; ins = ins->next)
	{
		if (ins->data && (ins->data == datum))
		{
			if (tmp)
				tmp->next = ins->next;
			else
				hashl->hash[hashkey] = ins->next;
			if (ins->key)
				free(ins->key);
			free(ins);
			return 1;
		}
		tmp = ins;
	}
	return 0;
}


/*
 * functions to manipulate and read x hashtable
 */
#define HASH_FUNCTIONS(datatype, index, keyname, hashname) \
int add_hash_##index (datatype *u) \
{ \
       if (u) \
         hash_insert(&(hashname), (keyname), u); \
       else  \
         return -1; \
       return 0;; \
} \
\
void remove_hash_##index (datatype *u) \
{ \
       if (u) \
         hash_delete_bydatum(&(hashname), (keyname), u); \
       return; \
} \
\
datatype *hash_find_##index (const char *seek) \
{ \
       if (seek) \
         return ((datatype *)hash_lookup(&(hashname), (seek))); \
       else \
         return NULL; \
}

#define H3
#include "hash.h"

/**********************************************************************/
