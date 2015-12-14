#include <xinu.h>

/*
 * Create a new empty mpint.
 */
MPINT* new_mpint() {
    MPINT* mp = (MPINT *)getmem(sizeof(MPINT));
    mp->size = 0;
    mp->order = 0;
    mp->sign = 1;

    return mp;
}

/*
 * Initialize a new empty mpint.
 */
void init_mpint(MPINT* mp) {
    mp->val = (HALF_WORD *)SYSERR;
    mp->size = 0;
    mp->order = 0;
    mp->sign = 1;
}

/*
 * Initialize a new empty mppprime.
 */
void init_mpprime(MPPRIME* mp) {
    init_mpint(&(mp->num));
    init_mpint(&(mp->inv));
    init_mpint(&(mp->r_square));
}

/*
 * Release memory used by the mpint.
 */
void delete_mpint(MPINT* n) {
    if (n != (MPINT *)SYSERR) {
        if (n->val != (HALF_WORD *)SYSERR)
            freemem((char *)n->val, n->size*sizeof(HALF_WORD));
        freemem((char *)n, sizeof(MPINT));
    }
}

/*
 * Release memory used by the mpint.
 */
void clear_mpint(MPINT* n) {
    if (n->val != (HALF_WORD *)SYSERR)
        freemem((char *)n->val, n->size*sizeof(HALF_WORD));
}

/*
 * Release memory used by the mpint.
 */
void clear_mpprime(MPPRIME* n) {
    clear_mpint(&(n->num));
    clear_mpint(&(n->inv));
    clear_mpint(&(n->r_square));
}

/*
 * Extend the integer precision to the new_size.
 * This function doesn't set a lower new length
 */
bool8 extend_mpint(MPINT* n, int32 new_size) {
    WORD* wt, *tmp;
    HALF_WORD* new;
    int32 i;

    new_size = roundm(new_size, MPINT_HWINIT) + 1;

    if (n->size >= new_size)
        return TRUE;

    tmp = (WORD *)getmem(new_size*sizeof(HALF_WORD));
    if (tmp == (WORD *)SYSERR) {
#ifdef MPINT_DEBUG
        kprintf("\nextend_mpint: OutOfMemory\n");
#endif
        return FALSE;
    }

    /* record pointer */
    new = (HALF_WORD *)tmp;

    /* Copy the value. */
    wt = (WORD *)n->val;
    for (i = 0 ; i < ((n->order + 1) >> 1) ; i++)
        *tmp++ = *wt++;

    /* nil the upper part. */
    for (; i < (new_size >> 1) ; ++i)
        *tmp++ = 0;

    /* nil extra HALF_WORD. */
    *((HALF_WORD *) tmp) = 0;

    /* free the memory. */
    freemem((char *)n->val, n->size*sizeof(HALF_WORD));

    /* Update the values. */
    n->size = new_size;
    n->val = new;

    return TRUE;
}

/*
 * Create or set the given mpint to the initial value with 1024bit precision.
 */
bool8 word_mpint(MPINT* n, SWORD val, int32 initsize) {
    WORD* wt;
    int32 i, bound;

    if (n->size == 0) {
        n->size = roundm(initsize, MPINT_HWINIT) + 1;
        n->val = (HALF_WORD *)getmem(n->size*sizeof(HALF_WORD));
        if (n->val == (HALF_WORD *)SYSERR) {
            n->size = 0;
            return FALSE;
        }
        bound = n->size >> 1;
    } else {
        bound = n->order >> 1;
    }

    wt = (WORD *)n->val;
    if (val >= 0) {
        *wt++ = val;
        n->sign = 1;
    } else {
        val = -val;
        *wt++ = val;
        n->sign = -1;
    }

    for (i = 1 ; i < bound ; ++i)
        *wt++ = 0;

    /* nil extra HALF_WORD. */
    *((HALF_WORD *) wt) = 0;

    /* update the order and sign */
    if (val == 0)
        n->order = 0;
    else if (val <= HW_MASK) {
        n->order = 1;
    } else {
        n->order = 2;
    }

    return TRUE;
}

bool8 read_bigendian_mpint(MPINT* dst, byte* buff, int32 length) {
    int32 nbhw, i;
    byte* wt;

    /* extend the mpint accordingly */
    nbhw = byte_toHW(length);
    if (extend_mpint(dst, nbhw) == FALSE) {
        return FALSE;
    }

    wt = (byte *)dst->val + length - 1;
    for (i = 0 ; i < length ; ++i)
        *wt-- = *buff++;
    dst->order = nbhw;

    while (dst->order > 0 && dst->val[dst->order-1] == 0)
        dst->order--;

    return TRUE;
}

