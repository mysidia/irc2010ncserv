/*
 *   pIrcd Options configuration script (ATTIC)         conf.c
 *   Copyright C 1998  Mysidia <***REMOVED***> 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the file 'LICENSE' included with this package
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define  __options_h_include__
#include "include/sys.h"
#undef strcasecmp
#undef strncasecmp
#undef strcmp
#undef strncmp

	/* I used these earlier to track down a buffer overflow... */
const char one[] = "1";
const char zero[] = "0";

char *get_choice(const char *, const char *, int eflag, const char *);
char *write_it(int fd, char *text);
char *fw(int fd, char *text, ...);
void makefilestr(char *var, char *value, int quote);
void s_int();
int fef = 0;
char *cflags, *pwd;

#define FLAGS_BOOL    0x1
#define FLAGS_NUMERIC 0x2
#define FLAGS_ALPHANUM 0x4
#define FLAGS_NULL 0x8

int as = 0;
int opts_done = 0;

#define pf printf
#define pu puts
#define gc(x) (void)get_choice(x,NULL, 0, NULL)
#define go(x,y,z, a) get_choice(x,y, z, a)
#define fatal(x,y) {perror(x); fprintf(stderr, "%s\n", (y)); exit (-1);}
clear_screen()
{
puts("\33[H\33[J");
}

 /* simple: grab the option from the environment and use that
            as the default value. */
#define getopt(name)	(doit(name))


int nopts = 0;
char **opts;
char **inf;
char *this;

char *boolstr(const char *bval)
{
    if (!bval) return("No");
    if (atol(bval)) return("Yes");
    else return("No");
}

char *doit(char *name)
{
  char *macro_foo, *macro_foo2=NULL;				
  int  macro_ind;						
	this = NULL;
								
  macro_foo=macro_foo2=NULL;					
  for (macro_ind=0;macro_ind<nopts;macro_ind++)			
  {								
	if (inf[macro_ind] && (name))				
	if (strcasecmp(inf[macro_ind], (name)) == 0)		
	{
	  macro_foo2 = opts[macro_ind];				
           break;						
	}							
  }								
  								
  macro_foo = getenv((name));					
  if (macro_foo && !macro_foo2)					
  {								
     nopts++;							
     opts = realloc(opts, sizeof(char *) * (nopts+2));	
     inf  = realloc(inf , sizeof(char *) * (nopts+2));	
     if (!inf||!opts) { perror("memory_alloc_error"); exit -1; }	
     opts[nopts-1] = this = strdup(macro_foo);		
     inf[nopts-1] = strdup((name));      			
	//this = strdup((name));					
  } else if (macro_foo2) {					
	this = macro_foo2;					
  }								
	return (this);
}

#if 0
void deldf(char **d1, char **d2) {return;}
#else
void deldf(char **d1, char **d2)
{
	if (d1 && *d1 && *d1!=zero && *d1!=one)
	{
		free(*d1);
		*d1=NULL;
	}
	if (d2 && *d2 && *d2!=zero && *d2!=one)
	{
		free(*d2);
		*d2=NULL;
	}
	return; 
}
#endif

void setdf(char **d1, char **d2)
{
     char *v1;
     char *nd1 = NULL, *nd2 = NULL;

     if (d1 && *d1)
     {
        v1 = index(*d1, ' ');
        if (v1)  *v1 = '\0';
        nd1 = calloc(1, sizeof(char)*(strlen(*d1)+1));
        if (nd1)
	{
		 strcpy(nd1, *d1);
                *d1 = nd1;
	}
	else return;
        if (v1)  *v1 = ' ';
     } else *d1=NULL;

     if (d2 && *d2)
     {
        if (**d2 == '\"')
        {
           (*d2) += 1;
           v1 = index((*d2) , '\"');
        } else v1=NULL;
        if (v1) *v1='\0';
        nd2 = calloc(1, (sizeof(char)*strlen(*d2)+1));
        if (nd2)
	{
		 strcpy(nd2, *d2);
		 *d2 = nd2;
	}
        if (v1) *v1='\"';
     } else *d2 = NULL;
        return;
}

