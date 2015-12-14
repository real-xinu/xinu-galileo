#include <xinu.h>

bool8 invmod_mpint(MPINT* x, MPINT* p) {

    MPINT y1, y2, a, b, q, r;
    MPINT tmp;
    bool8 st;

    if (cmp_mpint(x, p) >= 0) {
        return FALSE;
    }

    if (x->order == 1 && x->val[0] == 1) {
        return TRUE;
    }

    /* First division */
    init_mpint(&y1);
    init_mpint(&b);
    if (unsigndiv_mpint(p, x, &y1, &b) == FALSE) {
        clear_mpint(&y1);
        clear_mpint(&b);
        return FALSE;
    }
    (&y1)->sign = -1;

    st = TRUE;
    /* Initialize other variables. */
    init_mpint(&y2);
    init_mpint(&a);
    if (copy_mpint(&a, x) == FALSE || \
            word_mpint(&y2, 1, 1) == FALSE) {
        clear_mpint(&y1);
        clear_mpint(&b);
        clear_mpint(&y2);
        clear_mpint(&a);
        return FALSE;
    }

    init_mpint(&q);
    init_mpint(&r);
    init_mpint(&tmp);

    /* Extended Euclidean Algorithm */
    while ((&b)->order != 1 || ((&b)->val[0] != 1 && (&b)->val[0] != 0)) {
        /* Make division */
        if (unsigndiv_mpint(&a, &b, &q, &r) == FALSE || \
                /* erase tmp */
                word_mpint(&tmp, 0, (&q)->order + (&y1)->order) == FALSE || \
                /* tmp = q * y1 */
                mul_mpint(&tmp, &y1, &q, FALSE) != TRUE || \
                /* y2 = y2 - y1 * q */
                sub_mpint(&y2, &tmp) == FALSE) {
            st = FALSE;
            break;
        } else {
            swap_mpint(&y1, &y2);
            /* a = b, b = r */
            swap_mpint(&a, &b);
            swap_mpint(&b, &r);

        }
    }

    /* b should be equal to 1. */
    if ((&b)->order != 1 || (&b)->val[0] != 1)
        st = FALSE;

    if (st == TRUE) {
        swap_mpint(x, &y1);
        if (x->sign == -1) {
            x->sign = 1;
            st = diff_mpint(x, p, -1);
        }
    }

    clear_mpint(&y1);
    clear_mpint(&y2);
    clear_mpint(&a);
    clear_mpint(&b);
    clear_mpint(&q);
    clear_mpint(&r);
    clear_mpint(&tmp);

    return st;
}
