/*
** flags.h - defines flags
*/

#ifndef __flags_h_include__
#define	__flags_h_include__

#include <values.h>

#define	BIT01	0x00000001
#define	BIT02	0x00000002
#define	BIT03	0x00000004
#define	BIT04	0x00000008
#define	BIT05	0x00000010
#define	BIT06	0x00000020
#define	BIT07	0x00000040
#define	BIT08	0x00000080
#define	BIT09	0x00000100
#define	BIT10	0x00000200
#define	BIT11	0x00000400
#define	BIT12	0x00000800
#define	BIT13	0x00001000
#define	BIT14	0x00002000
#define	BIT15	0x00004000
#define	BIT16	0x00008000
#define	BIT17	0x00010000
#define	BIT18	0x00020000
#define	BIT19	0x00040000
#define	BIT20	0x00080000
#define	BIT21	0x00100000
#define	BIT22	0x00200000
#define	BIT23	0x00400000
#define	BIT24	0x00800000
#define	BIT25	0x01000000
#define	BIT26	0x02000000
#define	BIT27	0x04000000
#define	BIT28	0x08000000
#define	BIT29	0x10000000
#define	BIT30	0x20000000
#define	BIT31	0x40000000
#define	BIT32	0x80000000
#define BIT(x)	(1 << (x-1))

#define	M_NEEDPARAM_ADD	BIT01
#define	M_NEEDPARAM_DEL	BIT02
#define	M_NEEDPARAM	(BIT01|BIT02)
#define bitvector_t	basic_bitvector

/////////////////////////////////////////////////////////////////////////
// Bitvector class                                                     //
/////////////////////////////////////////////////////////////////////////

class basic_bitvector;

class LocalBitSet {
   friend class basic_bitvector;
   unsigned long int myValue;
};

class basic_bitvector {
public:
   bitvector_t(unsigned long int x, unsigned int num);
   bitvector_t(unsigned long int x) { myval(x, INTBITS);}
   bitvector_t(int x) { myval((unsigned long int)x, INTBITS);}
   bitvector_t() { myval(0, INTBITS);}
   ~bitvector_t() { delete theVal; }

   u_int HowManySlotsNeeded(), WhatSlot(u_int x);
   inline unsigned long int & WhatMask(u_int x);

   inline bool valid_bit(u_int bit);
   inline bool is_set(u_int bit);
   inline bool all_set(u_int bit);
   inline bool only_set(u_int bit);
   inline void set_bit(u_int bit);
   inline void and_bit(u_int bit);
   inline void clr_bit(u_int bit);
   inline void tog_bit(u_int bit);
   inline void set_bit(basic_bitvector);
   inline void clr_bit(basic_bitvector);
   inline void tog_bit(basic_bitvector);
   inline void clr_bit();
   inline bool is_zero();

   // Here we completely hide the implementation of bitvectors via interface
   // code, by letting code treat them as ordinary int flags -- this won't
   // be important here, but it sure will be very important for later.
   // it also means we can change how bvs work without missiling the code
   // too much                                                 -Mysidia

   unsigned long operator=(unsigned long int x) { return myval = x; }
   unsigned long operator=(int x) { return myval = (x > INT_MAX ? INT_MAX : x); }
   unsigned long operator=(long x) { return myval = (x > LONG_MAX ? LONG_MAX : x); }
   // This is for parsing a string bitvector for extended bitvectors,
   // this will be a more complex, but for now we don't need anything
   // special. -Mysidia

   void operator=(const char *str) {
       if (!str) return;
       if (*str == '0' && *(str+1) == 'x')
           myval = strtol(str, (char **)0, 16);
       else
           myval = strtol(str, (char **)0, 10);
       return;
   }

   // This uses a static cyclic buffer to print the 'word'
   // to write to disk to represent the flags. -Mysid
   const char *bv_str() {
       static char bv_buf[256+BV_BUF] = "";
       static size_t start = 0;
       size_t pos;

       if (start >= (256 - BV_BUF))
           start = 0;
       sprintf(bv_buf + start, "%lu", myval);
       pos = start;
       start += BV_BUF;
       return bv_buf + pos;
   }