int rules = 0; /* options.h.dist rules enabled ? */
int rule_max = 0; /* maximum int val */
int rule_min = 0; /* minimum int val */

main(int argc, char **argv)
{
char elist[1024]="";
char *t, *fido;
FILE *f, *f2;
int fd = 0, avs = 0, sval = 0, fd2 = 0, i = 0;
char fin[2048]="", cstr[2048]="";
char *dn; /* define name*/
char *def; /* default */
time_t curv = time(NULL);

#ifdef  POSIX_SIGNALS
        struct  sigaction act;
#endif


	(void)sprintf(cstr, "%s", ctime(&curv));
	cstr[strlen(cstr)-1] = '\0';

	if (argc < 4) 
	{
		printf("only %ld args...\n", argc);
		exit(-1);
	}

#ifdef POSIX_SIGNALS
        act.sa_handler = s_int;
        act.sa_flags = 0;
        (void)sigemptyset(&act.sa_mask);
        (void)sigaddset(&act.sa_mask, SIGINT);
        (void)sigaction(SIGUSR1, &act, NULL);
#else
signal(SIGINT, s_int);
#endif

cflags = argv[1];
pwd = argv[2];
avs = atol(getenv("MENU")?getenv("MENU"):"0");
if (atol(argv[3]) == 0) as = 0; else as = 1;

/* here's where it all starts */
opts_done = -1;
(void)rename("Options", "Options.tmp");

f = fopen("include/options.h", "w");
f2 = fopen("Options", "a");
	if (!f || !(fef = fd = fileno(f)) || !f2
            || !(fd2 = fileno(f2)) )
	{
	             pu(" A fatal Error Occured, I was unable to open ");
        	     pu("      the configuration file for output \n");
		     perror("open");
		     exit -1;
	}

	if ( !getopt("IRCD_VERSION") ) exit(-1);
fprintf(f2, 	""
		"#\n"
		"# VERSION: %s\n"
		"# DATE:    %s\n"
		"#\n"
		"# This file is automatically generated and will be updated each time you\n"
		"# run Config.  You should retain a copy of this to help migrate to future\n"
		"# versions of the SorceryNet server with ease.\n"
		"#\n"
		"export LAST_VERSION=\"%s\"\n", getopt("IRCD_VERSION"),
		 	cstr, getopt("IRCD_VERSION"));
	fflush(f2);

	printf("\n=== Welcome to the SorceryNet Server Configuration Program ===\n");
	printf("\nIRCD VERSION: %s\n", getopt("IRCD_VERSION"));
	printf("Dist: %s\n", getopt("RELEASE_TYPE"));
	printf("\n\n");
	if (as) as=2;	
	gc("[Press Enter to Continue]");

	if (!avs)
	{
		char rbuf[4096]="";
		FILE *fp = fopen("include/options.h.dist", "r");
		if (!fp)
		{
			pu("ERROR opening file:" "include/options.h.dist");
			exit -1;
		}
		while (fgets(rbuf, 2048, fp))
		{
			dn = def = NULL;

                        if (rbuf[0] == '\n' || rbuf[0] == '\r')
			{
					write_it(fd, "\n");
					continue;
			}

			if (rbuf[0] != '\0') rbuf[strlen(rbuf)-1]='\0';
			if (rbuf[0] != '#')
			{      /* clip double spaces */
				int sv = 0;
				char rbuf2[4096]="";
				int ipos=0, ilen=0;

				ilen = strlen(rbuf);
				for (ipos=0;ipos<ilen;ipos++)
					if (
						(rbuf[ipos]!=' '||
                                                 rbuf[ipos+1]!=' '))
                                         rbuf2[sv++] = rbuf[ipos];
					 rbuf2[sv++] = '\0';
					 strcpy(rbuf, rbuf2);
			}

			if (rbuf[0] == '#') 
			{
				fw(fd, "%s\n", rbuf);
				continue;
			}

			if (rbuf[0] == '=') 
			{
				fw(fd, " // %s\n", rbuf+1);
			}

			if (rbuf[0] == '#') continue;
			else
			{
				if (rbuf[0] == '=') printf("%s\n", rbuf+1);

				if (rbuf[0] == '!')
				{
				    if (strncasecmp(rbuf+1, "#define", 7)==0
				        && rbuf[7] && rbuf[8])
				    {
				      dn = rbuf+8+1;
				      def=NULL;
				      setdf(&dn, &def);
#if 0 /* boolify'ing these seems stupid */
				      def = (char *) one;
#endif
				    } else
				    if (strncasecmp(rbuf+1, "#undef", 6)==0
				        && rbuf[6] && rbuf[7])
				    {
				      dn = rbuf+7+1;
				      def=NULL;
				      setdf(&dn, &def);
#if 0
				      def="0";
#endif
				    }
					else
					{
					   dn = NULL;
					   write_it(fd, rbuf+1);
					   write_it(fd, "\n");
					}
				   if (dn)
				   {
					fw(fd, "%s\n", rbuf+1);
					fw(fd2, "export %s=\"%s\"\n", dn, def);
				   }

				}
				if (rbuf[0] == '@')
				{
				 def = dn = NULL;
				 if (strncasecmp(rbuf+1, "#define", 7)==0
				     && rbuf[7] && rbuf[8])
				 {
				   dn = rbuf+8+1;
				   def = NULL;
				   setdf(&dn, &def);
				   def = (char *) one;
				 }
				 if (strncasecmp(rbuf+1, "#undef", 6)==0
				     && rbuf[6] && rbuf[7])
				 {
				   dn = rbuf+7+1;
				   def = NULL;
				   setdf(&dn, &def);
				   def = (char *)zero;
				 }

				 fido = go(NULL, 
					   strcasecmp(def, one)==0
				           ?"Yes":"No", FLAGS_BOOL, getopt(dn)?getopt(dn):NULL );

				 if (fido && atol(fido))
				{
					 fw(fd, "#define %s 1\n", dn);
					 fw (fd2, "export %s=\"1\"\n", dn);

				}
				 else
				{
					 fw (fd2, "export %s=\"0\"\n", dn);
					 fw(fd, "#undef %s\n", dn);
				}

					deldf(&dn, &def);
				}



				if (rbuf[0] == '&' && rbuf[1])
				{
					char *foopr = index(rbuf+1, ' ');
					rbuf[0] = '\0';
					if (foopr) *foopr='\0';
					if (foopr)
					{
						if (strcasecmp(rbuf+1, "MAX")==0)
						{
						 rule_max = atol(foopr+1);
						 rules = 1;
						}
						else if (strcasecmp(rbuf+1, "MIN")==0)
						{
						 rule_min = atol(foopr+1);
						 rules = 1;
						}
					}
					if (strcasecmp(rbuf+1, "CLEAR")==0)
					{
						rules = 0;
					}
					rbuf[0] = '\0';
					continue;
				}

			/* handle numerical #defines */
				if (rbuf[0] == '+'||rbuf[0]=='^'||rbuf[0]=='%')
				{
				 def = dn = NULL;
				 if (strncasecmp(rbuf+1, "#define", 7)==0
				     && rbuf[7] && rbuf[8])
				 {
				   dn = rbuf+8+1;
				   def = index(dn , ' ')?index(dn, ' ')+1:NULL;
				   setdf(&dn, &def);
				 }
				 if (strncasecmp(rbuf+1, "#undef", 6)==0
				     && rbuf[6] && rbuf[7])
				 {
				   dn = rbuf+7+1;
				   def = index(dn , ' ')?index(dn, ' ')+1:NULL;
				   setdf(&dn, &def);
				 }

				redo_fido_num:
				 fido = go(NULL, def, FLAGS_NUMERIC, getopt(dn) );
				if (fido && atol(fido)<=0 && rbuf[0]=='%')
				{
					printf("must supply non-zero value\n");
					goto redo_fido_num;
				}

				if (rules && fido && rule_min && atol(fido)<rule_min)
				{
					printf("Value must be at least %ld\n", rule_min);
					goto redo_fido_num;
				}

				if (rules && fido && rule_min && atol(fido)>rule_max)
				{
					printf("Value can be no more than %ld\n", rule_max);
					goto redo_fido_num;
				}

				 if (atol(fido) || fido[0] == '+')
				{
					 fw(fd, "#define %s %s\n", dn, fido);
					 fw (fd2, "export %s=\"%s\"\n", dn, fido);

				}
				 else
				{
					 fw (fd2, "export %s=\"0\"\n", dn);
					 fw(fd, "#undef %s\n", dn);
				}

				   deldf(&dn, &def);
				}

			/* handle string defines */
				if (rbuf[0] == 'S'||rbuf[0]=='s'||toupper(rbuf[0])=='R')
				{
				 def = dn = NULL;
				 if (strncasecmp(rbuf+1, "#define", 7)==0
				     && rbuf[7] && rbuf[8])
				 {
				   dn = rbuf+8+1;
				   def = index(dn , ' ')?index(dn, ' ')+1:NULL;
				   setdf(&dn, &def);
				 }
				 if (strncasecmp(rbuf+1, "#undef", 6)==0
				     && rbuf[6] && rbuf[7])
				 {
				   dn = rbuf+7+1;
				   def = index(dn , ' ')?index(dn, ' ')+1:NULL;
				   setdf(&dn, &def);
				 }

				 fido = go(NULL, def, FLAGS_ALPHANUM|(rbuf[0]=='s'?FLAGS_NULL:0), getopt(dn) );

				 if (fido)
				 {
					 fw(fd, "#define %s \"%s\"\n", dn, fido);
					 fw (fd2, "export %s=\"%s\"\n", dn, fido);
				 }
				 else
				{
					 fw(fd, "#undef %s\n", dn);
					 fw (fd2, "export %s=\"\"\n", dn);
				}

				   if (toupper(rbuf[0])=='R')
				   { /* now, if it says to put it in Makefile,
					do that*/
				      if (!fido)
					makefilestr(dn, "", rbuf[0]=='R'?1:0);
				       else
					makefilestr(dn, fido, rbuf[0]=='R'?1:0);
				   }

				   deldf(&dn, &def);
				}
			}


		}
fclose(fp);
	}
     gc("============= Press Enter to Continue ==========");
     pu(" ");
fclose(f);

assert(!strcasecmp(zero, "0"));
assert(!strcasecmp(one, "1"));
#if 0
if (f = fopen("include/options.h.dist", "r"))
{
char *v, *vv, *vvv;
char rbuf[2048]="";

i=0;
	while(fgets(rbuf, 1024, f))
	{
		rbuf[0] = toupper(rbuf[0]);
		if (rbuf[0] != '@' && rbuf[0] != '!' && rbuf[0] != '+' &&
		    rbuf[0] != '^' && rbuf[0] != 'S' && rbuf[0] != '~' &&
		    rbuf[0] != '%') continue;
		rbuf[strlen(rbuf)-1]='\0';
		v = rbuf+1;
		if (strncasecmp(v, "#define ", 8)==0 ||
		    strncasecmp(v, "#undef ", 7)==0)
		{
		    if (v[1] == 'd') vv = rbuf+9;
		    else	     vv = rbuf+8;
		    if (*vv)
		    {
                       vvv = index(vv, ' ');
                       if (vvv) *vvv = '\0';
                       vvv = index(vv, '\t');
                       if (vvv) *vvv = '\0';
		       i++;			
		       if (i%4==0 && elist[0])
		       {
		         fw(fd2, "export %s\n", elist);
		         elist[0] = '\0';
		       } else
		       { 
		         if (i!=0&&(i-1)%4!=0) strcat(elist, " ");
		         strcat(elist, vv);
		       }
		    }
		}
	}
fclose(f);
}
	if (elist[0])      fw(fd2, "export %s\n", elist);
#endif

fclose(f2);

		/* clean up */
		opts_done = 1;
		(void)remove("Options.tmp");
}



