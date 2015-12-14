#include <xinu.h>

/*
 * This function perforn the operation v1 <- v1 + v2
 */
bool8 add_mpint(MPINT *v1, MPINT *v2) {

    int32 sign, biggest;
    /* Take care of the sign */
    if (v1->sign == v2->sign) {
        sign = v1->sign;
        if (unsignadd_mpint(v1, v2) == FALSE) {
            return FALSE;
        }
    } else {
        biggest = unsigncmp_mpint(v1, v2);
        if (biggest == 0) {
            return word_mpint(v1, 0, 1);
        } else if (biggest == 1) {
            sign = v1->sign;
        } else {
            sign = v2->sign;
        }
        if (diff_mpint(v1, v2, biggest) == FALSE) {
            return FALSE;
        }
    }

    /* Update the sign */
    v1->sign = sign;

    return TRUE;
}

/*
 * This function perforn the operation v1 <- v1 - v2
 */
bool8 sub_mpint(MPINT *v1, MPINT *v2) {

    bool8 st;

    v2->sign = -v2->sign;
    st = add_mpint(v1, v2);
    v2->sign = -v2->sign;

    return st;
}

/*
 * This function perforn the operation v1 <- |v1| + |v2|
 * Note: the sign of v1 is undefined afterward.
 * Return TRUE if the operation is successful FALSE otherwise and v1 state
 * is then undefined.
 */
bool8 unsignadd_mpint(MPINT *v1, MPINT *v2) {
    int32 index, min, max;
    WORD carry;
    HALF_WORD *lo, *ho;

    /* Compute the bound of the loops. */
    if (v1->order >=  v2->order) {
        // v2 is 0 nothing to do
        if (v2->order == 0)
            return TRUE;

        min = v2->order;
        max = v1->order;
        lo = v2->val;
        ho = v1->val;
    } else {
        // v1 is 0, add is then just a copy
        if (v1->order == 0)
            return copy_mpint(v1, v2);

        min = v1->order;
        max = v2->order;

        /* v1 is too small, it has to be extended. */
        if (v1->size <= max && extend_mpint(v1, max + 1) == FALSE) {
            return FALSE;
        }

        lo = v1->val;
        ho = v2->val;

    }

    carry = (WORD) *lo++ + *ho++;
    v1->val[0] = carry & HW_MASK;
    carry >>= HW_BITS;

    for (index = 1 ; index < min ; index++) {
        carry += (WORD)*lo++ + *ho++;
        v1->val[index] = (HALF_WORD) (carry & HW_MASK);
        carry >>= HW_BITS;
    }

    /* Handle the carry */
    while (carry > 0 && index < max) {
        carry += (WORD)*ho++;
        v1->val[index++] = carry & HW_MASK;
        carry >>= HW_BITS;
    }

    /* Handle the last carry */
    if (carry > 0) {
        if (max + 1 == v1->size && extend_mpint(v1, v1->size + 1) == FALSE) {
            return FALSE;
        }
        v1->val[max] = carry;
        v1->order = max + 1;
    } else if (v1->order < v2->order) {
        for (; index < max ; index++)
            v1->val[index] = v2->val[index];
        v1->order = v2->order;
    }

    return TRUE;
}

/*
 * This function perforn the operation v1 <- |v1 - v2|
 * Note: the sign of v1 is undefined afterward.
 * biggest is an indication of the greater value between v1 and v2
 *  1: v1, -1: v2, 0 have to check
 * Return TRUE if the operation is successful FALSE otherwise and v1 state
 * is then undefined.
 */
bool8 diff_mpint(MPINT *v1, MPINT *v2, int32 biggest) {
    int32 index, min, max;
    WORD carry;
    HALF_WORD *lo, *hi;

    /* Compute the bound of the loops. */
    if (v1->order >=  v2->order) {
        // v2 is 0 nothing to do
        if (v2->order == 0)
            return TRUE;

        min = v2->order;
        max = v1->order;
    } else {
        // v1 is 0, add is then just a copy
        if (v1->order == 0)
            return copy_mpint(v1, v2);

        min = v1->order;
        max = v2->order;
    }


    if (biggest == 0) {
        biggest = unsigncmp_mpint(v1, v2);
    }
    if (biggest == 0) {
        return word_mpint(v1, 0, 1);
    } else if (biggest == 1) {
        lo = v2->val;
        hi = v1->val;
    } else {
        /* v1 is too small, it has to be extended. */
        if (v1->size < max && extend_mpint(v1, max) == FALSE) {
            return FALSE;
        }
        lo = v1->val;
        hi = v2->val;
    }

    /* DO substraction. */
    carry = (WORD) *hi++ - *lo++;
    v1->val[0] = carry & HW_MASK;
    carry >>= HW_BITS;
    carry &= 0x1;

    for (index = 1 ; index < min ; index++) {
        carry = (WORD) *hi++ - *lo++ - carry;
        v1->val[index] = (HALF_WORD) (carry & HW_MASK);
        carry >>= HW_BITS;
        carry &= 0x1;
    }

    /* Handle the carry */
    while (carry > 0 && index < max) {
        carry = (WORD) *hi++ - carry;
        v1->val[index++] = carry & HW_MASK;
        carry >>= HW_BITS;
        carry &= 0x1;
    }

    /* Carry has bee carry to the top, we have to recompute the order. */
    if (index == max) {
        uporder_mpint(v1);
        /* if v2 was the highest value, we have to copy the rest. */
    } else if (v1->order < v2->order) {
        for (; index < max ; index++)
            v1->val[index] = v2->val[index];
        v1->order = v2->order;
    }

    return TRUE;
}

/*
 * This function performs v <- v + d * 2^(index * HW_BITS).
 */
bool8 addcst_mpint(MPINT* v, HALF_WORD d, int32 index) {
    WORD carry = d;

    while (carry > 0 && index < v->order) {
        carry += v->val[index];
        v->val[index++] = (HALF_WORD) (carry & HW_MASK);
        carry >>= HW_BITS;
    }

    if (carry > 0) {
        if (v->order < v->size) {
            v->val[v->order++] = carry & HW_MASK;
            return TRUE;
        } else {
            return DIRTY;
        }
    }

    return TRUE;
}
