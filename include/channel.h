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

