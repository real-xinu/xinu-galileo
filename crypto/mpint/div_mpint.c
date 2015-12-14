#include <xinu.h>

bool8 subroutine_div(MPINT* a, MPINT* b, MPINT* q, MPINT* r);
bool8 aux_div(MPINT* a, MPINT* b, MPINT* q, MPINT* r);

bool8 unsigndiv_mpint(MPINT* a, MPINT* b, MPINT* q, MPINT* r) {
    uint16 tmp, mask, nbits;
    int32 sa, sb;
    MPINT tmpa, tmpb;
    bool8 st;

    if (b->order == 0) // Division by 0
        return FALSE;

    /* Found b more significant bit. */
    mask = 1 << (HW_BITS - 1);
    tmp = b->val[b->order-1];
    nbits = 0;
    while ((tmp & mask) == 0) {
        mask >>= 1;
        nbits++;
    }

    /* initialize q and r */
    if (word_mpint(q, 0, MAX(a->order - b->order + 1, 0)) == FALSE || \
            word_mpint(r, 0, MIN(a->order, b->order)) == FALSE) {
        return FALSE;
    }

    /* pre process a and b */
    if (nbits != 0) {
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
    st = aux_div(a, b, q, r);

    /* post process data */
    if (nbits != 0) {
        clear_mpint(a);
        clear_mpint(b);
        rshift_mpint(r, nbits);
    }
    a->sign = sa;
    b->sign = sb;

    if (r->order >= r->size)
        return FALSE;

    return st;
}

bool8 aux_div(MPINT* a, MPINT* b, MPINT* q, MPINT* r) {

    MPINT A, s, q1, r1;
    bool8 st;

    /* a < b */
    if (a->order < b->order) {
        return copy_mpint(r, a);;
    }

    if (a->order == b->order) {
        if (unsigncmp_mpint(a, b) < 0) {
            return copy_mpint(r, a);
        } else {
            q->val[0] = 1;
            q->order = 1;
            if (copy_mpint(r, a) == FALSE || \
                    diff_mpint(r, b, 1) == FALSE) {
                return FALSE;
            }
            return TRUE;
        }
    }

    if (a->order == b->order + 1) {
        return subroutine_div(a, b, q, r);
    }

    st = TRUE;
    init_mpint(&A);
    init_mpint(&s);
    init_mpint(&q1);
    init_mpint(&r1);
    /* Extract MSB of a */
    if (subval_mpint(&A, 0, a, a->order - b->order - 1, b->order + 1) == FALSE || \
            /* Extract LSB of a */
            subval_mpint(&s, 0, a, 0, a->order - b->order - 1) == FALSE || \

            /* Compute MSB(a)/b into q1, r1*/
            subroutine_div(&A, b, &q1, &r1) == FALSE || \
            /* s = r || s */
            subval_mpint(&s, a->order - b->order - 1, &r1, 0, (&r1)->order) == FALSE || \
            /* Compute s/b into q,r */
            aux_div(&s, b, q, r) == FALSE || \
            /* q = q1 || q */
            subval_mpint(q, a->order - b->order - 1, &q1, 0, (&q1)->order) == FALSE) {
        st = FALSE;

    }


    clear_mpint(&A);
    clear_mpint(&s);
    clear_mpint(&q1);
    clear_mpint(&r1);

    return st;
}

bool8 subroutine_div(MPINT* a, MPINT* b, MPINT* q, MPINT* r) {
    MPINT A, sB;
    bool8 st = TRUE;
    uint32 qc;

#ifdef SSH_DEBUG
    if (b->order == 0 || (b->val[b->order-1] & 0x8000) == 0) {
        kprintf("Subroutine_div: fail B format is incorrect\n");
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

                /* recursive call */
                subroutine_div(&A, b, q, r) == FALSE || \
                /* q <- q + 2^16 */
                addcst_mpint(q, 1, 1) != TRUE) {
            st = FALSE;
        }

        /* Clean up. */
        clear_mpint(&A);
        clear_mpint(&sB);

        return st;
    }


#ifdef SSH_DEBUG
    if (a->val[b->order] == 0) {
        kprintf("Subroutine_div: fail A format is incorrect\n");
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
            if (diff_mpint(r, b, 1) == FALSE)
                return FALSE;
        }
    }

    /* put cq in q. */
    if (word_mpint(q, HW_MASK & qc, 1) == FALSE || \
            /* Update reminder */
            diff_mpint(r, a, -1) == FALSE)
        return FALSE;

    return TRUE;
}