char *get_choice(const char *prompt, const char *d, int eflag, const char *co)
{
static char re[1024]="";
int i = 0, l = 0;

restart:
re[0] = '\0';

if (co) /* replace default with co if available */
{
        d = co;
}

if (!d) {if (prompt) pf ("%s ",prompt);}

if (!prompt && d){
	if (strcasecmp(d, "0")==0 && eflag & FLAGS_BOOL)
	 pf ("[No] ->");
	else if (strcasecmp(d, one)==0 && eflag & FLAGS_BOOL)
	 pf ("[Yes] ->");
	else
	 pf ("[%s] ->",co?( (eflag&FLAGS_BOOL)?boolstr(co):co):d); 
}

if (!co || !as || (as==2))
	gets(&re[0]);
else	{
		eflag = 0;
		strcpy(re, co);
		printf("\n");
	}
 if (as==2) as = 1;
 if (re[0]=='\0'||re[0]=='\n'||re[0]=='\r')
 {
   if (d) {strcpy(re, d); } else { re[0]='\0'; return(re);} 
 }

if (eflag & FLAGS_NULL)
{
    if (strcasecmp("none", re)==0)
	return(NULL);
}

if (eflag & FLAGS_ALPHANUM)
{
 l = strlen(re);
 for (i=0;i<l;i++)
     if (!isdigit(re[i]) && !isalpha(re[i]) && !ispunct(re[i]))
     {
	   pu("    === Incorrect input ===  ");
	   pu(" a Only Alpha-Numeric Characters may be used");
	    goto restart;
     }
}

/* numeric expressions */
if (eflag & FLAGS_NUMERIC)
{
 l = strlen(re);
 for (i=0;i<l;i++)
     if (!isdigit(re[i]))
     {
   pu("    === Incorrect input ===  ");
   pu(" a NUMERIC value is expected");
       goto restart;
     }
}
/* for bool expressions */
if (eflag & FLAGS_BOOL)
{
l = strlen(re);
for (i=0;i<l;i++) re[i]=tolower(re[i]);
if (strcmp(re, "yes") == 0) strcpy(re, one);
if (strcmp(re, "no") == 0) strcpy(re, zero);
if (strcmp(re, "y") == 0) strcpy(re, one);
if (strcmp(re, "n") == 0) strcpy(re, zero);
if (strcmp(re, zero) && strcmp(re, one))
 {
   pu("    === Incorrect input ===  ");
   pu(" a Yes or No Response is expected");
   goto restart;
 }
}
return(&re[0]);
}