   bool operator==(bitvector_t bv2) { return myval == bv2.myval; }
   bool operator==(unsigned long int x) { return myval == x; }

   operator unsigned long int () { return myval; }

   unsigned long operator&(unsigned long int x) const { return myval & x; }
   unsigned long operator^(unsigned long int x) const { return myval ^ x; }

   unsigned long operator|=(unsigned long int x) { return myval |= x; }
   unsigned long operator&=(unsigned long int x) { return myval &= x; }
   unsigned long operator^=(unsigned long int x) { return myval ^= x; }

   unsigned long operator&(const bitvector_t bv2) const { return myval & bv2.myval; }
   unsigned long operator^(const bitvector_t bv2) const { return myval ^ bv2.myval; }
   unsigned long operator~() const { return ~myval; }

   unsigned long operator|=(const bitvector_t bv2) { return myval |= bv2.myval; }
   unsigned long operator&=(const bitvector_t bv2) { return myval &= bv2.myval; }
   unsigned long operator^=(const bitvector_t bv2) { return myval ^= bv2.myval; }
private:
   const static unsigned int BV_BUF = 15;
   LocalBitSet			*theVal;
   unsigned int			theNumBits;
};

/////////////////////////////////////////////////////////////////////////
// Bitvector implementation                                            //
/////////////////////////////////////////////////////////////////////////

basic_bitvector::bitvector_t(unsigned long int x, unsigned int num) :
   theNumBits(num), theVal(NULL) 
{
   int i = 0, slots = HowManySlotsNeeded();

   theVal = new LocalBitSet [HowManySlotsNeeded()];
   for(i = 0; i < slots; i++)
       theVal[i] = 0;
}

u_int basic_bitvector::HowManySlotsNeeded() {
    return (theNumBits < INTBITS) ? 1 : (1 + (theNumBits / INTBITS)); 
}

u_int basic_bitvector::WhatSlot(u_int x) {
    return (x < INTBITS) ? 0 : (x / INTBITS); 
}

inline unsigned long int & basic_bitvector::WhatMask(u_int x) {
    return (x < INTBITS) ? theVal[0].myValue : theVal[(x / INTBITS)].myValue; 
}

inline bool basic_bitvector::valid_bit(u_int bit) {
    if ((unsigned int)(bit) >= theNumBits || bit < 0) return 0; return 1; 
}

inline bool basic_bitvector::is_set(u_int bit) {
    if (!valid_bit(bit))
        return 0;
    return (WhatMask(bit) & (1 << (bit))) ? true : false;
}

inline bool basic_bitvector::all_set(u_int bit) {
//    if (!valid_bit(bit)) return 0;
//     return ((myval & (1 << (bit))) == (unsigned int)((1 << (bit)))) ? true : false;
    return 0;
}

bool basic_bitvector::only_set(u_int bit) {
//     if (!valid_bit(bit)) return 0;
//     return ((myval & (1 << (bit))) == (myval)) ? true : false;
       return false;
}

inline void basic_bitvector::set_bit(u_int bit) {
    if (!valid_bit(bit)) return;
    WhatMask(bit) |= (1 << WhatOffSet(bit));
}

inline void basic_bitvector::and_bit(u_int bit) {
    if (!valid_bit(bit)) return; 
    WhatMask(bit) &= (1 << WhatOffset(bit));
}

inline void basic_bitvector::clr_bit(u_int bit) {
    if (!valid_bit(bit)) return;
    WhatMask(bit) &= ~(1 << WhatOffset(bit));
}

inline void basic_bitvector::tog_bit(u_int bit) {
    if (!valid_bit(bit)) return;
    WhatMask(bit) ^= (1 << WhatOffset(bit));
}

inline void basic_bitvector::set_bit(bitvector_t bv2) {
    myval |= bv2.get_bits();
}

inline void basic_bitvector::clr_bit(bitvector_t bv2) {
    myval &= ~(bv2.get_bits());
// xxx
}

