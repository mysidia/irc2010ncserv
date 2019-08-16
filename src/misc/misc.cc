/*
 *   pIrcd utilities                                           src/misc/misc.c
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pircd.h"
#include "h.h"

/*
 * in the future these will log to files instead of stdout
 * and only when proper defines are set
 */

void log_error (const char *fmt,...)
{
     va_list ap;
     static char buf[OFILEBUF + 1] = "";
     struct tm *g;
     time_t tNOW;
     int i = 0;

     tNOW = time (NULL);
     g = gmtime (&tNOW);
     i = strftime (buf, sizeof (buf) - 1, "[%b %d %H:%M:%S %Y] ", g);

     va_start (ap, fmt);
     vsnprintf (buf + i, (sizeof (buf) - i), fmt, ap);
     va_end (ap);

     puts (buf);
}

void logit (const char *fmt,...)
{
     va_list ap;
     static char buf[OFILEBUF + 1] = "";
     struct tm *g;
     time_t tNOW;
     int i = 0;

     tNOW = time (NULL);
     g = gmtime (&tNOW);
     i = strftime (buf, sizeof (buf) - 1, "[%b %d %H:%M:%S %Y] ", g);

     va_start (ap, fmt);
     vsnprintf (buf + i, (sizeof (buf) - i), fmt, ap);
     va_end (ap);

     puts (buf);
}

/*
**  lower loglevels for more significant things,
**  higher for more trivial
*/
 
void DebugLog (int loglevel, const char *fmt,...)
{
     va_list ap;
     static char buf[OFILEBUF + 1] = "";
     struct tm *g;
     time_t tNOW;
     int i = 0;

     tNOW = time (NULL);
     g = gmtime (&tNOW);
     i = strftime (buf, sizeof (buf) - 1, "[%b %d %H:%M:%S %Y] ", g);

     va_start (ap, fmt);
     vsnprintf (buf + i, (sizeof (buf) - i), fmt, ap);
     va_end (ap);

     puts (buf);
}



void fdprintf (int fd, const char *fmt,...)
{
     va_list ap;
     static char buf[OSOCKBUF + 1] = "";

     va_start (ap, fmt);
     vsnprintf (buf, (sizeof (buf)), fmt, ap);
     va_end (ap);
     write (fd, buf, strlen (buf));
}


/*
**     Convert ip address from the format found in hp structures to the
**   quad-octet format
*/

char *ctoip (const char *f)
{
     static char buffer[2048] = "";

     sprintf (buffer, "%d.%d.%d.%d", (unsigned char) f[0],
	      (unsigned char) f[1], (unsigned char) f[2], (unsigned char) f[3]);

     return buffer;
}


/*
**     Convert ip address from the quad-octet format to that taken by
**   gethostbyaddr()
*/

char *iptoc (const char *f)
{
     static char buffer[50];

     sscanf (f, "%d.%d.%d.%d", (int *) &buffer[0], (int *) &buffer[1],
	     (int *) &buffer[2], (int *) &buffer[3]);
     buffer[4] = 0;
     return (char *) buffer;
}

/*
 *   generic line-splitting device
 *   intended for use when read()'ing from DNS sockets
 *   methods for read()'ing irc sockets must be more highly-optimized
 */
void line_splitter (char *buffer, int (*sP) (char *))
{
     static char residue[16384] = "";
     int i = 0, bl = 16383;

     for (i = 0; buffer[i]; i++)
     {
	  if (buffer[i] == '\n' || buffer[i] == '\r' || (bl < 2))
	  {
	       bl = sizeof (residue) - 1;
	       (*sP) (residue);
	       *residue = 0;
	  }
	  else
	       strncat (residue, buffer + i, 1);
     }
}

void oom()
{
     log_error ("Server out of memory");
     exit (0);
}


/*
** match() - written by binary
**
**  We shouldn't copy very much of regular ircd's code, so I included my own,
**    which has been optimized to death.
*/

