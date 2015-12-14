#include <xinu.h>

bool8 kmul_mpint(MPINT *dst, MPINT *v1, MPINT *v2, bool8 trunc) {
	return FALSE;
}

/* http://www.usna.edu/Users/cs/roche/research/papers/ams09.pdf */
/* dst = dst + (v1_0 + v1_0) * v2 */
//bool8 kmul_subroutine_A(HALF_WORD* dst, HALF_WORD* v1_0, HALF_WORD* v1_1, HALF_WORD* v2, int32 size) {
//
//	int32 half_size, tq_size;
//	int32 i, j;
//	WORD carry;
//
//	kprintf("call %d\n", size);
//
//	/* Base case */
//	if (size == 1) {
//		*((uint32 *) dst) += (*v1_0 + *v1_1) * *v2;
//		return TRUE;
//	}
//
//	half_size = size >> 1;
//	tq_size = half_size + size;
//
//	/* Step 1 */
//	kprintf("Step 1\n");
//	/* dst_4 */
//	carry = 0;
//	for (i = 0 ; i < half_size ; i++) {
//		carry += v1_0[i] + v1_0[i + half_size] + v1_1[i] + v1_1[i + half_size];
//		dst[tq_size + i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//	}
//	if (carry)
//		kprintf("? (0) %x\n", carry);
//
//	/* dst_1 += dst_0; */
//	carry = 0;
//	for ( ; i < size ; i++) {
//		carry += dst[i - half_size] + dst[i];
//		dst[i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//	}
//
//	if (carry)
//		kprintf("? (1) %x\n", carry);
//
//	/* Step 2 */
//	kprintf("Step 2\n");
//	kmul_subroutine_A(dst + half_size, v2, v2 + half_size, dst + tq_size, half_size);
//
//	/* Step 3 */
//	kprintf("Step 3\n");
//	carry = 0;
//	for (i = half_size ; i < size ; i++) {
//		carry += dst[i] + dst[half_size + i];
//		dst[size + i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//	}
//
//	if (carry)
//		kprintf("? (2) %x\n", carry);
//
//	/* Step 4 */
//	kprintf("Step 4\n");
//	kmul_subroutine_A(dst, v1_0, v1_1, v2, half_size);
//
//	/* step 5 */
//	kprintf("Step 5\n");
//	carry = 0;
//	for (i = size ; i < tq_size ; i++) {
//		carry = dst[i] - dst[i - half_size] - carry;
//		dst[i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//		carry &= 0x1;
//	}
//	if (carry)
//		kprintf("? (3) %x\n", carry);
//
//	carry = 0;
//	for (i = 0 ; i < half_size ; i++) {
//		carry = dst[tq_size + i] - dst[i] - carry;
//		dst[half_size + i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//		carry &= 0x1;
//	}
//	if (carry)
//		kprintf("? (4) %x\n", carry);
//
//	/* step 6 */
//	kprintf("Step 6\n");
//	kmul_subroutine_A(dst + size, v1_0 + half_size, v1_1 + half_size, v2 + half_size, half_size);
//
//	/* step 7 */
//	kprintf("Step 7\n");
//	carry = 0;
//	for (i = half_size ; i < size ; i++) {
//		carry = dst[i] - dst[i + half_size] - carry;
//		dst[i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//		carry &= 0x1;
//	}
//	if (carry)
//		kprintf("? (5) %x\n", carry);
//
//	carry = 0;
//	for (i = size ; i < tq_size ; i++) {
//		carry = dst[i] - dst[i + half_size] - carry;
//		dst[i] = carry & HW_MASK;
//		carry >>= HW_BITS;
//		carry &= 0x1;
//	}
//	if (carry)
//		kprintf("? (6) %x\n", carry);
//
//	return TRUE;
//}
//
//void test_kmul() {
//
//	MPINT v1_0, v1_1, v2, dst;
//
//	init_mpint(&v1_0);
//	init_mpint(&v1_1);
//	init_mpint(&v2);
//	init_mpint(&dst);
//
//	parse_mpint(&v1_0, "55555555555555555555555555555555", 32);
//	parse_mpint(&v1_1, "55555555555555555555555555555555", 32);
//	parse_mpint(&v2, "11111111111111111111111111111111", 32);
//	word_mpint(&dst, 0, 1);
//
//	kprintf("%d, %d, %d\n", v1_0.order, v1_1.order, v2.order);
//
//	kmul_subroutine_A(dst.val, v1_0.val, v1_1.val, v2.val, 8);
//
//	dst.order = 16;
//	print_mpint(&dst);
//
//	clear_mpint(&v1_0);
//	clear_mpint(&v1_1);
//	clear_mpint(&v2);
//	clear_mpint(&dst);
//}
