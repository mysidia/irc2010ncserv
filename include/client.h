#ifndef __client_h__
#define __client_h__

typedef enum {
   CON_UNDEFINED, CON_UNKNOWN, CON_USER, CON_SERVER
} connection_type;

class Connection				
{						
	public:					
	char ident[USERLEN+1];			
	connection_type con_type;			
	time_t last_cmd, signon, ping_sent;	
	struct in_addr ip;
	Client *client;

	int fd;				
	LIST_ENTRY(Connection)   lp_con;

	Connection() :
          con_type(CON_UNDEFINED), last_cmd(0), signon(0), ping_sent(0)
        {
          ident[0] = '\0';
          ip.s_addr = 0;
	}

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
