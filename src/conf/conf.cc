/*
 *   pIrcd conf reader                                             src/conf/conf.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 Mysidia <mysidia at qmud dot org> <Mysidia>
 *   see file 'AUTHORS' for a comprehensive list of pIrcd authors
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public license Version 2
 *   as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#undef CDEBUG
#undef CDEBUG2
#define __conf_cc__
#include "pircd.h"
#include "conf.h"

static void conf_in(char *, int *, int *, int, int, int);
char npath[2048] = "";
char confpath[2048] = "";
static ConfigBlock *firstblock;
static ConfigBlock *CurrentBlock;

extern ConfData *getconfbypath(char *cpath);
extern void SetRootBlock();
extern void SetConfigBlock(char *, int, char *);
extern void SetConfigData(char *, int, char *);
extern char *st_dup(char *);
extern int is_block(char *);
extern int is_data(char *);
extern char *stripln(const char *);

long gcline = 0;



#define savenpath() strcpy(npath2, npath);
#define restorenpath() strcpy(npath, npath2);

#define AssignConf(conf, l, r) do{  \
      DISPOSE(conf->variable);    \
      DISPOSE(conf->equals);      \
      conf->variable = strdup(l); \
      conf->equals = strdup(r);   \
}while(0)


extern int conf_bind(ConfData *);

struct conf_funcs {
    char *name;
    int (* func)(ConfData *);
};

struct conf_funcs conftab[] = {
        { "bind@", conf_bind },
        { "listen@", conf_bind },
        { NULL, (int (*)(ConfData *))0 }
};

void do_conf(ConfBlock *x, int y)
{
int i = 0;
ConfBlock *b, *bb;
ConfData *bd;
       if (!firstblock) return;

       if ( x )
         for ( bd = (x)->confs ; bd ; bd = bd->next)
         {
               if ( index( bd->variable, '@') )
               {
                   for ( i = 0 ; conftab[i].name; i++)
                     if (!mycmp( bd->variable, conftab[i].name ))
                          (* conftab[i].func)(bd);
               }
         }

       /* recursive part comes second so confs are read in order */
       if (!x)
       {
          for ( b = firstblock; b ; b = b->next )
            do_conf(b, 0);
          return;
       }
       else if (!y)
       {
            for ( b = x ; b; b = b->next )
            {
               if (b != x) do_conf(b, 1);
               for ( bb = b ; bb; bb = bb->blocks )
                     do_conf(bb, 1);
            }
       }
}

int conf_bind(ConfData *d)
{
      char *r = d->equals;
      char bindhost[CONFLINE+1];
      int bindport;

      if (!index( r, ':' ))
      {
          if (index(r, '.') || !isdigit(*r))
          {
               log_error("Error in conf line %d, listen@ definition must specify a port.",
                         d->line);
               return -1;
          }
          else
             sscanf(r, "%d", &bindport);
      }
      else
      {
         char *sep = index(r, ':');
          if (sep) *sep = 0;
           
           if ( !sep || 
               ( sscanf(r, "%s", bindhost) < 1 ) || (sscanf(sep+1, "%d", &bindport) < 1 ))
           {
                log_error("error parsing listen@ entry [%s] in ircd.cf, line %d", r, d->line);
                return 0;
           }
           if (sep) *sep = ':';
      }
/*      log_error("listen@ going to listen on %s, port %d", bindhost, bindport); */
      want_to_listen(bindhost, bindport);
      return 0;
}


void free_strings(char **x)
{
  int i = 0;
  for ( i = 0 ; x[i]; i++)
       free(x[i]);
}

ConfBlock *getblockbypath(char *spath)
{
int x = 0, i = 0, depth = 0;
static char path[8192] = "";
char tp[255*(MAXNEST+1)]= "", *n;
char *ar[(MAXNEST+10)];
ConfBlock *b, *b2;

       ar[0] = ar[MAXNEST] = NULL;
       if (!firstblock) return NULL;
       strcpy(path, "conf_root");
       if (*spath)
       {
         strcat(path, ".");
         strcat(path, spath);
       }

       if ( !index(path, '.') )
            ar[x++] = strdup(path);
       else
       {
       for ( i = 0; path[i]; i++)
        {
            if (path[i] != '.')
                    strncat(tp, path+i, 1);
            else
            {
               if ( (n = index(path+i+1, '.') ) ) *n = 0;
               ar[x++] = strdup(path+i+1);
               depth++;
               if (n) *n = '.';
               *tp = 0;
            }
        }
       }
         ar[x++] = NULL;
      b = firstblock;
      for ( i = 0 ; ar[i]; i++)
      {
            for ( ; b ; b = b->next )
            {
               for ( b2 = b ; b2; b2 = b2->blocks )
                if (b2->name && !stricmp( b2->name , ar[i] ))
                     if((b2->depth == (depth))  ) {goto FOUND;}
            }
      }
      free_strings(ar);
      return NULL;
FOUND:
      free_strings(ar);
      return b;
}



