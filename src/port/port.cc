/*
 *   pIrcd ports source file                                   src/port/port.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 Mysidia <***REMOVED***> <Mysidia>
 *   see file 'AUTHORS' for a comprehensive list of pIrcd authors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the file 'LICENSE' included with this package
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "pircd.h"
#define CONST const

#ifndef HAVE_STRDUP
char *strdup(const char *string)
{
   char *x;
   x = (char *)malloc(strlen(string) + 1);
   strcpy(x, string);
   return x;
}
#endif

/* #ifndef HAVE_STRFTIME*/

#if 0
#if 0
#include <ansidecl.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <langinfo.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define LIMAGIC(category)       (0x051472CA ^ (category))

/* Structure describing locale data in core for a category.  */
struct locale_data
  {
    const char *filedata;       /* Region mapping the file data.  */
    off_t filesize;             /* Size of the file (and the region).  */

    unsigned int nstrings;      /* Number of strings below.  */
    const char *strings[0];     /* Items, usually pointers into `filedata'.  */
  };


#define __tzname        tzname
#define __daylight      daylight
#define __timezone      timezone

#define add(n, f)                                                             \
  do                                                                          \
    {                                                                         \
      i += (n);                                                               \
      if (i >= maxsize)                                                       \
        return 0;                                                             \
      else                                                                    \
        if (p != NULL)                                                        \
          {                                                                   \
            f;                                                                \
            p += (n);                                                         \
          }                                                                   \
    } while (0)

#define cpy(n, s)       add( (n), memcpy((PTR) p, (PTR) (s), (n)) )
#define fmt(n, args)    add( (n), if (sprintf args != (n)) return 0)
/* Return the week in the year specified by TP,
   with weeks starting on STARTING_DAY.  */
#ifdef  __GNUC__
inline
#endif

static unsigned int
week(tp, starting_day)
	const struct tm *tp;
	int starting_day;
{
  int wday, dl;

  wday = tp->tm_wday - starting_day;
  if (wday < 0)
    wday += 7;
  /* Set DL to the day in the year of the last day of the week previous to the
     one containing the day specified in TP.  If DL is negative or zero, the
     day specified in TP is in the first week of the year.  Otherwise,
     calculate the number of complete weeks before our week (DL / 7) and
     add any partial week at the start of the year (DL % 7).  */
  dl = tp->tm_yday - wday;
  return dl <= 0 ? 0 : ((dl / 7) + ((dl % 7) == 0 ? 0 : 1));
}

static unsigned int
vweek ( tp )
const struct tm *tp;
{
  int vday;
  int numweeks = 0;

  /* The week starts on a Monday.  Calculate the number of days since Mon. */
  vday = tp->tm_wday - 1;
  if (vday < 0)
    vday += 7;

  /* Calculate the week number of the year.  The first calculation gives
     the number of weeks since the first week of the year. */
  numweeks = ((tp->tm_yday - vday) + 6) / 7;

  /* We now analyse the first week of the year.  If there are more than
     4 days in the week containing Jan. 1, then we can count it as 1 week.
     If not, then we consider that week to be part of the prior year */
  if (((tp->tm_yday - vday + 7)  % 7 >= 4) || ((tp->tm_yday - vday + 7)  % 7 == 0))
    numweeks += 1;

  /* The value returned must be in the range [1, 53].  If numweeks equals
     0, then it means that that week is really the last week of the previous
     year.  Hence we must return the value 53 to indicate it is the 53rd
     week of the previous year. */
  if (numweeks == 0)
    numweeks = 53;

  return numweeks;
}

/* Write information from TP into S according to the format
   string FORMAT, writing no more that MAXSIZE characters
   (including the terminating '\0') and returning number of
   characters written.  If S is NULL, nothing will be written
   anywhere, so to determine how many characters would be
   written, use NULL for S and (size_t) UINT_MAX for MAXSIZE.  */
