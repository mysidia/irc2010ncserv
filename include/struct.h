/* 
** include/struct.h
** 
**   defines structures and other information needed for storing information
**    in RAM
*/

#ifndef __struct_h_include__
#define __struct_h_include__

#include "queue.h"
#include "flags.h"
typedef unsigned char token_t;

#define NICKLEN 30
#define USERLEN 8
#define HOSTLEN 64
#define	REALLEN 128
#define KEYLEN	32
#define CHANLEN	64

class Connection;
class Client;
class User;
class Server;
class Ban;
class ChanMode;
class ChanMember;
class Channel;

#include "client.h"
#include "channel.h"

typedef class Client aClient;		/* client type */
typedef class User anUser;		/* specific user info */
typedef class Server Server;		/* specific server info */
typedef class Channel Chan;		/* channel type */
typedef class ChanMember UClist;	/* channel-user chain list */
typedef class ChanMember Member;	/* channel-user chain list */
typedef class ChanMode CMode;		/* channel mode */
typedef struct sListener aListener;

struct sListener
{
	char *addy;
	int port, fd;
	LIST_ENTRY(sListener)   portent;
};


extern Client *firstClient, *lastClient;
extern User *firstUser, *lastUser;
extern Server *firstServer, *lastServer;
extern Chan *firstChan, *lastChan;

/* Client *local[MAXCONNECTIONS]; */

#define find_client(name) find_client_hash_entry(name)
#define find_channel(name) find_channel_hash_entry(name)
#ifdef __cplusplus
#define malloc pircd_alloc::oalloc
#define free pircd_alloc::ofree
#endif

#endif