int32 ntoh_mpint(MPINT* dst, byte* buff) {
    int32 length, toread;

    /* extract length requiered */
    length = (int32)ntohl(*((int32 *)buff));
    buff += 4;

    if (length == 0) {
        word_mpint(dst, 0, 0);
        return 4;
    }

    /* check first bytes for sign */
    dst->sign = (*buff & 0x80) ? -1 : 1;
    toread = length;
    if (*buff == 0 || *buff == -1) {
        buff++;
        toread--;
    }

    if (read_bigendian_mpint(dst, buff, toread) == FALSE) {
        return SYSERR;
    }

    return length + 4;
}

void write_bigendian_mpint(byte* buff, MPINT* src, int32 len) {
    byte* wt;

    /* Copy into the buffer */
    wt = ((byte *)src->val) + len - 1;
    for (; len > 0; len--)
        *buff++ = *wt--;
}

int32 hton_mpint(byte* buff, MPINT* src, int32 len) {
    int32 nbytes, towrite;
    byte *wt;
    uint32* length;

    length = (uint32 *)buff;
    buff += 4;

    /* find highest significant byte. */
    nbytes = src->order * sizeof(HALF_WORD);
    wt = ((byte *)src->val) + nbytes - 1;
    while (nbytes > 0 && *wt == 0) {
        nbytes--;
        wt--;
    }

    if (nbytes == 0) {
        *length = 0;
        return 4;
    }

    towrite = nbytes;
    if (src->sign > 0 && (*wt & 0x80) != 0) {
        if (nbytes >= len)
            return SYSERR;
        *buff++ = 0;
        nbytes++;
    } else if (src->sign < 0 && (*wt & 0x80) == 0) {
        if (nbytes >= len)
            return SYSERR;
        *buff++ = -1;
        nbytes++;
    } else if (nbytes > len) {
        return SYSERR;
    }

    *length = (uint32) htonl(nbytes);

    write_bigendian_mpint(buff, src, towrite);

    return nbytes + 4;
}

bool8 copy_mpprime(MPPRIME* dst, MPPRIME* src) {
    return copy_mpint(&(dst->num), &(src->num)) &&
        copy_mpint(&(dst->inv), &(src->inv)) &&
        copy_mpint(&(dst->r_square), &(src->r_square));
}

/*
 * Copy the src into the dst. (Extend the destination if necessary)
 */
bool8 copy_mpint(MPINT* dst, MPINT* src) {
    WORD *wtdst, *wtsrc;
    int32 i;

    if (dst->size < src->size) {
        if (dst->size > 0)
            freemem((char *)dst->val, dst->size*sizeof(HALF_WORD));

        dst->val = (HALF_WORD *)getmem(src->size*sizeof(HALF_WORD));
        if (dst->val == (HALF_WORD *)SYSERR)
            return FALSE;

        dst->size = src->size;
    }

    wtsrc = (WORD *)src->val;
    wtdst = (WORD *)dst->val;

    /* copy value */
    for (i = 0 ; i < (src->order >> 1) ; i++)
        *wtdst++ = *wtsrc++;

    if ((src->order & 0x1) != 0) {
        *wtdst = 0;
        *((HALF_WORD *) wtdst) = *((HALF_WORD *) wtsrc);
        wtdst++;
    }

    /* erase upper part if any. */
    for (; i < (dst->size >> 1) ; ++i)
        *wtdst++ = 0;

    /* Extra halfword */
    *((HALF_WORD *) wtdst) = 0;

    /* copy order and sign. */
    dst->sign = src->sign;
    dst->order = src->order;

    return TRUE;
}

/*
 * this function extracts a subpart of the mpint as a "ghost".
 * there are no memory copies, Careful!!! with great power comes grat responsibilities.
 */
bool8 subvalghost_mpint(MPINT* dst, MPINT* src, int32 start, int32 length) {

    if (start + length > src->order) {
        length = src->order - start;
    }

    /* Some checks. */
    if (start < 0 || length < 0) {
        return FALSE;
    }

    dst->size = 0;
    dst->val = src->val + start;
    dst->order = length;
    while (dst->order > 0 && dst->val[dst->order - 1] == 0)
        dst->order--;

    dst->sign = 1;

    return TRUE;
}


/*
 * Copy a part of the src into dst.
 */
