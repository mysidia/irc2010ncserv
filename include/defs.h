#define IRCD_CONF	"ircd.cf"
#define	MAXNEST		4		/* maximum nested blocks in ircd.cf */
#define	OFILEBUF	1024
#define	OSOCKBUF	8192
#define MAXHOST		63		/* maximum length of a host (dont change)*/
#define CONFLINE	255		/* maximum length of a conf line*/
#undef	RFC1459				/* when defined pIrcd will be rfc1459 compliant */
#undef	DF467				/* when defined pIrcd will try to get along with df467 servers on the network */

 /*
    these should either be un-defined or point to 
    alternate dns resolver config files
 */
/* #define IRCD_HOSTCONF	"/etc/host.conf" */
#define IRCD_RESCONF		"/etc/resolv.conf"

#define MAXCONNECTIONS		150
#define LISTEN_SIZE		5
