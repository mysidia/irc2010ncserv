/*
 * port.so
 */

extern		int pircd_mycmp(u_char *, u_char *);
#ifdef __cplusplus
extern		int pircd_mycmp(char *, char *);
#endif
extern		int init_tabs();

/*
 *  conf.so
 */
extern		int	read_config(const char *);
extern		void	free_strings(char **x);

extern		ConfBlock	*getblockbypath(char *path);

/*
 *  socket.so
 */
extern		void	noblock(int fd);
extern		void	set_sock_opts(int fd);
extern		void	want_to_listen(char *addy, int port);
extern		void	lqueue_delete(char *addy, int port);
extern		void	start_listening(struct sListener *);
extern		void	stop_listening(struct sListener *);
extern		void	update_listeners();

/*
 *  misc.so
 */
extern		void	mm_boot();
extern		void	oom();
extern		int	match(char *mask, char *name);
extern		void	log_error(const char *fmt, ...);
extern		void	logit(const char *fmt, ...);
extern		void	fdprintf(int fd, const char *fmt, ...);
extern		void DebugLog(int loglevel, const char *fmt, ...);
extern		void	line_splitter(char *buffer, int (* sP)(char *) );
extern		char	*ctoip (const char *f);
extern		char	*iptoc (const char *f);
extern		char	*litoa(long);
namespace pircd_alloc { extern void *oalloc(size_t), ofree(void *); }

/*
 *  resolver.so
 */
extern		int	dnsloop(int);
extern		int	boot_dns(int args, char *argc[], char *envp[]);
extern		int	dnsdie(int s = 0);
extern		int	dnsline(char *buf);


/*
 *  struct.c
 */
extern 		void	init_lists();
extern 		Client	*new_client(char *name);
extern 		void	del_client(Client *cptr);
extern 		User	*new_user();
extern 		User	*make_user(Client *cptr, Server *svptr);
extern 		void	remove_user(User *uptr);
extern 		Server	*new_server();
extern 		void	del_server(Server *svptr);
extern 		Server	*make_server(Client *cptr, Server *uplink);
extern 		void	remove_server(Server *svptr);
extern 		Chan	*new_channel(char *name);
extern 		void	del_channel(Chan *chptr);
extern 		Member	*find_member(Chan *chptr, Client *cptr);
extern 		Member	*add_user_to_channel(Chan *chptr, Client *cptr);
extern 		int	rem_user_from_channel(Member *mptr);
extern 		int	is_banned(Channel *chptr, Client *cptr, Member *mptr);
extern 		int	ban_is_redundant(Channel *chptr, char *ban);
extern 		int	add_ban_to_channel(Chan *chptr, char *banmask);
extern 		void	del_ban_from_channel(Chan *chptr, Ban *bptr);
extern		char	*get_userhost(Client *acptr);

/*
 * list.c/lists
 */
extern		void	free_client(Client *);
iextern		void	use_client(Client *);
iextern		void	rel_client(Client *);
extern		void	free_listener(aListener *);


/*
 *  hash.c
 */
extern		void	init_hash();
extern		void 	add_client_hash_entry(Client *cptr);
extern		void 	del_client_hash_entry(Client *cptr);
extern		Client 	*find_client_hash_entry(char *name);
extern		void	add_channel_hash_entry(Chan *chptr);
extern		void	del_channel_hash_entry(Chan *chptr);
extern		Chan	*find_channel_hash_entry(char *name);

/*
 * proc.c
 */
extern		void initsetproctitle(int, char **, char **);
extern		void setproctitle(const char *, ...);



#define H1
#include "hash.h"
