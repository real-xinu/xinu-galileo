#include <xinu.h>

/*
 * This function perform dst <- dst + v1 * v2
 * If dst is negative the output will be invorrect.
 * return FALSE if any problem occur
 * WARNING: even if TRUNC if false, the result can be truncated if the
 * initial value of dst is not 0. In this case the function return the
 * value DIRTY.
 */
bool8 mul_mpint(MPINT *dst, MPINT *v1, MPINT *v2, bool8 trunc) {

  int32 i, j;
  int32 bound;
  int32 hio, loo;
  HALF_WORD *hi, *lo;
  WORD carry = 0;

  /* 0 check */
  if (v1->order == 0 || v2->order == 0) {
    return word_mpint(dst, 0, 1);
  }

  /* Set the dst to the correct size. */
  if (trunc) {
    if (extend_mpint(dst, v1->order) == FALSE)
      return FALSE;
  } else {
    if (extend_mpint(dst, v1->order + v2->order) == FALSE)
      return FALSE;
  }

  if (v1->order >= v2->order) {
    hi = v1->val;
    lo = v2->val;
    hio = v1->order;
    loo = v2->order;
  } else {
    hi = v2->val;
    lo = v1->val;
    hio = v2->order;
    loo = v1->order;
  }

  /* Compute the product. */
  for (j = 0 ; j < loo ; ++j) {
    carry = 0;
    bound = (trunc ? MIN(v1->size - 1, j + hio)  : j + hio);
    for (i = j ; i < bound ; i++) {
      carry += (WORD) lo[j] * hi[i-j] + dst->val[i];
      dst->val[i] = carry & HW_MASK;
      carry >>= HW_BITS;
    }

    /* propagate the carry */
    bound = (trunc ? v1->size - 1 : dst->size - 1);
    while (carry > 0 && i < bound) {
      carry += dst->val[i];
      dst->val[i++] = carry & HW_MASK;
      carry >>= HW_BITS;
    }
  }

  /* Update the sign. */
  dst->sign = v1->sign * v2->sign;
  if (trunc) {
    uporder_mpint(dst);
  } else {
    if (carry > 0) {
      dst->val[dst->size-1] = carry;
      dst->order = dst->size;
      return DIRTY;
    } else {
      if (dst->order < v1->order + v2->order)
          dst->order = v1->order + v2->order;
      if (dst->val[dst->order - 1] == 0) {
        dst->order--;
      }
    }
  }

  return TRUE;
}

/*
 * This function perform dst <- v1^2
 * return FALSE if any problem occur
 * dst have to be a empty mpint
 */
bool8 square_mpint(MPINT *dst, MPINT *v) {

  int32 i, j, k;
  WORD carry = 0;

  /* 0 check */
  if (v->order == 0) {
    return word_mpint(dst, 0, 1);
  }

  /* Set the dst to the correct size. */
  if (extend_mpint(dst, v->order << 1) == FALSE) {
    return FALSE;
  }

  /* Compute the square. */
  /* First compute cross factor ONCE */
  for (j = 0 ; j < v->order-1 ; j++) {
    i = (j << 1) + 1;
    carry = (WORD) v->val[j] * v->val[j+1] + dst->val[i];
    dst->val[i++] = carry & HW_MASK;
    carry >>= HW_BITS;

    for (k = j + 2; i < j + v->order ; i++, k++) {
      carry += (WORD) v->val[j] * v->val[k] + dst->val[i];
      dst->val[i] = carry & HW_MASK;
      carry >>= HW_BITS;
    }
    while (carry > 0 && i < (v->order << 1)) {
      carry += dst->val[i];
      dst->val[i++] = carry & HW_MASK;
      carry >>= HW_BITS;
    }
  }

  /* Update apparent order */
  dst->order = v->order << 1;
  /* multiplyby 2 */
  safelshift_mpint(dst, 1);

  /* Add the square terms */
  carry = (WORD) (v->val[0]) * (WORD) (v->val[0]);
  dst->val[0] = (HALF_WORD) (carry & HW_MASK);
  carry >>= HW_BITS;
  carry += dst->val[1];
  dst->val[1] = (HALF_WORD) (carry & HW_MASK);
  carry >>= HW_BITS;
  for (i = 2 ; i < (v->order << 1) ; i+=2) {
    carry +=   (WORD) v->val[i>>1] * v->val[i>>1] + dst->val[i];
    dst->val[i] = (HALF_WORD) (carry & HW_MASK);
    carry >>= HW_BITS;
    carry += dst->val[i+1];
    dst->val[i+1] = (HALF_WORD) (carry & HW_MASK);
    carry >>= HW_BITS;
  }

  /* Update the sign. */
  dst->sign = 1;

  /* Update the order. */
  if (dst->val[dst->order - 1] == 0) {
    dst->order--;
  }

  return TRUE;
}

bool8 mulcst_mpint(MPINT *v1, HALF_WORD n) {

  int32 j;
  WORD carry = 0;

  /* 0 check */
  if (v1->order == 0) {
    return TRUE;
  }

  if (n == 0) {
    return word_mpint(v1, 0, 1);
  }

  /* Update the sign. */
  if (n < 0) {
    v1->sign = -v1->sign;
  }

  /* Compute the product. */
  for (j = 0 ; j < v1->order ; ++j) {
    carry += (WORD) v1->val[j] * n;
    v1->val[j] = carry & HW_MASK;
    carry >>= HW_BITS;
  }

  if (carry > 0) {
    v1->val[v1->order++] = carry;
    if (v1->order < v1->size) {
      return TRUE;
    } else {
      return DIRTY;
    }
  }

  return TRUE;
}
