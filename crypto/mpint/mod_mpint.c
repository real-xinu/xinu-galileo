#include <xinu.h>

bool8 subroutine_mod(MPINT* a, MPINT* b, MPINT* r);
bool8 aux_mod(MPINT* a, MPINT* b, MPINT* r);

bool8 unsignmod_mpint(MPINT* r, MPINT* a, MPINT* b) {
    uint16 tmp, mask, nbits;
    int32 sa, sb;
    MPINT tmpa, tmpb;
    bool8 st;

    if (b->order == 0) {// Division by 0
        return FALSE;
    }

    /* Found b more significant bit. */
    mask = 1 << (HW_BITS - 1);
    tmp = b->val[b->order-1];
    nbits = 0;
    while ((tmp & mask) == 0) {
        mask >>= 1;
        nbits++;
    }


    /* initialize r */
    if (word_mpint(r, 0, MIN(a->order, b->order)) == FALSE) {
        return FALSE;
    }

    /* pre process a and b */
    if (nbits > 0) {
        init_mpint(&tmpa);
        init_mpint(&tmpb);
        if (copy_mpint(&tmpa, a) == FALSE ||
                copy_mpint(&tmpb, b) == FALSE) {
        }
        a = &tmpa;
        b = &tmpb;
        safelshift_mpint(a, nbits);
        safelshift_mpint(b, nbits);
    }
    sa = a->sign;
    sb = b->sign;
    a->sign = 1;
    b->sign = 1;

    /* Divide */
    st = aux_mod(a, b, r);

    /* post process data */
    if (nbits != 0) {
        clear_mpint(a);
        clear_mpint(b);
        rshift_mpint(r, nbits);
    }
    a->sign = sa;
    b->sign = sb;

    if (r->order >= r->size) {
        return FALSE;
    }

    return st;
}

bool8 aux_mod(MPINT* a, MPINT* b, MPINT* r) {

    MPINT A, s, r1;
    bool8 st;

    /* a < b */
    if (a->order < b->order) {
        return copy_mpint(r, a);
    }

    if (a->order == b->order) {
        if (unsigncmp_mpint(a, b) < 0) {
            return copy_mpint(r, a);
        } else {
            if (copy_mpint(r, a) == FALSE || \
                    diff_mpint(r, b, 1) == FALSE) {
                return FALSE;
            }
            return TRUE;
        }
    }

    if (a->order == b->order + 1) {
        return subroutine_mod(a, b, r);
    }

    st = TRUE;
    init_mpint(&A);
    init_mpint(&s);
    init_mpint(&r1);
    /* Extract MSB of a */
    if (subval_mpint(&A, 0, a, a->order - b->order - 1, b->order + 1) == FALSE || \
            /* Extract LSB of a */
            subval_mpint(&s, 0, a, 0, a->order - b->order - 1) == FALSE || \
            /* Compute MSB(a) mod b into r1*/
            subroutine_mod(&A, b, &r1) == FALSE || \
            /* s = r1 || s */
            subval_mpint(&s, a->order - b->order - 1, &r1, 0, (&r1)->order) == FALSE || \
            /* Compute s mod b into r */
            aux_mod(&s, b, r) == FALSE) {
        st = FALSE;
    }

    clear_mpint(&A);
    clear_mpint(&s);
    clear_mpint(&r1);

    return st;
}

bool8 subroutine_mod(MPINT* a, MPINT* b, MPINT* r) {
    MPINT A, sB;
    bool8 st = TRUE;
    uint32 qc;

#ifdef SSH_DEBUG
    if (b->order == 0 || (b->val[b->order-1] & 0x8000) == 0) {
        kprintf("Subroutine_mod: fail B format is incorrect\n");
        print_mpint(b);

        return FALSE;
    }
#endif

    /* If a >= 2^HW_BITS * b */
    if (a->order > b->order + 1 || a->val[a->order-1] >= b->val[b->order-1]) {
        init_mpint(&A);
        init_mpint(&sB);
        /* compute A - 2^16 * B. */
        /* A <- a - 2^HW_BITS * b */
        if (copy_mpint(&A, a) == FALSE || \
                subval_mpint(&sB, 1, b, 0, b->order) == FALSE || \
                diff_mpint(&A, &sB, 1) == FALSE || \
                subroutine_mod(&A, b, r) == FALSE) {
            st = FALSE;
        }

        /* Clean up. */
        clear_mpint(&A);
        clear_mpint(&sB);

        return st;
    }


#ifdef SSH_DEBUG
    if (a->val[b->order] == 0) {
        kprintf("Subroutine_mod: fail A format is incorrect\n");
        print_mpint(a);

        return FALSE;
    }
#endif

    qc = ((WORD) ((a->val[b->order] & HW_MASK) << HW_BITS) | (a->val[b->order-1] & HW_MASK))/(HW_MASK & b->val[b->order-1]);

    if (qc > HW_MASK)
        qc = HW_MASK;

    if (copy_mpint(r, b) == FALSE || \
            mulcst_mpint(r, (HALF_WORD)qc) == FALSE) {
        return FALSE;
    }

    if (unsigncmp_mpint(r, a) >= 0) {
        qc--;
        if (diff_mpint(r, b, 1) == FALSE)
            return FALSE;

        if (unsigncmp_mpint(r, a) >= 0) {
            qc--;
            if (diff_mpint(r, b, 1) == FALSE) {
                return FALSE;
            }
        }
    }

    /* Update remainder */
    if (diff_mpint(r, a, -1) == FALSE)
        return FALSE;

    return TRUE;
}
