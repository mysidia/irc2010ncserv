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

/*
** include/hash.h
**
**  Sets up the basic structure of the hash tables
*/

#if !defined(EHASH_DECL)
#define EHASH_DECL(x)   extern hashlist x
#define HASH_DECL(x)    hashlist x
#endif
#ifdef H3
#undef H3

/* HASH_FUNCTIONS ( <datatype>, <for f(x) names>, <key element>, <hash names for var names> )
 * add one of these to make hash_find_<x> functions*/
HASH_FUNCTIONS( int, hltest, litoa((int)u), hltest);

#elif defined( H2 )
/* hash data/vars */

HASH_DECL( hltest );
struct hashlist_data hashtab[] =
    {
        { &hltest, 500     },
        { NULL,       0    }
    };

#undef H2
#elif defined( H1 )
/* prototypes for hash_find_<x> functions */
PROTO_HASH_FUNCTIONS( int, hltest  );

#elif defined( H0 )
/* external declaration of hashtab vars */
extern HASH_DECL(hltest);

#undef H0
struct hashlist_data {
     hashlist *table;
     int buckets;
};
#endif

/* Quick hashing - sorts by using fragments of the first 3 letters of a name */

#ifndef __hash_h_include__
#define __hash_h_include__
#include "struct.h"

typedef struct hasharray_ hasharray;
typedef struct hashlist_ hashlist;

#define H0
#include "hash.h"

#define	STANDARD_LONG_ARRANGEMENT	1

#ifdef	STANDARD_LONG_ARRANGEMENT /* This should work on most systems... */
#define	QUAD_B1_4 1
#define	QUAD_B2_3 1
#define	QUAD_B3_2 1
#define	QUAD_B4_1 1
#endif

#ifdef	QUAD_B1_1
#define	QUAD_W1_1 1
#endif
#ifdef	QUAD_B2_1
#define	QUAD_W1_1 1
#endif

typedef union squadchar quadchar;
#define quadchar union squadchar

union squadchar
    {
	struct
	    {
	    	/* Maybe not all CPU's and OS's have the same arrangment
	    	** of bytes in a 32-bit value for RAM.  I know that standard
	    	** i86's arrange them from lowest byte to highest byte.
	    	** The config file will make sure what it is.
	    	** When a string is cast to quadchar.q, b1 should be the
	    	** first letter of the string, b2 the second, and so forth.
	    	** The purpose of using quadchar is because it takes
	    	** less CPU to modify four bytes as a whole LONG
	    	** than it is to deal with them individually.
	    	*/
	    	
#ifdef	QUAD_B1_1
		char b1;
#else
# ifdef	QUAD_B2_1
		char b2;
# else
#  ifdef QUAD_B3_1
		char b3;
#  else
		char b4;
#  endif
# endif
#endif


#ifdef	QUAD_B1_2
		char b1;
#else
# ifdef	QUAD_B2_2
		char b2;
# else
#  ifdef QUAD_B3_2
		char b3;
#  else
		char b4;
#  endif
# endif
#endif


#ifdef	QUAD_B1_3
		char b1;
#else
# ifdef	QUAD_B2_3
		char b2;
# else
#  ifdef QUAD_B3_3
		char b3;
#  else
		char b4;
#  endif
# endif
#endif


#ifdef	QUAD_B1_4
		char b1;
#else
# ifdef	QUAD_B2_4
		char b2;
# else
#  ifdef QUAD_B3_4
		char b3;
#  else
		char b4;
#  endif
# endif
#endif
	    } b;
	struct
	    {
#ifdef	QUAD_W1_1
		unsigned short w1, w2;
#else
		unsigned short w2, w1;
#endif
	    } w;
	unsigned long q;
    };

/***********************************************************************************
 *    generic hashes for storing things looked up frequently
 ***********************************************************************************/
struct hasharray_
{
   char *key;
   void *data;
   struct hasharray_ *next; 
};

struct hashlist_
{
   int size;
   int buckets;
   hasharray **hash;
};


extern void		init_hash();
extern void		*hash_lookup(hashlist *hashl, const char *key);
extern void		hash_insert(hashlist *hashl, const char *key, void *instruct);
extern void		hash_delete(hashlist *hashl, char *key);
extern int		hash_delete_bydatum(hashlist *hashl, char *key, void *datum);
extern void		zero_hash(hashlist *t);

#define PROTO_HASH_FUNCTIONS(datatype, index)               \
extern datatype		*hash_find_##index (const char *); \
extern void		remove_hash_##index (datatype *u); \
extern int		add_hash_##index (datatype *)

#endif

