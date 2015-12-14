#include <xinu.h>

bool8 rand_mpint(MPINT *x, int32 length, randctx* ctx) {

  int32 i;
  WORD *wt;

  if (extend_mpint(x, length) == FALSE)
    return FALSE;

  wt = (WORD *)(x->val);
  for (i = 0 ; i < ((length+ 1) >> 1) ; ++i) {
    *wt++ = crand(ctx);
  }

  x->val[length] = 0;

  return uporder_mpint(x);
}

bool8 randmod_mpint(MPINT* k, MPINT* q, randctx* ctx) {
  int32 i, j;
  bool8 st;

  j = 0;
  while (j++ < RAND_TRIES && rand_mpint(k, q->order, ctx) != FALSE) {
    if (k->order > q->order)
      continue;
    else if (k->order < q->order) {
      return TRUE;
    }

    i = (k->order - 1);
    while (i >= 0 &&  k->val[i] == q->val[i])
      i--;

    if ((i >= 1 && k->val[i] < q->val[i]) ||   // q >= 2^16 + k => q - 2 >= k
        (i == 0 && k->val[i] + 2 <= q->val[i])) {  // q - 2 >= k
      st = addcst_mpint(k, 1, 0);
      return st;
    }
  }

  // Bad luck or error in the random generation
  return FALSE;
}
