#include <xinu.h>

bool8 rshift_mpint(MPINT* x, int32 shift) {

  int32 startHW, nbits;
  int32 i;
  HALF_WORD *wt;

  if (shift <= 0 || x->order == 0)
    return TRUE;

  startHW = shift / HW_BITS;
  nbits = shift % HW_BITS;

  if (startHW >= x->order) {
    return word_mpint(x, 0, 1);
  }

  wt = x->val + startHW;
  for (i = 0 ; i < x->order - startHW - 1 ; i++) {
    x->val[i] = (*((WORD *)wt) >> nbits) & HW_MASK;
    wt++;
  }
  x->val[i++] = (*wt >> nbits) & HW_MASK;
  for ( ; i < x->order ; i++) {
    x->val[i] = 0;
  }

  /* update order */
  x->order -= startHW;
  if (x->val[x->order-1] == 0)
    x->order--;

  return TRUE;

}

bool8 rshifthw_mpint(MPINT* x, int32 shift) {

  int32 i;

  if (shift <= 0 || x->order == 0)
    return TRUE;

  if (shift >= x->order) {
    return word_mpint(x, 0, 1);
  }

  for (i = 0 ; i < x->order - shift ; i++) {
    x->val[i] = x->val[i + shift];
  }
  for ( ; i < x->order ; i++) {
    x->val[i] = 0;
  }

  /* update order */
  x->order -= shift;

  return TRUE;

}

bool8 lshift_mpint(MPINT* x, int32 shift) {

  int32 startHW, nbits;
  int32 i, new_order;
  HALF_WORD *wt;

  if (shift <= 0 || x->order == 0)
    return TRUE;

  startHW = shift / HW_BITS;
  nbits = shift % HW_BITS;

  if ((((WORD)x->val[x->order - 1] << nbits) & HW_MASK) > 0)
    new_order = x->order + startHW + 1;
  else
    new_order = x->order + startHW;

  if (extend_mpint(x, new_order) == FALSE) {
    return FALSE;
  }

  wt = x->val + x->order - 1;
  for (i = x->order + startHW ; i > startHW ; i--) {
    x->val[i] = ((*((WORD *)wt) << nbits) >> HW_BITS) & HW_MASK;
    wt--;
  }

  x->val[i--] = x->val[0] << nbits;
  for ( ; i >=0 ; i--) {
    x->val[i] = 0;
  }

  /* update order */
  x->order = new_order;

  return TRUE;
}

bool8 safelshift_mpint(MPINT* x, int32 shift) {

  int32 i;
  uint32 carry;

  if (shift > HW_BITS)
    return FALSE;

  if (shift <= 0 || x->order == 0)
    return TRUE;

  carry = 0;
  for (i = 0 ; i < x->order ; i++) {
    carry = ((WORD) x->val[i] << shift) | carry;
    x->val[i] = carry & HW_MASK;
    carry >>= HW_BITS;
  }
  if (carry) {
    x->val[x->order] = carry;
    x->order++;
  }

  return TRUE;
}
