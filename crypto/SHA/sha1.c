#include <xinu.h>

void sha1_init  (struct hash_state* state) {

	state->state[0] = IH0;
	state->state[1] = IH1;
	state->state[2] = IH2;
	state->state[3] = IH3;
	state->state[4] = IH4;

	state->current = (byte *)(state->block);
	state->current_size = 0;
	state->block_used = 0;
}

void sha1_update(struct hash_state* state, byte* msg, uint32 length) {
	/* Algorithm variables. */
	uint32 a, b, c, d, e;

	/* Update size of the total message */
	state->current_size += length;

	while (length > 0) {
		/* Copy next 512bits chunk */
		while (length > 0 && state->block_used < 64) {
			*state->current = *msg++;
			state->current++;
			state->block_used++;
			length--;
		}

		if (state->block_used < 64)
			break;

		/* initial intermediate state */
		a = state->state[0];
		b = state->state[1];
		c = state->state[2];
		d = state->state[3];
		e = state->state[4];

		/* 4 rounds of 20 operations each. Loop unrolled. */
		R00(a,b,c,d,e,state->block, 0); R00(e,a,b,c,d,state->block, 1); R00(d,e,a,b,c,state->block, 2); R00(c,d,e,a,b,state->block, 3);
		R00(b,c,d,e,a,state->block, 4); R00(a,b,c,d,e,state->block, 5); R00(e,a,b,c,d,state->block, 6); R00(d,e,a,b,c,state->block, 7);
		R00(c,d,e,a,b,state->block, 8); R00(b,c,d,e,a,state->block, 9); R00(a,b,c,d,e,state->block,10); R00(e,a,b,c,d,state->block,11);
		R00(d,e,a,b,c,state->block,12); R00(c,d,e,a,b,state->block,13); R00(b,c,d,e,a,state->block,14); R00(a,b,c,d,e,state->block,15);
		R16(e,a,b,c,d,state->block,16); R16(d,e,a,b,c,state->block,17); R16(c,d,e,a,b,state->block,18); R16(b,c,d,e,a,state->block,19);
		R20(a,b,c,d,e,state->block,20); R20(e,a,b,c,d,state->block,21); R20(d,e,a,b,c,state->block,22); R20(c,d,e,a,b,state->block,23);
		R20(b,c,d,e,a,state->block,24); R20(a,b,c,d,e,state->block,25); R20(e,a,b,c,d,state->block,26); R20(d,e,a,b,c,state->block,27);
		R20(c,d,e,a,b,state->block,28); R20(b,c,d,e,a,state->block,29); R20(a,b,c,d,e,state->block,30); R20(e,a,b,c,d,state->block,31);
		R20(d,e,a,b,c,state->block,32); R20(c,d,e,a,b,state->block,33); R20(b,c,d,e,a,state->block,34); R20(a,b,c,d,e,state->block,35);
		R20(e,a,b,c,d,state->block,36); R20(d,e,a,b,c,state->block,37); R20(c,d,e,a,b,state->block,38); R20(b,c,d,e,a,state->block,39);
		R40(a,b,c,d,e,state->block,40); R40(e,a,b,c,d,state->block,41); R40(d,e,a,b,c,state->block,42); R40(c,d,e,a,b,state->block,43);
		R40(b,c,d,e,a,state->block,44); R40(a,b,c,d,e,state->block,45); R40(e,a,b,c,d,state->block,46); R40(d,e,a,b,c,state->block,47);
		R40(c,d,e,a,b,state->block,48); R40(b,c,d,e,a,state->block,49); R40(a,b,c,d,e,state->block,50); R40(e,a,b,c,d,state->block,51);
		R40(d,e,a,b,c,state->block,52); R40(c,d,e,a,b,state->block,53); R40(b,c,d,e,a,state->block,54); R40(a,b,c,d,e,state->block,55);
		R40(e,a,b,c,d,state->block,56); R40(d,e,a,b,c,state->block,57); R40(c,d,e,a,b,state->block,58); R40(b,c,d,e,a,state->block,59);
		R60(a,b,c,d,e,state->block,60); R60(e,a,b,c,d,state->block,61); R60(d,e,a,b,c,state->block,62); R60(c,d,e,a,b,state->block,63);
		R60(b,c,d,e,a,state->block,64); R60(a,b,c,d,e,state->block,65); R60(e,a,b,c,d,state->block,66); R60(d,e,a,b,c,state->block,67);
		R60(c,d,e,a,b,state->block,68); R60(b,c,d,e,a,state->block,69); R60(a,b,c,d,e,state->block,70); R60(e,a,b,c,d,state->block,71);
		R60(d,e,a,b,c,state->block,72); R60(c,d,e,a,b,state->block,73); R60(b,c,d,e,a,state->block,74); R60(a,b,c,d,e,state->block,75);
		R60(e,a,b,c,d,state->block,76); R60(d,e,a,b,c,state->block,77); R60(c,d,e,a,b,state->block,78); R60(b,c,d,e,a,state->block,79);

		/* update state. */
		state->state[0] += a;
		state->state[1] += b;
		state->state[2] += c;
		state->state[3] += d;
		state->state[4] += e;

		state->block_used = 0;
		state->current = (byte *)(state->block);
	}

}

void sha1_finish(struct hash_state* state, byte* diggest) {

	int32 offset_size;
	byte tmp[128];
	int32* wt;

	memset(tmp, 0, 128);

	/* add 0x80 */
	tmp[0] = 0x80;

	offset_size = 64 - state->block_used - 8;
	/* can we can add the size */
	if (offset_size <= 0) {
		offset_size += 64;
	}

	wt = (int32 *) (tmp + offset_size);
	*wt++ = big_endian(state->current_size >> 29);
	*wt = big_endian(state->current_size << 3); // in bits -> * 8

	/* should trigerred the last computation */
	sha1_update(state, tmp, offset_size + 8);

	((uint32 *)diggest)[0] = big_endian(state->state[0]);
	((uint32 *)diggest)[1] = big_endian(state->state[1]);
	((uint32 *)diggest)[2] = big_endian(state->state[2]);
	((uint32 *)diggest)[3] = big_endian(state->state[3]);
	((uint32 *)diggest)[4] = big_endian(state->state[4]);
}

void sha1_96_finish(struct hash_state* state, byte* diggest) {
	int32 i;
	byte digg[20];
	byte* wt = digg;

	sha1_finish(state, digg);

	for (i = 0 ; i < 12 ; i++)
		*diggest++ = *wt++;

	return;
}

void sha1(byte* msg, uint32 length, byte* diggest) {
	struct hash_state state;

	sha1_init(&state);
	sha1_update(&state, msg, length);
	sha1_finish(&state, diggest);

	return;
}

void sha1_96(byte* msg, uint32 length, byte* diggest) {
	struct hash_state state;

	sha1_init(&state);
	sha1_update(&state, msg, length);
	sha1_96_finish(&state, diggest);

	return;
}
