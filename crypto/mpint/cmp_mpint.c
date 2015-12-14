#include <xinu.h>

int32 cmp_mpint(MPINT *v1, MPINT *v2) {
  int32 i;

  if (v1->sign != v2->sign)
    return v1->sign > v2->sign ? 1 : -1;

  if (v1->order != v2->order)
    return v1->order > v2->order ? v1->sign : -v1->sign;

  i = (v1->order - 1);
  while (i >= 0 &&  v1->val[i] == v2->val[i])
    i--;

  if (i == -1)
    return 0;

  return v1->val[i] > v2->val[i] ? v1->sign : -v1->sign;
}

int32 unsigncmp_mpint(MPINT *v1, MPINT *v2) {
  int32 i;

  if (v1->order != v2->order)
    return v1->order > v2->order ? 1 : -1;

  i = (v1->order - 1);
  while (i >= 0 &&  v1->val[i] == v2->val[i])
    i--;

  if (i == -1)
    return 0;

  return v1->val[i] > v2->val[i] ? 1 : -1;
}

bool8 iszero_mpint(MPINT *v1) {

  int32 i;
  i = 0;
  while (i < v1->order && v1->val[i] == 0)
    i++;

  return v1->order == i ? TRUE : FALSE;
}