bool8 subval_mpint(MPINT* dst, int32 offset, MPINT* src, int32 start, int32 length) {
    WORD *wtdst, *wtsrc;
    HALF_WORD *hwtdst;
    int32 i, original;

    if (length == 0 || start >= src->order)
        return TRUE;

    if (start + length > src->order) {
        length = src->order - start;
    }

    /* Some checks. */
    if (offset < 0 || start < 0 || length < 0) {
        return FALSE;
    }

    if (dst->size <= offset + length) { // Need to extend the dst

        /* Was not null */
        if (dst->size > 0) {

            /* Compute new size */
            original = dst->size;
            dst->size = roundm(offset + length, MPINT_HWINIT) + 1;

            hwtdst = (HALF_WORD *)getmem(dst->size*sizeof(HALF_WORD));
            if (hwtdst == (HALF_WORD *)SYSERR) {
                dst->size = original;
                return FALSE;
            }

            /* we have to copy from 0 to offset (WORD by WORD)*/
            wtdst = (WORD *)hwtdst;
            wtsrc = (WORD *)dst->val;
            for (i = 0 ; i < ((offset + 1) >> 1) ; ++i)
                *wtdst++ = *wtsrc++;

            freemem((char *)dst->val, original*sizeof(HALF_WORD));
            dst->val = hwtdst;

            /* And erase from offset + length to dst->length */
            hwtdst = hwtdst + offset + length;
            for (i = offset + length ; i < dst->size ; ++i)
                *hwtdst++ = 0;

        } else {
            /* Was null */
            if (word_mpint(dst, 0, offset + length - 1) == FALSE)
                return FALSE;
        }
    }

    /* Copy the value. */
    wtdst = (WORD *)(dst->val + offset);
    wtsrc = (WORD *)(src->val + start);
    for (i = 0 ; i < (length >> 1) ; ++i)
        *wtdst++ = *wtsrc++;

    if ((length & 0x1) != 0) {
        *((HALF_WORD *)wtdst) = *((HALF_WORD *)wtsrc);
    }


    /* update order */
    if (dst->order < offset + length) {
        dst->order = offset + length;
        hwtdst = dst->val + offset + length - 1;
        while (0 < dst->order && *hwtdst-- == 0)
            dst->order--;
    }

    return TRUE;
}

/*
 * Update the order of the mpint.
 * return FALSE if the order is equal to the size+1 (arrayis full)
 * or TRUE otherwise.
 */
bool8 uporder_mpint(MPINT *n) {
    int32 i;
    HALF_WORD *wt;

    i = n->size;
    wt = n->val + n->size - 1;
    while (i > 0 && *wt-- == 0)
        i--;

    n->order = i;

    return i == n->size ? FALSE : TRUE;
}

/* Only for testing purpose */

byte hextobin(char v) {
    return v > '9' ? (byte)v - 0x57 : v - '0';
}

void swap_mpint(MPINT* v1, MPINT* v2) {

    HALF_WORD* tmpval;
    uint32 tmpint;

    tmpval = v1->val;
    v1->val = v2->val;
    v2->val = tmpval;

    tmpint = v1->order;
    v1->order = v2->order;
    v2->order = tmpint;

    tmpint = v1->size;
    v1->size = v2->size;
    v2->size = tmpint;

    tmpint = v1->sign;
    v1->sign = v2->sign;
    v2->sign = tmpint;
}

bool8 parse_mpint(MPINT* n, char* val, int32 length) {

    char *wtval;
    byte *wtn, bl, bh;
    int32 i, new_size;

    /* Check sign */
    wtval = val + length - 1;
    if (*val == '-') {
        n->sign = -1;
        length--;
    }

    new_size = ((length * 4 + MPINT_INIT - 1)/MPINT_INIT) * MPINT_HWINIT;

    /* Create the mpint */
    if (word_mpint(n, n->sign, new_size) == FALSE) {
#ifdef MPINT_DEBUG
        kprintf("\nparse_mpint: fail initialisation\n");
#endif
        return FALSE;
    }

    /* parse the input string */
    wtn = (byte *)n->val;
    for (i = 0 ; i < (length >> 1) ; ++i) {
        bl = *wtval--;
        bh = *wtval--;
        *wtn++ = (hextobin(bl) & 0xF) | (0xF0 &  (hextobin(bh) << 4));
    }
    if ((length & 0x1) != 0) {
        *wtn = hextobin(*wtval) & 0xF;
    }

    /* Update order */
    return uporder_mpint(n);
}

void print_mpint(MPINT* n) {
    int32 i;

    printf("\nSize %d, order %d\n%s(%04x)", n->size, n->order, n->sign < 0 ? "-" : "", n->size > 0 ? n->val[n->order] : 0xcafe);
    for (i = n->order-1 ; i >= 0 ; i--)
        printf("%04x", n->val[i]);
    printf("\n");
}

void print_mpintbin(MPINT* n) {
    int32 i, j;

    printf("\nSize %d, order %d\n%s(%04x)", n->size, n->order, n->sign < 0 ? "-" : "", n->val[n->order]);
    for (i = n->order-1 ; i >= 0 ; i--) {
        for (j = 0x8000 ; j > 0 ; j >>= 1) {
            printf("%d", (n->val[i] & j) != 0 ? 1 : 0);
        }
        printf(" ");
    }
    printf("\n");
}

void to_static_mpint(MPINT* n) {
    int32 i;

    for (i = 0 ; i < n->order ; i++)
        printf("0x%04x, ", n->val[i]);
    printf("\n");
}