char *write_it(int fd, char *text)
{
  write(fd, text, strlen(text));
}

char *fw(int fd, char *fmt, ...)
{
  va_list args;
  char buf[4096]="";
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  write(fd, buf, strlen(buf));
}


void s_int(int sno)
{
   close(fef);
  if (opts_done == -1) /* undo it if we didnt finish... */
  {
   (void)remove("include/options.h");
   (void)rename("Options.tmp", "Options");
 }
   exit(0);
}

void makefilestr(char *var, char *value, int quote)
{
FILE *makefile;
FILE *tmpfile;
char ind[1024+512]="";
char v1[1024+2]="";

	strncpy(v1, var, 1022);
	v1[1022] = '\0';

	if (!(makefile = fopen("Makefile", "r")))
	{
		fatal("open", "while opening Makefile for read access");
	}
	if (!(tmpfile = fopen("Makefile.tmp", "w")))
	{
		fatal("open", "while opening Makefile.tmp for write access");
	} else fchmod(fileno(tmpfile), S_IRUSR|S_IWUSR);

	while (fgets(ind, 1024, makefile))
	{
		ind[strlen(ind)-1] = '\0';
		if (strncasecmp(ind, v1, strlen(v1)) == 0)
		{
		   if (quote)
		     fprintf(tmpfile, "%s = \"%s\"\n", v1, value);
		   else
		     fprintf(tmpfile, "%s = %s\n", v1, value);
		}
		else fprintf(tmpfile, "%s\n", ind);
	}
fclose(tmpfile);
fclose(makefile);
(void)rename("Makefile.tmp", "Makefile");
return;
}