size_t
strftime(s, maxsize, format, tp)
char *s;
size_t maxsize;
const char *format;
Reg1 const struct tm *tp;
{
#if 0
  const char *CONST a_wkday = _time_info->abbrev_wkday[tp->tm_wday];
  const char *CONST f_wkday = _time_info->full_wkday[tp->tm_wday];
  const char *CONST a_month = _time_info->abbrev_month[tp->tm_mon];
  const char *CONST f_month = _time_info->full_month[tp->tm_mon];
  size_t aw_len = strlen(a_wkday);
  size_t am_len = strlen(a_month);
  size_t wkday_len = strlen(f_wkday);
  size_t month_len = strlen(f_month);
  const char *CONST ampm = _time_info->ampm[hour12 >= 12];
  size_t ap_len = strlen(ampm);
  const unsigned int y_week0 = week(tp, 0);
  const unsigned int y_week1 = week(tp, 1);
  const unsigned int y_week4 = week(tp, 4);
  const unsigned int v_week = vweek(tp);
  const char *zone;
  size_t zonelen;
#else
#define SET_AWEEK  if (a_wkday == NULL) { \
          a_wkday = _NL_CURRENT (LC_TIME, ABDAY_1 + tp->tm_wday); \
          aw_len = strlen(a_wkday); \
        }
#define SET_AMONTH if (a_month == NULL) { \
          a_month = _NL_CURRENT (LC_TIME, ABMON_1 + tp->tm_mon); \
          am_len = strlen(a_month); \
        }
#define SET_FWEEK  if (f_wkday == NULL) { \
          f_wkday = _NL_CURRENT (LC_TIME, DAY_1 + tp->tm_wday); \
          wkday_len = strlen(f_wkday); \
        }
#define SET_FMONTH if (f_month == NULL) { \
          f_month = _NL_CURRENT (LC_TIME, MON_1 + tp->tm_mon); \
          month_len = strlen(f_month); \
        }
#define SET_AMPM if (ampm == NULL) { \
          ampm = _NL_CURRENT (LC_TIME, \
                (tp->tm_hour >= 12) ? PM_STR : AM_STR); \
          ap_len = strlen(ampm); \
        }
#define SET_Y_WEEK0 if (y_week0 == -1) { y_week0 = week(tp, 0); }
#define SET_Y_WEEK1 if (y_week1 == -1) { y_week1 = week(tp, 1); }
#define SET_Y_WEEK4 if (y_week4 == -1) { y_week4 = week(tp, 4); }
#define SET_V_WEEK if (v_week == -1) { v_week = vweek(tp); }
#define SET_ZONE if (tp->tm_isdst < 0) \
    { zone = ""; zonelen = 0; } \
  else { \
      zone = __tzname[tp->tm_isdst]; \
      zonelen = strlen(zone); \
    }
  const char *a_wkday = NULL;
  const char *f_wkday = NULL;
  const char *a_month = NULL;
  const char *f_month = NULL;
  size_t aw_len = 0;
  size_t am_len = 0;
  size_t wkday_len = 0;
  size_t month_len = 0;
  const char *ampm = NULL;
  size_t ap_len = 0;
  unsigned int y_week0 = -1;
  unsigned int y_week1 = -1;
  unsigned int y_week4 = -1;
  unsigned int v_week = -1;
  const char *zone = NULL;
  size_t zonelen = 0;
#endif
  int hour12 = tp->tm_hour;
  int dayweek = tp->tm_wday;
  register size_t i = 0;
  register char *p = s;
  register const char *f;

#if 0
  if (tp->tm_isdst < 0)
    {
      zone = "";
      zonelen = 0;
    }
  else
    {
      zone = __tzname[tp->tm_isdst];
      zonelen = strlen(zone);
    }
