/* Testing of hashes and structs */

#include "pircd.h"

void list()
{
	Client *cptr;
	User *uptr;
	Server *svptr;
	Chan *chptr;
	Ban *bptr;
	Member *mptr;
	int i;
	
	puts("Clients:");
	for (cptr = firstClient; cptr; cptr = cptr->next)
	{
		puts(cptr->name);
		i++;
		if (i > 40)
			getchar();
	}
	puts("Channels:");
	for (chptr = firstChan; chptr; chptr = chptr->next)
	{
		puts(chptr->chname);
		printf("- Bans: ");
		for (bptr = chptr->banlist; bptr; bptr = bptr->next)
		{
			printf("%s, ", bptr->banmask);
		}
		printf("\n- Members: ");
		for (mptr = chptr->members; mptr; mptr = mptr->next)
		{
			printf("%s, ", mptr->cptr->name);
		}
		i += 3;
		if (i > 40)
			getchar();
	}
}
	

void main()
{
	Client *cptr, *c2ptr, *c3ptr, *meptr;
	User *uptr, *u2ptr, *u3ptr;
	Server *svptr;
	Chan *chptr, *ch2ptr;
	Member *mptr, *m2ptr;
	
	meptr = new_client("myserver.bleh.net");
	svptr = make_server(meptr, (Server *)NULL);
	cptr = new_client("binary");
	uptr = make_user(cptr, svptr);
	chptr = new_channel("#test");
	mptr = add_user_to_channel(chptr, cptr);
	list();
}
