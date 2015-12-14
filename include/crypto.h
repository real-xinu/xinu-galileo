#ifndef CRYPTO_H
#define CRYPTO_H

#include <mpint.h>

/* tests. */
void  launch_cryptotests();

#define CRYPTO_DEBUG
#define CRYPTO_TEST

#define CIPH_ID(cipher, mode) ((mode) << 16 | (cipher))
#define CIPHER(x) ((x) & 0xFFFF)
#define MODE(x) ((x) >> 16)

#define KEX_ID(group, hash) ((group) << 16 | (hash))
#define HASH(x) ((x) & 0xFFFF)
#define GROUP(x) ((x) >> 16)

/* global definition UPDATE everytime a new algorithm is added!! */
#define MAX_HASH_DIGGEST	20
#define MAX_HASH_BLOCK		64
#define MAX_CIPHER_BLOCK	8
#define MAX_KEYS_SIZE		24

/* Definitions. */
#define NONE 0
#define CHAR_BIT 8
#define rol(value,shift) ((value) << (shift) | ((value) >> (sizeof(value) * CHAR_BIT - (shift))))

/* change endianess */
#define big_endian(x) ((rol(x, 24) & 0xFF00FF00) \
		| (rol(x,8) & 0x00FF00FF))

/******************/
/* Hash functions */
/******************/

/* HMAC */
struct hash_state {
	uint32  block[MAX_HASH_BLOCK/4];
	byte*   current;
	int32   block_used;
	int32   current_size;
	uint32  state[MAX_HASH_BLOCK/4];
};

typedef void (* INIT_DIGGEST) (struct hash_state* state);
typedef void (* UPDATE_DIGGEST) (struct hash_state* state, byte* msg, uint32 length);
typedef void (* FINISH_DIGGEST) (struct hash_state* state, byte* diggest);

#define IPAD 0x36
#define OPAD 0x5C

typedef int32 hid32;
struct hashent {
	INIT_DIGGEST      init;
	UPDATE_DIGGEST    update;
	FINISH_DIGGEST    finish;
	uint32            block_size;
	uint32            diggest_size;
	uint32            key_size;
	char*             name;
};

extern struct hashent hashtab[];
#define SHA1      1
#define SHA1_96   2

void hmac_init(hid32 id, struct hash_state* state, byte *key, uint32 key_length);
void hmac_update(hid32 id, struct hash_state* state, byte *msg, uint32 length);
void hmac_finish(hid32 id, struct hash_state* state, byte *key, uint32 key_length, byte *diggest);

/* SHA-1 */

/* SHA-1 constantes */
#define SHA1_DIGGEST_SIZE 20

#define IH0 (0x67452301)
#define IH1 (0xEFCDAB89)
#define IH2 (0x98BADCFE)
#define IH3 (0x10325476)
#define IH4 (0xC3D2E1F0)

#define K00 (0x5A827999)
#define K20 (0x6ED9EBA1)
#define K40 (0x8F1BBCDC)
#define K60 (0xCA62C1D6)

/* SHA-1 functions. */
#define __SHA1
#define blk0(t,i) (t[i] = (rol(t[i], 24) & 0xFF00FF00) \
		| (rol(t[i],8) & 0x00FF00FF))

#define blk(t,i) (t[i&15] = rol(t[(i+13)&15]^t[(i+8)&15] \
			^t[(i+2)&15]^t[i&15],1))

#define R00(v,w,x,y,z,t,i) z+=((w&(x^y))^y)+blk0(t,i)+K00+rol(v,5);w=rol(w,30);
#define R16(v,w,x,y,z,t,i) z+=((w&(x^y))^y)+blk(t,i)+K00+rol(v,5);w=rol(w,30);
#define R20(v,w,x,y,z,t,i) z+=(w^x^y)+blk(t,i)+K20+rol(v,5);w=rol(w,30);
#define R40(v,w,x,y,z,t,i) z+=(((w|x)&y)|(w&x))+blk(t,i)+K40+rol(v,5);w=rol(w,30);
#define R60(v,w,x,y,z,t,i) z+=(w^x^y)+blk(t,i)+K60+rol(v,5);w=rol(w,30);

void sha1_init  (struct hash_state* state);
void sha1_update(struct hash_state* state, byte* msg, uint32 length);
void sha1_finish(struct hash_state* state, byte* diggest);
void sha1_96_finish(struct hash_state* state, byte* diggest);