ConfData *getconfbypath(char *cpath)
{
char *n, *item, *blockpath;
ConfBlock *b;
ConfData  *d;

       if ( (n = strrchr( cpath, '.'  )) )
       {
                  *n = 0;
                  item = n+1;
                  blockpath = cpath;
       }
       if ( !( b = getblockbypath(blockpath) ) )
       {
                log_error("getconfbypath: getblockbypath returned: Null");
                return NULL;
       }

       if (!b||!b->confs) return NULL;

       for ( d = b->confs; d ; d = d->next)
       {
            if(d->variable && !stricmp(d->variable, item) )
                   return d;
       }
       return NULL;
}


void SetRootBlock()
{
    firstblock = (ConfBlock *)calloc(1, sizeof(ConfBlock));
    firstblock->depth = 0;
    firstblock->name = strdup("conf_root");
}

void SetConfigBlock(char *buf, int depth, char *xpath)
{
     ConfigBlock *b;
     int bNEW = 0;
     /* char tp[255*(MAXNEST+1)]= ""; */
     char path[8192] = "";


     if (!firstblock)
            SetRootBlock();

/*
     int x = 0;
       for ( i = 0; path[i]; i++)
        {
            if (path[i] != '.')
                    strncat(tp, path+i, 1);
            else
            {
               if ( (n = index(path+i+1, '.') ) ) *n = 0;
               ar[x++] = strdup(path+i+1);
               if (n) *n = '.';
               *tp = 0;
            }
        }
*/

#ifdef CDEBUG
        printf("SetConfigBlock([%s]) %s\n", path, buf);
#endif

        {   
               if (( b = getblockbypath(path) )) 
                       bNEW=0;

               if (b)
               {
                  if (b->depth > depth)
                  {
                    ConfBlock *bd, *bx;
                     if(!b->blocks)
                        b = b->blocks = (ConfBlock *)calloc(1, sizeof(ConfBlock));
                     else
                     {
                         bNEW=0;
                         bd = (ConfBlock *)calloc(1, sizeof(ConfBlock));
                         for (bx = bd->blocks; bx->next; bx = bx->next) ;
                         bx->next = bd;
                     }
                  }
                  else
                  {
                     b = (ConfBlock *)calloc(1, sizeof(ConfBlock));
                     b->next = firstblock;
                     firstblock = b;
                  }
               }
               /*else if (!firstblock) b = firstblock = calloc(1, sizeof(ConfBlock));
               else { log_error("returning without setting confblock"); return;}*/
               b->depth = (depth+1);
        }
        if (!b) return;
        b->depth = depth+1;
        b->name = strdup(buf);

}

void SetConfigData(char *buf, int depth, char *path)
{
     ConfBlock *b;
     ConfData *d;
     char left[2048] = "", right[2048], *x;
     int lst;

     if (!firstblock)
            SetRootBlock();
     
      *left = *right = 0;
      lst = strlen(buf)-1;
      if ( buf[lst] == '@' )
      {
           buf[lst] = 0;
           strncpy(left, buf, sizeof(left));
           left[sizeof(left)-1]=0;
           buf[lst] = (unsigned char)'@';
      }
      else
      if ( ( x = strchr(buf, '=') ) )
      {
          *x = 0;
          strncpy(left, buf, sizeof(left));
          strncpy(right, x+1, sizeof(right));
          left[sizeof(left)-1]=0;
          right[sizeof(right)-1]=0;
      }

      if (!( b = getblockbypath(path) )) 
      {
           log_error("SetConfigData: getblockbypath(%s) returned: Null", path);
           return;
      }
      if ( b->confs )
      {
          d = (ConfData *)calloc(1, sizeof(ConfData));
          d->next = b->confs;
          b->confs = d; 
      }      
      else b->confs = d = (ConfData *)calloc(1, sizeof(ConfData));

      if ( !d )
      {
          log_error("SetConfigData %s.%s", path, left);
          return;
      }

#ifdef CDEBUG
        printf("SetConfigData([%s.%s]) = %s \n", path, left, right);
#endif
          AssignConf(d, left, right);
          d->line = gcline;
}

char *st_dup(char *in)
{
  static char ib[4096] = "";

  if (!in) return NULL;
  strncpy(ib, in, sizeof(ib));
  ib[sizeof(ib)-1] = 0;
  return ib;   
}

/*
 *   is_block() is the standard for determining data and blocks (in conf files)
 */
