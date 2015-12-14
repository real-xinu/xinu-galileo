/*
------------------------------------------------------------------------------
rand.h: definitions for a random number generator
By Bob Jenkins, 1996, Public Domain
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: renamed seed to flag
  980605: recommend RANDSIZL=4 for noncryptography.
  010626: note this is public domain
------------------------------------------------------------------------------
*/
#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

#define bis(target,mask)  ((target) |=  (mask))
#define bic(target,mask)  ((target) &= ~(mask))
#define bit(target,mask)  ((target) &   (mask))


/* context of random number generator */
struct randctx
{
  uint32 randcnt;
  uint32 randrsl[RANDSIZ];
  uint32 randmem[RANDSIZ];
  uint32 randa;
  uint32 randb;
  uint32 randc;
};
typedef  struct randctx  randctx;

/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..RANDSIZ-1] as the seed.
------------------------------------------------------------------------------
*/
void randinit(/* _randctx *r, word flag _*/);

void isaac(/*_ randctx *r _*/);

#define crand(ctx) \
  (!(ctx)->randcnt-- ? \
   (isaac(ctx), (ctx)->randcnt=RANDSIZ-1, (ctx)->randrsl[(ctx)->randcnt]) : \
   (ctx)->randrsl[(ctx)->randcnt])

/* Xinu pool random */

#define TAP1    99     /* The polynomial taps */
#define TAP2    59
#define TAP3    31
#define TAP4    9
#define TAP5    7

#define POOLWORDS   128

uint32 get_nano();
void init_randompool();
void update_randompool(uint32 input);
void update_randompool_net(uint32 ipsrc);
void update_randompool_keyboard();

/* require SHA1 implementation */
void init_context(randctx* ctx);
void get_urandom(byte* dst, int32 length);
