#include "pircd.h"
#define VA_LOCAL_DECL   va_list ap;
#define VA_START(f)     va_start(ap, f)
#define VA_END          va_end(ap)

/*
   **  SETPROCTITLE -- set process title for ps
   **
   **   Parameters:
   **           fmt -- a printf style format string.
   **           a, b, c -- possible parameters to fmt.
   **
   **   Returns:
   **           none.
   **
   **   Side Effects:
   **           Clobbers argv of our main procedure so ps(1) will
   **           display the title.
 */
#define SPT_NONE        0       /* don't use it at all */
#define SPT_REUSEARGV   1       /* cover argv with title information */
#define SPT_BUILTIN     2       /* use libc builtin */
#define SPT_PSTAT       3       /* use pstat(PSTAT_SETCMD, ...) */
#define SPT_PSSTRINGS   4       /* use PS_STRINGS->... */
#define SPT_SYSMIPS     5       /* use sysmips() supported by NEWS-OS 6 */
#define SPT_SCO         6       /* write kernel u. area */
#define SPT_CHANGEARGV  7       /* write our own strings into argv[] */
#define MAXLINE      2048       /* max line length for setproctitle */
#define SPACELEFT(buf, ptr)  (sizeof buf - ((ptr) - buf))

#ifndef SPT_TYPE
#define SPT_TYPE        SPT_REUSEARGV
#endif

#if SPT_TYPE != SPT_NONE && SPT_TYPE != SPT_BUILTIN

#if SPT_TYPE == SPT_PSTAT
#include <sys/pstat.h>
#endif
#if SPT_TYPE == SPT_PSSTRINGS
#include <machine/vmparam.h>
#include <sys/exec.h>
#ifndef PS_STRINGS              /* hmmmm....  apparently not available after all */
#undef SPT_TYPE
#define SPT_TYPE        SPT_REUSEARGV
#else
#ifndef NKPDE                   /* FreeBSD 2.0 */
#define NKPDE 63
typedef unsigned int *pt_entry_t;
#endif
#endif
#endif
#if SPT_TYPE == SPT_PSSTRINGS || SPT_TYPE == SPT_CHANGEARGV
#define SETPROC_STATIC  static
#else
#define SETPROC_STATIC
#endif

#if SPT_TYPE == SPT_SYSMIPS
#include <sys/sysmips.h>
#include <sys/sysnews.h>
#endif

#if SPT_TYPE == SPT_SCO
#ifdef UNIXWARE
#include <sys/exec.h>
#include <sys/ksym.h>
#include <sys/proc.h>
#include <sys/user.h>
#else /* UNIXWARE */
#include <sys/immu.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/fs/s5param.h>
#endif /* UNIXWARE */
#if PSARGSZ > MAXLINE
#define SPT_BUFSIZE     PSARGSZ
#endif
#ifndef _PATH_KMEM
#define _PATH_KMEM      "/dev/kmem"
#endif /* _PATH_KMEM */
#endif /* SPT_SCO */

#ifndef SPT_PADCHAR
#define SPT_PADCHAR     ' '
#endif

#ifndef SPT_BUFSIZE
#define SPT_BUFSIZE     MAXLINE
#endif

#endif /* SPT_TYPE != SPT_NONE && SPT_TYPE != SPT_BUILTIN */

#if SPT_TYPE == SPT_REUSEARGV || SPT_TYPE == SPT_CHANGEARGV
char **Argv = NULL;             /* pointer to argument vector */
#endif

#if SPT_TYPE == SPT_REUSEARGV
char *LastArgv = NULL;          /* end of argv */
#endif

/*
   **  Pointers for setproctitle.
   **   This allows "ps" listings to give more useful information.
 */
