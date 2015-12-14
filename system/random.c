/*
 * Version 0.95, last modified 4-Nov-95
 *
 * Copyright Theodore Ts'o, 1994, 1995.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU Public License, in which case the provisions of the GPL are
 * required INSTEAD OF the above restrictions.  (This clause is
 * necessary due to a potential bad interaction between the GPL and
 * the restrictions contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <xinu.h>

static uint32 randpool[POOLWORDS];

void init_randompool() {
	int32 i, j;

	memset(randpool, 0xa5, sizeof(randpool));

	for (j = 0 ; j < 5 ; j++) {
		for (i = 0 ; i < POOLWORDS ; i++) {
			update_randompool(get_nano());
		}
		sleepms(11);
	}

}

void update_randompool(uint32 input) {

	static int32 i = POOLWORDS-1;
	static int32 rot = 2;

	uint32 w = (input << rot) | (input >> (32 - rot));
	i = (i + 1) & (POOLWORDS-1);
	rot = (rot + 14) & 31;

	/* XOR in the various taps */
	w ^= randpool[(i+TAP1)&(POOLWORDS-1)];
	w ^= randpool[(i+TAP2)&(POOLWORDS-1)];
	w ^= randpool[(i+TAP3)&(POOLWORDS-1)];
	w ^= randpool[(i+TAP4)&(POOLWORDS-1)];
	w ^= randpool[(i+TAP5)&(POOLWORDS-1)];
	w ^= randpool[i];

	/* Rotate w left 1 bit (stolen from SHA) and store */
	randpool[i] = (w << 1) | (w >> 31);
}

void update_randompool_net(uint32 ipsrc) {
	update_randompool(ipsrc);
	update_randompool(get_nano());
}

void update_randompool_keyboard() {
	update_randompool(get_nano());
}

void init_context(randctx* ctx) {
	byte diggest[SHA1_DIGGEST_SIZE];
	uint32* wt;
	struct hash_state state;
	int32 i, j;

	i = 0;
	while (i < RANDSIZL) {
		sha1_init(&state);
		sha1_update(&state, (byte *)randpool, sizeof(randpool));
		sha1_update(&state, diggest, SHA1_DIGGEST_SIZE);
		sha1_finish(&state, diggest);

		wt = (uint32 *) diggest;
		for (j = 0 ; j < SHA1_DIGGEST_SIZE && i < RANDSIZL ; i++, j += sizeof(uint32)) {
			ctx->randrsl[i] = *wt;
			update_randompool(*wt++);
		}
	}

	randinit(ctx, TRUE);
	memset(diggest, 0, SHA1_DIGGEST_SIZE);
	memset((char *)&state, 0, sizeof(struct hash_state));
}

/*
 * This function supplies an unlimited number of random bytes.
 * The randompool is used to seed a cryptographic strong
 * pseudo random generator which then supplies the stream of
 * bytes.
 */
void get_urandom(byte* dst, int32 length) {
	randctx ctx;
	byte *wtb;
	int32 i, j;

	/* add some randomness to the pool */
	update_randompool(get_nano());
	if (length <= 0)
		return;

	/* init ISAAC context with randomness from the pool */
	init_context(&ctx);

	i = 0;
	while (i < length) {
		isaac(&ctx);
		wtb = (byte *)ctx.randrsl;
		for (j = 0 ; j < RANDSIZL * sizeof(uint32) && i < length ; i++, j++) {
			*dst++ = *wtb++;
		}
	}

	memset((char *) &ctx, 0, sizeof(randctx));

	return;
}

/*
------------------------------------------------------------------------------
rand.c: By Bob Jenkins.  My random number generator, ISAAC.  Public Domain.
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: added main (ifdef'ed out), also rearranged randinit()
  010626: Note that this is public domain
------------------------------------------------------------------------------
*/

#define ind(mm,x)  (*(uint32 *)((byte *)(mm) + ((x) & ((RANDSIZ-1)<<2))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

void     isaac(ctx)
randctx *ctx;
{
   register uint32 a,b,x,y,*m,*mm,*m2,*r,*mend;
   mm=ctx->randmem; r=ctx->randrsl;
   a = ctx->randa; b = ctx->randb + (++ctx->randc);
   for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; )
   {
      rngstep( a<<13, a, b, mm, m, m2, r, x);
      rngstep( a>>6 , a, b, mm, m, m2, r, x);
      rngstep( a<<2 , a, b, mm, m, m2, r, x);
      rngstep( a>>16, a, b, mm, m, m2, r, x);
   }
   for (m2 = mm; m2<mend; )
   {
      rngstep( a<<13, a, b, mm, m, m2, r, x);
      rngstep( a>>6 , a, b, mm, m, m2, r, x);
      rngstep( a<<2 , a, b, mm, m, m2, r, x);
      rngstep( a>>16, a, b, mm, m, m2, r, x);
   }
   ctx->randb = b; ctx->randa = a;
}


#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

/* if (flag==TRUE), then use the contents of randrsl[] to initialize mm[]. */
void randinit(ctx, flag)
randctx *ctx;
int32     flag;
{
   int32 i;
   uint32 a,b,c,d,e,f,g,h;
   uint32 *m,*r;
   ctx->randa = ctx->randb = ctx->randc = 0;
   m=ctx->randmem;
   r=ctx->randrsl;
   a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

   for (i=0; i<4; ++i)          /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   if (flag)
   {
     /* initialize using the contents of r[] as the seed */
     for (i=0; i<RANDSIZ; i+=8)
     {
       a+=r[i  ]; b+=r[i+1]; c+=r[i+2]; d+=r[i+3];
       e+=r[i+4]; f+=r[i+5]; g+=r[i+6]; h+=r[i+7];
       mix(a,b,c,d,e,f,g,h);
       m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
       m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
     }
     /* do a second pass to make all of the seed affect all of m */
     for (i=0; i<RANDSIZ; i+=8)
     {
       a+=m[i  ]; b+=m[i+1]; c+=m[i+2]; d+=m[i+3];
       e+=m[i+4]; f+=m[i+5]; g+=m[i+6]; h+=m[i+7];
       mix(a,b,c,d,e,f,g,h);
       m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
       m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
     }
   }
   else
   {
     /* fill in m[] with messy stuff */
     for (i=0; i<RANDSIZ; i+=8)
     {
       mix(a,b,c,d,e,f,g,h);
       m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
       m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
     }
   }

   isaac(ctx);            /* fill in the first set of results */
   ctx->randcnt=RANDSIZ;  /* prepare to use the first set of results */
}
