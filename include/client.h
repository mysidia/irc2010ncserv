#ifndef __client_h__
#define __client_h__

class Connection				
{						
	public:					
	char ident[USERLEN+1];			
	flag_t con_type;			
        time_t last_cmd, signon, ping_sent;	
	int fd;				
	Client *cli;				
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
