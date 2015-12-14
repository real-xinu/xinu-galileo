#ifndef MPINT_H
#define MPINT_H

typedef uint16  HALF_WORD;
typedef uint32  WORD;
typedef int32   SWORD;
#define HW_BITS (16)
#define HW_MASK (0xFFFF)

#define MPINT_DEBUG

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* Only with power of 2!! */
#define roundm(x, m) (((x) + (m) - 1) & ~((m) - 1))
#define ismultipleof(x, m) ((x) == roundm((x), (m)))
#define byte_toHW(x) ((roundm((x), sizeof(HALF_WORD))) / sizeof(HALF_WORD))

/* Structures */
#define MPINT_INIT  1024  // # of bit for the smallest mpint
#define MPINT_HWINIT  64  // # of HW for the smallest mpint

typedef struct mpint {
  HALF_WORD*  val;    // number representation using little endian
                      // the array size is size
  uint32      size;   // has to be (a multiple of MPINT_HWINIT) + 1
  uint32      order;  // index of the higher HW <> 0
  int32       sign;   // sign of the number
} MPINT;

/* More memory is allocated when order + EXTEND_THRES >= size */
//#define EXTEND_THRES 2

// The precomputed values are used for the Montgomery
// reduction in order to speed up multiplications modulo
// this prime number
typedef struct prime {
  MPINT num;       // a prime number
  MPINT inv;       // -num mod 2^size
  MPINT r_square;  // 2^(2*size) mod num
} MPPRIME;

/* Tests */
void launch_mpint();

/* utils_mpint */

MPINT* new_mpint();
void init_mpint(MPINT* m);
bool8 word_mpint(MPINT* n, SWORD x, int32 initsize);
bool8 extend_mpint(MPINT* n, int32 new_size);
void delete_mpint(MPINT* n);
void clear_mpint(MPINT* n);
bool8 copy_mpint(MPINT* dst, MPINT* src);
bool8 subval_mpint(MPINT* dst, int32 offset, MPINT* src, int32 start, int32 length);
bool8 subvalghost_mpint(MPINT* dst, MPINT* src, int32 start, int32 length);
bool8 uporder_mpint(MPINT *n);
void swap_mpint(MPINT* v1, MPINT* v2);
void write_bigendian_mpint(byte* buff, MPINT* src, int32 len);
int32 hton_mpint(byte* buff, MPINT* src, int32 len);
bool8 read_bigendian_mpint(MPINT* dst, byte* buff, int32 length);
int32 ntoh_mpint(MPINT* dst, byte* buff);
bool8 parse_mpint(MPINT* n, char* val, int32 length);
void print_mpint(MPINT* val);
void tostatic_mpint(MPINT* val);
void print_mpintbin(MPINT* val);

/* cmp_mpint */
int32 cmp_mpint(MPINT *v1, MPINT *v2);
int32 unsigncmp_mpint(MPINT *v1, MPINT *v2);
bool8 iszero_mpint(MPINT *v1);

/* sum_mpint */

bool8 add_mpint(MPINT *v1, MPINT *v2);
bool8 sub_mpint(MPINT *v1, MPINT *v2);
bool8 unsignadd_mpint(MPINT *v1, MPINT *v2);
bool8 diff_mpint(MPINT *v1, MPINT *v2, int32 biggest);
bool8 addcst_mpint(MPINT* v, HALF_WORD d, int32 index);

/* mul_pint */
#define DIRTY 2
bool8 mul_mpint(MPINT *dst, MPINT *v1, MPINT *v2, bool8 trunc);
bool8 square_mpint(MPINT *dst, MPINT *v1);
bool8 mulcst_mpint(MPINT *v1, HALF_WORD n);

/* shift_mpint */
bool8 lshift_mpint(MPINT* x, int32 shift);
bool8 safelshift_mpint(MPINT* x, int32 shift);
bool8 rshift_mpint(MPINT* x, int32 shift);
bool8 rshifthw_mpint(MPINT* x, int32 shift);


/* Montgomery */
bool8 montgomery_mpint(MPINT *v1, MPINT *v2, MPINT* n, MPINT* inv_n);
bool8 montgomery_inv_mpint(MPINT *v1, MPINT* n, MPINT* inv_n);
bool8 montgomery_square_mpint(MPINT *v, MPINT* n, MPINT* inv_n);

#define montgomery_mul_mpint(v1, v2, p) montgomery_mpint(v1, v2, &((p)->num), &((p)->inv))
#define montgomery_sqr_mpint(v1, p) montgomery_square_mpint(v1, &((p)->num), &((p)->inv))
#define to_montgomery_mpint(v1, p) montgomery_mpint(v1, &((p)->r_square), &((p)->num), &((p)->inv))
#define from_montgomery_mpint(v1, p) montgomery_inv_mpint(v1, &((p)->num), &((p)->inv))

/* pow */
#define REAL        0
#define MONTGOMERY  1
bool8 powmod_mpint(MPINT *b, MPINT *n, MPPRIME *N, int32 output_space);

/* div */
bool8 unsigndiv_mpint(MPINT* a, MPINT* b, MPINT* q, MPINT* r);

/* mod */
bool8 unsignmod_mpint(MPINT* r, MPINT* a, MPINT* b);

/* invmod */
bool8 invmod_mpint(MPINT* x, MPINT* p);

/* prime */
MPPRIME* new_mpprime();
void init_mpprime(MPPRIME* mp);
void clear_mpprime(MPPRIME* n);
bool8 precompprime_mpint(MPPRIME* prime);
void delete_mpprime(MPPRIME *val);
bool8 copy_mpprime(MPPRIME* dst, MPPRIME* src);

/* random */
extern randctx mpint_ctx;

void initrand_mpint();

#define RAND_TRIES 10
bool8 rand_mpint(MPINT *x, int32 length, randctx* ctx);
bool8 randmod_mpint(MPINT *x, MPINT* p, randctx* ctx);

#endif // MPINT_H