void initsetproctitle(int argc, char **argv, char **envp)
{
#if SPT_TYPE == SPT_REUSEARGV
    register int i, envpsize = 0;
    char **newenviron;
    extern char **environ;

    /*
       **  Save start and extent of argv for setproctitle.
     */

    LastArgv = argv[argc - 1] + strlen(argv[argc - 1]);
    if (envp != NULL) {
        /*
           **  Move the environment so setproctitle can use the space at
           **  the top of memory.
         */
        for (i = 0; envp[i] != NULL; i++)
            envpsize += strlen(envp[i]) + 1;
        newenviron = (char **) malloc(sizeof(char *) * (i + 1));
        if (newenviron) {
            int err = 0;
            for (i = 0; envp[i] != NULL; i++) {
                if ((newenviron[i] = strdup(envp[i])) == NULL) {
                    err = 1;
                    break;
                }
            }
            if (err) {
                for (i = 0; newenviron[i] != NULL; i++)
                    free(newenviron[i]);
                free(newenviron);
                i = 0;
            }
            else {
                newenviron[i] = NULL;
                environ = newenviron;
            }
        }
        else {
            i = 0;
        }

        /*
           **  Find the last environment variable within wu-ftpd's
           **  process memory area.
         */
        while (i > 0 && (envp[i - 1] < argv[0] ||
                    envp[i - 1] > (argv[argc - 1] + strlen(argv[argc - 1]) +
                                   1 + envpsize)))
            i--;

        if (i > 0)
            LastArgv = envp[i - 1] + strlen(envp[i - 1]);
    }
#endif /* SPT_TYPE == SPT_REUSEARGV */

#if SPT_TYPE == SPT_REUSEARGV || SPT_TYPE == SPT_CHANGEARGV
    Argv = argv;
#endif
}


#if SPT_TYPE != SPT_BUILTIN

/*VARARGS1 */
void setproctitle(const char *fmt,...)
{
#if SPT_TYPE != SPT_NONE
    register char *p;
    register int i;
    SETPROC_STATIC char buf[SPT_BUFSIZE];
    VA_LOCAL_DECL
#if SPT_TYPE == SPT_PSTAT
        union pstun pst;
#endif
#if SPT_TYPE == SPT_SCO
    static off_t seek_off;
    static int kmemfd = -1;
    static int kmempid = -1;
#ifdef UNIXWARE
    off_t offset;
    void *ptr;
    struct mioc_rksym rks;
#endif /* UNIXWARE */
#endif /* SPT_SCO */

    p = buf;

    /* print ftpd: heading for grep */
    (void) strcpy(p, "pIrcd: ");
    p += strlen(p);

    /* print the argument string */
    VA_START(fmt);
    (void) vsnprintf(p, SPACELEFT(buf, p), fmt, ap);
    VA_END;

    i = strlen(buf);

#if SPT_TYPE == SPT_PSTAT
    pst.pst_command = buf;
    pstat(PSTAT_SETCMD, pst, i, 0, 0);
#endif
#if SPT_TYPE == SPT_PSSTRINGS
    PS_STRINGS->ps_nargvstr = 1;
    PS_STRINGS->ps_argvstr = buf;
#endif
#if SPT_TYPE == SPT_SYSMIPS
    sysmips(SONY_SYSNEWS, NEWS_SETPSARGS, buf);
#endif
#if SPT_TYPE == SPT_SCO
    if (kmemfd < 0 || kmempid != getpid()) {
        if (kmemfd >= 0)
            close(kmemfd);
        if ((kmemfd = open(_PATH_KMEM, O_RDWR, 0)) < 0)
            return;
        (void) fcntl(kmemfd, F_SETFD, 1);
        kmempid = getpid();
#ifdef UNIXWARE
        seek_off = 0;
        rks.mirk_symname = "upointer";
        rks.mirk_buf = &ptr;
        rks.mirk_buflen = sizeof(ptr);
        if (ioctl(kmemfd, MIOC_READKSYM, &rks) < 0)
            return;
        offset = (off_t) ptr + (off_t) & ((struct user *) 0)->u_procp;
        if (lseek(kmemfd, offset, SEEK_SET) != offset)
            return;
        if (read(kmemfd, &ptr, sizeof(ptr)) != sizeof(ptr))
            return;
        offset = (off_t) ptr + (off_t) & ((struct proc *) 0)->p_execinfo;
        if (lseek(kmemfd, offset, SEEK_SET) != offset)
            return;
        if (read(kmemfd, &ptr, sizeof(ptr)) != sizeof(ptr))
            return;
        seek_off = (off_t) ptr + (off_t) ((struct execinfo *) 0)->ei_psargs;
#else /* UNIXWARE */
        seek_off = UVUBLK + (off_t) & ((struct user *) 0)->u_psargs;
#endif /* UNIXWARE */
    }
#ifdef UNIXWARE
    if (seek_off == 0)
        return;
#endif /* UNIXWARE */
    buf[PSARGSZ - 1] = '\0';
    if (lseek(kmemfd, (off_t) seek_off, SEEK_SET) == seek_off)
        (void) write(kmemfd, buf, PSARGSZ);
#endif /* SPT_SCO */
#if SPT_TYPE == SPT_REUSEARGV
    if (i > LastArgv - Argv[0] - 2) {
        i = LastArgv - Argv[0] - 2;
        buf[i] = '\0';
    }
    (void) strcpy(Argv[0], buf);
    p = &Argv[0][i];
    while (p < LastArgv)
        *p++ = SPT_PADCHAR;
    Argv[1] = NULL;
#endif
#if SPT_TYPE == SPT_CHANGEARGV
    Argv[0] = buf;
    Argv[1] = 0;
#endif
#endif /* SPT_TYPE != SPT_NONE */
}

