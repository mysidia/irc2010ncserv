/*
 * conf.h header
 */
#include "types.h"

extern void do_conf(ConfBlock *, int);

struct configblock_
{
   char *name;
   ConfData *confs;
   ConfBlock *blocks, *next;
   int depth;
};

struct configelement_
{
   char *variable;
   char *equals;
   int ctype, depth, line;
   ConfData *next;
};