inline void basic_bitvector::clr_bit()
{
    myval  = 0; 
}

void basic_bitvector::tog_bit(bitvector_t bv2) 
{
    myval ^=  (bv2.get_bits()); 
}

bool basic_bitvector::is_zero() 
{
    return myval ? false : true; 
}


class flag_t : public basic_bitvector {
};

/////////////////////////////////////////////////////////////////////////

struct flag_list
	{
		flag_t flag;
		char modechar;
 		char needparam;
		char *name;          /* for purpose of flag functions */
		short xbit;	     /* extra flags regarding how the flag is set */
	};

#define FLFLAG_NOUNSET	BIT01	     /* flag cannot be unset by user */
#define FLFLAG_NOSET	BIT02	     /* flag cannot be set by user */
#define FLFLAG_INTERNAL	BIT03	     /* flag is used elsewhere not by flag routines */

#define FLAGS_DELETED	BIT01	     /* client deleted, free when inuse drops to 0 */

#define	UMODE_OPER	BIT01
#define	UMODE_LOCOP	BIT02
#define	UMODE_WALLOPS	BIT03
#define	UMODE_INVISIBLE	BIT04
#define	UMODE_GLOBOPS	BIT05
#define	UMODE_HELPOP	BIT06
#define	UMODE_DISGUISE	BIT07

#define FLAGS_INVALID	BIT01

#ifndef DECL_FLISTS
extern struct flag_list umode_list[];
#else
struct flag_list umode_list[] =
	{
		{	UMODE_OPER,		'o', 0,		"oper" },
		{	UMODE_LOCOP,		'O', 0,		"locop" },
		{	UMODE_WALLOPS,		'w', 0,		"wallops" },
		{	UMODE_INVISIBLE,	'i', 0,		"invis" },
		{	UMODE_GLOBOPS,		'g', 0,		"globops" },
		{	UMODE_HELPOP,		'h', 0,		"helpop" },
		{	UMODE_DISGUISE,		'd', 0,		"disguise" },
		{	0,			 0 , 0,		(char *)NULL }
	};
#endif

#define	CMODE_NOPRIVMSGS	BIT01
#define	CMODE_TOPICLIMIT	BIT02
#define	CMODE_SECRET		BIT03
#define	CMODE_PRIVATE		BIT04
#define	CMODE_INVITEONLY	BIT05
#define	CMODE_MODERATED		BIT06
#define	CMODE_LIMIT		BIT07
#define	CMODE_KEY		BIT08
#define	CMODE_BAN		BIT09
#define	CMODE_OP		BIT10
#define	CMODE_VOICE		BIT11

#ifndef DECL_FLISTS
extern struct flag_list cmode_list[];
#else
struct flag_list cmode_list[] =
	{
		{	CMODE_NOPRIVMSGS,	'n', 0,			"noext"},
		{	CMODE_TOPICLIMIT,	't', 0,			"topiclock" },
		{	CMODE_SECRET,		's', 0,			"secret" },
		{	CMODE_PRIVATE,		'p', 0,			"private" },
		{	CMODE_INVITEONLY,	'i', 0,			"inviteonly" },
		{	CMODE_MODERATED,	'm', 0,			"moderated" },
		{	CMODE_LIMIT,		'l', M_NEEDPARAM_ADD,	"userlimit" },
		{	CMODE_KEY,		'k', M_NEEDPARAM,	"key" },
		{	CMODE_BAN,		'b', M_NEEDPARAM,	"ban" },
		{	CMODE_OP,		'o', M_NEEDPARAM,	"op" },
		{	CMODE_VOICE,		'v', M_NEEDPARAM,	"voice" },
		{	0,			 0 , 0,			(char *)0 }
	};
#endif
#define	CHFL_OP		BIT01
#define	CHFL_VOICE	BIT02
#define	CHFL_BANNED	BIT04 /* no need for constant lookups */
#define	CHFL_NOTBANNED	BIT05

#endif /* _FLAGS_INCLUDE_ */