#endif

  if (hour12 > 12)
    hour12 -= 12;
  else
    if (hour12 == 0) hour12 = 12;

  if (dayweek < 1)
    dayweek += 7;

  for (f = format; *f != '\0'; ++f)
    {
      const char *subfmt;

      if (!isascii(*f))
        {
          /* Non-ASCII, may be a multibyte.  */
          int len = mblen(f, strlen(f));
          if (len > 0)
            {
              cpy(len, f);
              continue;
            }
        }

      if (*f != '%')
        {
          add(1, *p = *f);
          continue;
        }
      ++f;
      switch (*f)
        {
        case '\0':
        case '%':
          add(1, *p = *f);
          break;

        case 'a':
          SET_AWEEK;
          cpy(aw_len, a_wkday);
          break;

        case 'A':
          SET_FWEEK;
          cpy(wkday_len, f_wkday);
          break;

        case 'b':
        case 'h':               /* XOpen extension.  */
          SET_AMONTH;
          cpy(am_len, a_month);
          break;

        case 'B':
          SET_FMONTH;
          cpy(month_len, f_month);
          break;

        case 'c':
          subfmt = _NL_CURRENT (LC_TIME, D_T_FMT);
        subformat:
          {
            size_t len = strftime (p, maxsize - i, subfmt, tp);
            add(len, );
          }
          break;

        case 'C':               /* XOpen extension. */
          fmt (2, (p, "%.2d", (1900 + tp->tm_year) / 100));
          break;

        case 'D':               /* XOpen extension.  */
          subfmt = "%m/%d/%y";
          goto subformat;

        case 'd':
          fmt(2, (p, "%.2d", tp->tm_mday));
          break;

        case 'e':               /* XOpen extension: %d, but blank-padded.  */
          fmt(2, (p, "%2d", tp->tm_mday));
          break;

        case 'H':
          fmt(2, (p, "%.2d", tp->tm_hour));
          break;

        case 'I':
          fmt(2, (p, "%.2d", hour12));
          break;

        case 'k':               /* GNU extension.  */
          fmt(2, (p, "%2d", tp->tm_hour));
          break;

        case 'K':               /* extension for Germany, among others */
          SET_Y_WEEK4;
          fmt(2, (p, "%.2u", y_week4));
          break;

        case 'l':               /* GNU extension.  */
          fmt(2, (p, "%2d", hour12));
          break;

        case 'j':
          fmt(3, (p, "%.3d", tp->tm_yday + 1));
          break;

        case 'M':
          fmt(2, (p, "%.2d", tp->tm_min));
          break;

        case 'm':
          fmt(2, (p, "%.2d", tp->tm_mon + 1));
          break;

        case 'n':               /* XOpen extension.  */
          add (1, *p = '\n');
          break;

        case 'p':
          SET_AMPM;
          cpy(ap_len, ampm);
          break;

        case 'R':               /* XOpen extension.  */
          subfmt = "%H:%M";
          goto subformat;

        case 'r':               /* XOpen extension.  */
          subfmt = "%I:%M:%S %p";
          goto subformat;

        case 'S':
          fmt(2, (p, "%.2d", tp->tm_sec));
          break;

        case 'T':               /* XOpen extension.  */
          subfmt = "%H:%M:%S";
          goto subformat;

        case 't':               /* XOpen extension.  */
          add (1, *p = '\t');
          break;

        case 'U':
          SET_Y_WEEK0;
          fmt(2, (p, "%.2u", y_week0));
          break;

        case 'u':               /* XPG4 extension.  */
          fmt(2, (p, "%.2d", dayweek));
          break;

        case 'V':               /* XPG4 extension.  */
          SET_V_WEEK;
          fmt(2, (p, "%.2u", v_week));
          break;

        case 'W':
          SET_Y_WEEK1;
          fmt(2, (p, "%.2u", y_week1));
          break;

        case 'w':
          fmt(2, (p, "%.2d", tp->tm_wday));
          break;

        case 'X':
          subfmt = _NL_CURRENT (LC_TIME, T_FMT);
          goto subformat;

        case 'x':
          subfmt = _NL_CURRENT (LC_TIME, D_FMT);
          goto subformat;

        case 'Y':
          fmt(4, (p, "%.4d", 1900 + tp->tm_year));
          break;

        case 'y':
          fmt(2, (p, "%.2d", tp->tm_year % 100));
          break;

        case 'Z':
          SET_ZONE;
          cpy(zonelen, zone);
          break;

        default:
          /* Bad format.  */
          break;
        }
    }

  if (p != NULL)
    *p = '\0';
  return i;
}

#endif
#endif