int match(char *mask, char *name)
{
  Reg1 unsigned char *m;
  Reg2 unsigned char *n;
  Reg3 unsigned char cm;
  unsigned char *wsn, *wsm;
	
  m = (u_char *)mask;
  
  cm = *m;

#define lc(x) tolower(x)

  n = (u_char *)name;  
  if (cm == '*')
  {
    if (m[1] == '\0') 			/* mask is just "*", so true */
      return 0;
  }
  else if (cm != '?' && lc(cm) != lc(*n))
  	return 1;			/* most likely first chars won't match */
  else
  {
  	m++;
  	n++;
  }
  cm = lc(*m);
  wsm = (unsigned char *)NULL;
  while (1)
  {
      if (cm == '*')			/* found the * wildcard */
      {
          m++;				/* go to next char of mask */
          if (!*m)			/* if at end of mask, */
              return 0;			/* function becomes true. */
          while (*m == '*')		/* while the char at m is "*" */
          {
    	      m++;			/* go to next char of mask */
    	      if (!*m)			/* if at end of mask, */
    	          return 0;		/* function becomes true. */
    	  }
    	  cm = *m;
    	  if (cm == '\\')		/* don't do ? checking if a \ */
    	  {
    	    cm = *(++m);		/* just skip this char, no ? checking */
    	  }
    	  else if (cm == '?')		/* if it's a ? */
    	  {
    	    do
    	    {
    	      m++;			/* go to the next char of both */
    	      n++;
    	      if (!*n)			/* if end of test string... */
    	        return (!*m ? 0 : 1);	/* true if end of mask str, else false */
    	    } while (*m == '?');	/* while we have ?'s */
    	    cm = *m;
            if (!cm)			/* last char of mask is ?, so it's true */
    	      return 0;
    	  }
    	  cm = lc(cm);
    	  while (lc(*n) != cm)
    	  {				/* compare */
    	      n++;			/* go to next char of n */
    	      if (!*n)			/* if at end of n string */
    	        return 1;		/* function becomes false. */
    	  }
    	  wsm = m;			/* mark after where wildcard found */
    	  cm = lc(*(++m));		/* go to next mask char */
    	  wsn = n;			/* mark spot first char was found */
    	  n++;				/* go to next char of n */
    	  continue;
      }
      if (cm == '?')			/* found ? wildcard */
      {
          cm = lc(*(++m));		/* just skip and go to next */
          n++;
          if (!*n)			/* return true if end of both, */
          	return (cm ? 1 : 0);	/* false if end of test str only */
          continue;
      }
      if (cm == '\\')			/* next char will not be a wildcard. */
      {					/* skip wild checking, don't continue */
     	  cm = lc(*(++m));
     	  n++;
      }
      if (lc(*n) != cm)			/* if the current chars don't equal, */
      {
          if (!wsm)			/* if there was no * wildcard, */
              return 1;			/* function becomes false. */
          n = wsn + 1;			/* start on char after the one we found last */
          m = wsm;			/* set m to the spot after the "*" */
          cm = lc(*m);
          while (cm != lc(*n))
          {				/* compare them */
              n++;			/* go to next char of n */
              if (!*n)			/* if we reached end of n string, */
                  return 1;		/* function becomes false. */
          }
          wsn = n;			/* mark spot first char was found */
      }
      if (!cm)				/* cm == cn, so if !cm, then we've */
      	return 0;			/* reached end of BOTH, so it matches */ 
      m++;				/* go to next mask char */
      n++;				/* go to next testing char */
      cm = lc(*m); 			/* pointers are slower */
  }
}

/* a #if 0 block was here.... */

char *litoa(long val)
{
      static char data[1024+200]="", *dpr, *d;
      static int i = 0;
      if (!dpr) { bzero(data, 1024+5); dpr = d = data; }
      else if ((dpr+200 >= (data+1024)))
      {
               i = 0;
               dpr = data;
               memset(data, 0, 1024);
      }
      (void)sprintf(dpr, "%ld\n", val);
      i = strlen(dpr)+1;
      d = dpr;
      dpr += i;
      return d;
}