int is_block(char *buf)
{
      int i;
      if(!buf) return 0;

      for ( i = 0 ; buf[i]; i++)
      {
            if (buf[i] == '=') return 0;
            else if (buf[i] == '@') return 0;
            else if (buf[i] == '$') return 0;
            else if (buf[i] == '}') return 0;
            else if (buf[i] == '{') return 1;
      }
      return 1;
}

int is_data(char *buf)
{
     return (!is_block(buf));
}

char * stripln(const char *buf)
{
    static char *buf2 = NULL;
    int i = 0, x = 0;
    int q = 0, e = 0;

    if (buf2) free(buf2);
    buf2 = (char *)malloc(strlen(buf) + 2);
    for ( i = 0; buf[i]; i++)
    {
        if (buf[i] == '\\') e = !e;
        if (buf[i] == '\"' && !e) q = !q;
         if ((e||q) || ( buf[i]!='\n'&&buf[i]!='\r'&&(!(buf[i]==' ' /*&& buf[i-1]==' '*/)) ))
            buf2[x++] = buf[i];
        if (buf[i] != '\\') e = 0;
    }
    buf2[x++] = 0;
    return buf2;
}

int read_config(const char *pConfIgFile)
{
     FILE *f;
     char buf[512] = "";
     int nested = 0, quote = 0, iBlock = 0;

     logit("(reading %s...)\n", pConfIgFile);
     if (!( f = fopen(pConfIgFile, "r") ))
     {
         log_error("Trouble reading '%s': %s", pConfIgFile, strerror(errno));
         return -1;
     }
     CurrentBlock = NULL;
     *npath = 0;
     gcline = 1;

     while ( fgets(buf, CONFLINE, f) )
     {
          gcline++;
          if (!*buf||*buf=='\n'||*buf=='#'||*buf==';'||*buf=='\r')
                   continue;
          strncpy(buf, stripln(buf), sizeof(buf));
          buf[sizeof(buf)-1] = 0;
          iBlock = is_block(buf);
          conf_in(buf, &nested, &quote, 5, iBlock, 0);
     }
     fclose(f);
     do_conf( NULL, 0 );
     return 0;
}

static void conf_in(char *buf, int *depth, int *dQuot, int adj, int iBlock, int rec)
{
    int i = 0, esc = 0;

    if (!buf || !*buf || *depth < 0 || !depth || !dQuot)
            return;

    for ( i = 0 ; buf[i]; i++)
    {

           if (buf[i] == '\\') esc = !esc;
           if (!esc && (*buf=='\"'))
                 *dQuot = !(*dQuot);
           if ( !esc && !(*dQuot) && (buf[i] == '{' || buf[i] == '}') )
           {
               if ( buf[i] == '{')
               {
                  buf[i] = 0;
                  conf_in(buf, depth, dQuot, 0, is_block(buf), rec+1);
                 if ( (*depth) >= (MAXNEST-1) )
                     log_error("Maxnest exceeded in ircd.cf");
                 else
                  (*depth)++;
                  adj = 0;
                  buf[i] = '{';
                  return;
               }
               else

               if ( buf[i] == '}')
               {
                  buf[i] = 0;
                  conf_in(buf, depth, dQuot, 0, is_block(buf), rec+1);
                 if ( (*depth) < 1 )
                     log_error("Depth went below 0 in ircd.cf");
                 else
                    (*depth)--;
                  adj = 1;
                  buf[i] = '}';
                  return;
               }

           }
           if (buf[i] != '\\') esc = 0;
    }

           if ( adj < -1 || adj > 1) adj = 0;
           if (!*buf || (*buf == ' '&& !*(buf+1)))
               return;

           (*depth) += (adj);
           if (/* *depth == 0 ||*/ iBlock)
           { /* exterior block */
//               int x = 0;
//               for (x = 0 ; x < *depth; x++) printf(" ");
//                printf("Conf block(%d): <%s>\n", *depth, buf);
#ifdef CDEBUG2
               printf("Conf block(%d): <%s>\n", *depth, npath);
#endif
               SetConfigBlock(st_dup(buf), *depth, npath);
               if (*npath)
                  strcat(npath, ".");
                  strcat(npath, buf);
           }
           else
           { /* interior data */
//               int x = 0;
//               for (x = 0 ; x < *depth; x++) printf(" ");
//                printf("Conf data(%d): <%s>\n", *depth, buf);
//               printf(" ");
               strcpy(confpath, npath);
#ifdef CDEBUG2
                printf("Conf data(%d): <%s>\n", *depth, confpath);
#endif
               SetConfigData(buf, *depth, confpath);

/*               if (*confpath)
                  strcat(confpath, ".");
                  strcat(confpath, buf);*/
           } 
           (*depth) += -(adj);

}

