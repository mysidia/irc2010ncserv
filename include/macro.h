#ifndef __macro_h_include_
#define __macro_h_include_
/*
 *      special macros
 */
#define czerosocket(x)  {if (x>=0) closesocket(x); (x) = -1;}
#define strncpyzt(x, y, z) do{strncpy(x, y, z); x[z-1]=0;}while(0)
#define strncatzt(x, y, z) do{strncat(x, y, z); x[z-1]=0;}while(0)

#define IsPerson(x)	((x)->user)

#define __IDSTRING(name,string) \
   static const char name[] __attribute__((__unused__)) = (string)
#ifndef __RCSID
#define RCSID(x) __RCSID(x)
#define __RCSID(s) __IDSTRING(rcsid,s)
#define __USEVAR(s) __IDSTRING(s, "(null)")
#endif
#define FTRUE(X) (X?X:1)
#define FPOS(X) (x<0?0:x)

#define out_of_memory()	oom()

#undef toupper
#undef tolower
#undef isdigit
#undef isalpha
extern unsigned char lowertab[], uppertab[], digit_tab[], alpha_tab[];

#define isdigit(character) (digit_tab[(unsigned char) character])
#define isalpha(character) (alpha_tab[(unsigned char) character])
#define tolower(character) (lowertab[(unsigned char)character])
#define toupper(character) (uppertab[(unsigned char)character])
#define DISPOSE(x) do{if (x)free(x);(x)=NULL;}while(0)

#endif
