#include <xinu.h>

bool8 powmod_mpint(MPINT *b, MPINT *n, MPPRIME *N, int32 output_space) {
  int32 j, k, max;
  uint16 mask;
  HALF_WORD tmp;
  MPINT res;

  if (n->order == 0) {
    return word_mpint(b, 1, 1);
  }

  /* init result with 1. */
  init_mpint(&res);
  if (word_mpint(&res, 1, (N->num).order) == FALSE) {
    clear_mpint(&res);
    return FALSE;
  }

  /* Found the values of the highest bit in exponent. */
  mask = 1 << (HW_BITS - 1);
  tmp = n->val[n->order-1];
  max = HW_BITS;
  while ((tmp & mask) == 0) {
    mask >>= 1;
    max--;
  }

  /* To Montgomery space */
  if (montgomery_mpint(b, &(N->r_square), &(N->num), &(N->inv)) == FALSE ||
      montgomery_mpint(&res, &(N->r_square), &(N->num), &(N->inv)) == FALSE) {
    clear_mpint(&res);
    return FALSE;
  }

  /* Now the troubles start. */
  for (j = 0 ; j < n->order-1 ; ++j) {
    mask = 1;
    tmp = n->val[j];
    for (k = 0 ; k < HW_BITS ; ++k) {
      if ((((tmp & mask) > 0) && montgomery_mpint(&res, b, &(N->num), &(N->inv)) == FALSE) \
          || montgomery_square_mpint(b, &(N->num), &(N->inv)) == FALSE) {
        clear_mpint(&res);
        return FALSE;
      }
      mask <<= 1;
    }
  }

  mask = 1;
  tmp = n->val[n->order-1];
  for (k = 0 ; k < max-1 ; ++k) {
    if ((((tmp & mask) > 0) && montgomery_mpint(&res, b, &(N->num), &(N->inv)) == FALSE) \
        || montgomery_square_mpint(b, &(N->num), &(N->inv)) == FALSE) {
      clear_mpint(&res);
      return FALSE;
    }
    mask <<= 1;
  }

  if (montgomery_mpint(b, &res, &(N->num), &(N->inv)) == FALSE) {
    clear_mpint(&res);
    return FALSE;
  }
  clear_mpint(&res);

  if (output_space == REAL)
    return montgomery_inv_mpint(b, &(N->num), &(N->inv));
  else
    return TRUE;
}