#endif /* SPT_TYPE != SPT_BUILTIN */

#if 0
#ifdef KERBEROS
/* thanks to gshapiro@wpi.wpi.edu for the following kerberosities */

void init_krb()
{
    char hostname[100];

#ifdef HAVE_SYSINFO
    if (sysinfo(SI_HOSTNAME, hostname, sizeof(hostname)) < 0) {
        perror("sysinfo");
#else
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("gethostname");
#endif
        exit(1);
    }
    if (strchr(hostname, '.'))
        *(strchr(hostname, '.')) = 0;

    sprintf(krb_ticket_name, "/var/dss/kerberos/tkt/tkt.%d", getpid());
    krb_set_tkt_string(krb_ticket_name);

    config_auth();

    if (krb_svc_init("hesiod", hostname, (char *) NULL, 0, (char *) NULL,
                     (char *) NULL) != KSUCCESS) {
        fprintf(stderr, "Couldn't initialize Kerberos\n");
        exit(1);
    }
    }
}

void end_krb()
{
    unlink(krb_ticket_name);
}

#endif /* KERBEROS */

void do_daemon(int argc, char **argv, char **envp)
{
    struct sockaddr_in server;
    struct servent *serv;
    int pgrp;
    int lsock;
    int one = 1;
    FILE *pidfile;
    int i;

    /* Some of this is "borrowed" from inn - lots of it isn't */

    if (be_daemon == 2) {
        /* Fork - so I'm not the owner of the process group any more */
        i = fork();
        if (i < 0) {
            syslog(LOG_ERR, "cant fork %m");
            exit(1);
        }
        /* No need for the parent any more */
        if (i > 0)
            exit(0);

#ifdef NO_SETSID
        pgrp = setpgrp(0, getpid());
#else
        pgrp = setsid();
#endif
        if (pgrp < 0) {
            syslog(LOG_ERR, "cannot daemonise: %m");
            exit(1);
        }
    }

    if (!Bypass_PID_Files)
        if ((pidfile = fopen(_PATH_FTPD_PID, "w"))) {
            fprintf(pidfile, "%ld\n", (long) getpid());
            fclose(pidfile);
        }
        else {
            syslog(LOG_ERR, "Cannot write pidfile: %m");
        }

    /* Close off all file descriptors and reopen syslog */
    if (be_daemon == 2) {
        int i, fds;
#ifdef HAVE_GETRLIMIT
        struct rlimit rlp;

        rlp.rlim_cur = rlp.rlim_max = RLIM_INFINITY;
        if (getrlimit(RLIMIT_NOFILE, &rlp))
            return;
        fds = rlp.rlim_cur;
#else
#ifdef HAVE_GETDTABLESIZE
        if ((fds = getdtablesize()) <= 0)
            return;
#else
#ifdef OPEN_MAX
        fds = OPEN_MAX;         /* need to include limits.h somehow */
#else
        fds = sizeof(long);     /* XXX -- magic */
#endif
#endif
#endif

        closelog();
        for (i = 0; i <= fds; i++) {
            close(i);
        }
#ifdef FACILITY
        openlog("ftpd", LOG_PID | LOG_NDELAY, FACILITY);
#else
        openlog("ftpd", LOG_PID);
#endif

        /* junk stderr */
        (void) freopen(_PATH_DEVNULL, "w", stderr);
    }

    if (RootDirectory != NULL) {
        if ((chroot(RootDirectory) < 0)
            || (chdir("/") < 0)) {
            syslog(LOG_ERR, "Cannot chroot to initial directory, aborting.");
            exit(1);
        }
        free(RootDirectory);
        RootDirectory = NULL;
    }

    if (!use_accessfile)
        syslog(LOG_WARNING, "FTP server started without ftpaccess file");

    syslog(LOG_INFO, "FTP server (%s) ready.", version);

    /* Create a socket to listen on */
    lsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (lsock < 0) {
        syslog(LOG_ERR, "Cannot create socket to listen on: %m");
        exit(1);
    }

    if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) < 0) {
        syslog(LOG_ERR, "Cannot set SO_REUSEADDR option: %m");
        exit(1);
    }
    if (keepalive)
        (void) setsockopt(lsock, SOL_SOCKET, SO_KEEPALIVE, (char *) &one, sizeof(one));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = acl_DaemonAddress();
    if (daemon_port == 0) {
        if (!(serv = getservbyname("ftp", "tcp"))) {
            syslog(LOG_ERR, "Cannot find service ftp: %m");
            exit(1);
        }
        server.sin_port = serv->s_port;
        daemon_port = ntohs(serv->s_port);
    }
    else
        server.sin_port = htons(daemon_port);

    if (bind(lsock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        syslog(LOG_ERR, "Cannot bind socket: %m");
        exit(1);
    }

    listen(lsock, MAX_BACKLOG);

    sprintf(proctitle, "accepting connections on port %i", daemon_port);
    setproctitle("%s", proctitle);

    while (1) {
        int pid;
        int msgsock;

        msgsock = accept(lsock, 0, 0);
        if (msgsock < 0) {
            syslog(LOG_ERR, "Accept failed: %m");
            sleep(1);
            continue;
        }
        if (keepalive)
            (void) setsockopt(msgsock, SOL_SOCKET, SO_KEEPALIVE, (char *) &one, sizeof(one));

        /* Fork off a handler */
        pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "failed to fork: %m");
            sleep(1);
            sleep(1);
            continue;
        }
        if (pid == 0) {
            /* I am that forked off child */
            closelog();
            /* Make sure that stdin/stdout are the new socket */
            dup2(msgsock, 0);
            dup2(msgsock, 1);
            /* Only parent needs lsock */
            if (lsock != 0 && lsock != 1)
                close(lsock);
#ifdef FACILITY
            openlog("ftpd", LOG_PID | LOG_NDELAY, FACILITY);
#else
            openlog("ftpd", LOG_PID);
#endif
            return;
        }

        /* I am the parent */
        close(msgsock);

        /* Quick check to see if any of the forked off children have
         * terminated. */
        while ((pid = waitpid((pid_t) -1, (int *) 0, WNOHANG)) > 0) {
            /* A child has finished */
        }
    }
}

#endif
