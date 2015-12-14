#include <xinu.h>

bool8 montgomery_mpint(MPINT *v1, MPINT *v2, MPINT* n, MPINT* inv_n) {

  MPINT tmpghost, tmp1, tmp2;
  bool8 st;

  /* First compute v1 * v2 */
  init_mpint(&tmp1);
  if (mul_mpint(&tmp1, v1, v2, FALSE) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }

  /* Extract the less significant byte [0 -> n->size - 1[ */
  if (subvalghost_mpint(&tmpghost, &tmp1, 0, n->size - 1) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }

  /* multiply tmp by inv_n mod 2^(n->size - 1) */
  init_mpint(&tmp2);
  if (mul_mpint(&tmp2, inv_n, &tmpghost, TRUE) == FALSE) {
    clear_mpint(&tmp1);
    clear_mpint(&tmp2);
    return FALSE;
  }

  /* multiply dst by n and add to tmp */
  st = mul_mpint(&tmp1, n, &tmp2, FALSE);
  if (st == FALSE) {
    clear_mpint(&tmp1);
    clear_mpint(&tmp2);
    return FALSE;
  }
  clear_mpint(&tmp2);

  if (subval_mpint(v1, 0, &tmp1, n->size - 1,  n->size - 1) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }
  clear_mpint(&tmp1);

  if ((st == DIRTY || unsigncmp_mpint(v1, n) >= 0) && diff_mpint(v1, n, 1) == FALSE)
    return FALSE;

  return TRUE;

}

bool8 montgomery_inv_mpint(MPINT *v, MPINT* n, MPINT* inv_n) {

  MPINT tmp1;
  bool8 st;

  /* multiply tmp by inv_n mod 2^(n->size - 1) */
  init_mpint(&tmp1);
  if (mul_mpint(&tmp1, inv_n, v, TRUE) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }

  st = mul_mpint(v, n, &tmp1, FALSE);
  if (st == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }
  clear_mpint(&tmp1);

  if (rshifthw_mpint(v, n->size - 1) == FALSE) {
    return FALSE;
  }

  if ((st == DIRTY || unsigncmp_mpint(v, n) >= 0) && diff_mpint(v, n, 1) == FALSE)
    return FALSE;

  return TRUE;
}

bool8 montgomery_square_mpint(MPINT *v1, MPINT* n, MPINT* inv_n) {

  MPINT tmpghost, tmp1, tmp2;
  bool8 st;

  /* First compute v1^2 */
  init_mpint(&tmp1);
  if (square_mpint(&tmp1, v1) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }

  /* Extract the less significant byte [0 -> n->size - 1[ */
  if (subvalghost_mpint(&tmpghost, &tmp1, 0, n->size - 1) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }

  /* multiply tmp by inv_n mod 2^(n->size - 1) */
  init_mpint(&tmp2);
  if (mul_mpint(&tmp2, inv_n, &tmpghost, TRUE) == FALSE) {
    clear_mpint(&tmp1);
    clear_mpint(&tmp2);
    return FALSE;
  }

  /* multiply dst by n and add to tmp */
  st = mul_mpint(&tmp1, n, &tmp2, FALSE);
  if (st == FALSE) {
    clear_mpint(&tmp1);
    clear_mpint(&tmp2);
    return FALSE;
  }
  clear_mpint(&tmp2);

  if (subval_mpint(v1, 0, &tmp1, n->size - 1,  n->size - 1) == FALSE) {
    clear_mpint(&tmp1);
    return FALSE;
  }
  clear_mpint(&tmp1);

  if ((st == DIRTY || unsigncmp_mpint(v1, n) >= 0) && diff_mpint(v1, n, 1) == FALSE)
    return FALSE;

  return TRUE;

}
