#ifndef __port_h_inc
#define __port_h_inc

#ifndef HAVE_STRDUP
extern char *strdup(const char *string);
#endif

#ifndef TM_IN_SYS_TIME
#if 0
struct tm
{
	int	tm_sec;		/* seconds */
	int	tm_min;		/* minutes */
	int	tm_hour;	/* hours */
	int	tm_mday;	/* day of the month */
	int	tm_mon;		/* month */
	int	tm_year;	/* year */
	int	tm_wday;	/* day of the week */
	int	tm_yday;	/* day in the year */
	int	tm_isdst;	/* daylight saving time */
};
#endif
#endif

#endif