/* Ciphers */
/*****************************************/
/* prototype of any cipher function used */
/* with cbc, ctr function.               */
/* NOTE: it should be safe to call:      */
/*  func(key, block, block);             */
/*****************************************/
typedef void (* CIPHER_FUNCTION) (void* keyblob, byte *block, byte *ciphertext);
typedef void (* KEYGEN_FUNCTION) (byte* master_key, void* keyblob);

typedef int32 cid32;
#define DES     1
#define TDEA    2
#define AES128  3

#define CBC 1

struct ciphent {
	CIPHER_FUNCTION   enc;
	CIPHER_FUNCTION   dec;
	KEYGEN_FUNCTION   gen;
	uint32            block_size;
	uint32            key_size;
	uint32            keyblob_size;
	char*             name;
};

typedef bool8 (* CIPHER_MODE)(cid32 cid, byte* iv, void* keyblob, byte* msg, byte* ciphertext, int32 length);

struct modeent {
	CIPHER_MODE   enc;
	CIPHER_MODE   dec;
	char*         name;
};

extern struct ciphent ciphtab[];
extern struct modeent modetab[];

bool8 cipher_cbc_enc(cid32 cid, byte* iv, void* keyblob, byte* msg, byte* ciphertext, int32 length);
bool8 cipher_cbc_dec(cid32 cid, byte* iv, void* keyblob, byte* msg, byte* ciphertext, int32 length);

bool8 cipher_ctr(cid32 cid, byte* iv, byte* keyblob, byte* msg, byte* ciphertext, int32 length);

/** DES **/

void generate_desip_table();
void des_ip(byte in[8], uint32* left, uint32* right);
void des_ipinv(uint32 left, uint32 right, byte out[8]);
void fast_des_ip(byte in[8], uint32* left, uint32* right);
void fast_des_ipinv(uint32 left, uint32 right, byte out[8]);

void des_estep(uint32 val, byte key[8], byte out[8]);
#define fast_des_estep(val, key, out) \
	*out++ = *key++ ^ (((val & 1) << 5) | (val >> 27)); \
*out++ = *key++ ^ ((val >> 23) & 0x3F); \
*out++ = *key++ ^ ((val >> 19) & 0x3F); \
*out++ = *key++ ^ ((val >> 15) & 0x3F); \
*out++ = *key++ ^ ((val >> 11) & 0x3F); \
*out++ = *key++ ^ ((val >> 7) & 0x3F);  \
*out++ = *key++ ^ ((val >> 3) & 0x3F);  \
*out = *key ^ (((val << 1) & 0x3F) | (val >> 31))

#define des_key_shift(val, shift) (((val << shift) & 0x0FFFFFFF) | ((val >> (28 - shift)) & 0xF))

extern byte s_boxes[8][4][16];
void generate_desperm_table();

uint32 des_funcf(uint32 R, byte* key);
uint32 fast_des_funcf(uint32 R, byte* key);
void des_enc(byte keyblob[16][8], byte* block, byte* ciphertext);
void des_encrypt(byte key[8], byte* block, byte* ciphertext);
void des_dec(byte keyblob[16][8], byte* block, byte* ciphertext);
void des_decrypt(byte key[8], byte* block, byte* ciphertext);

void des_generatekeys(byte* key, byte round_key[16][8]);

/** TDEA **/
struct tdea_keyblob {
	byte key_1[16][8];
	byte key_2[16][8];
	byte key_3[16][8];
};

void tdea_generatekeys(byte* key, struct tdea_keyblob* key_b);

void tdea_enc(struct tdea_keyblob* key, byte* block, byte* ciphertext);
void tdea_dec(struct tdea_keyblob* key, byte* block, byte* ciphertext);
void tdea_encrypt(byte key[24], byte* block, byte* ciphertext);
void tdea_decrypt(byte key[24], byte* block, byte* ciphertext);

/* Signature algorithms. */

struct dsacert {
	MPPRIME   p;
	MPINT     q;
	MPINT     g;
	MPINT     y;
};

struct dsapriv {
	MPPRIME   p;
	MPINT     q;
	MPINT     g;
	MPINT     x;
};

struct dsasign {
	MPINT     r;
	MPINT     s;
};

void init_dsacert(struct dsacert* cert);
void init_dsasign(struct dsasign* sign);
void init_dsapriv(struct dsapriv* priv);
bool8 dsa_sign(byte* msg, int32 length, struct dsapriv *priv, hid32 hashid, randctx* ctx, struct dsasign *sign);
bool8 dsa_check(byte* msg, int32 length, struct dsacert *cert, hid32 hashid, struct dsasign *sign);
void clear_dsacert(struct dsacert* cert);
void clear_dsasign(struct dsasign* sign);
void clear_dsapriv(struct dsapriv* priv);


#endif // CRYPTO_H
