#include <xinu.h>
#include <stdio.h>

char strOK[] = "OK";
char strFAIL[] = "FAIL";

HALF_WORD val1[] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5381, 0xECE6, 0x6651, 0x4928, 0x1FE6, 0x7C4B, 0x2411, 0xAE9F, 0x9FA5, 0x5A89, 0x6BFB, 0xEE38, 0xB7ED, 0xF406, 0x5CB6, 0x0BFF, 0xED6B, 0xA637, 0x42E9, 0xF44C, 0x7EC6, 0x625E, 0xB576, 0xE485, 0xC245, 0x6D51, 0x356D, 0x4FE1, 0x1437, 0xF25F, 0x0A6D, 0x302B, 0x431B, 0xCD3A, 0x19B3, 0xEF95, 0x04DD, 0x8E34, 0x0879, 0x514A, 0x9B22, 0x3B13, 0xBEA6, 0x020B, 0xCC74, 0x8A67, 0x4E08, 0x2902, 0x1CD1, 0x80DC, 0x628B, 0xC4C6, 0xC234, 0x2168,0xDAA2, 0xC90F, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000};
MPINT p1val = {.val = val1, .order = 64, .size = 65, .sign = 1};

// R2p1val = (2^1024)^2 mod p1val
HALF_WORD R2p1valval[] = { 0x1f42, 0x6003, 0xf055, 0x86b8, 0xe3b2, 0xc7b5, 0x1890, 0xc52e, 0x172a, 0x1893, 0x7232, 0x7cae, 0x89e3, 0x9699, 0x58a4, 0x55da, 0x09b5, 0x267f, 0xfbfa, 0x1f69, 0x0803, 0x01b8, 0x5d0d, 0x64e9, 0x1c6d, 0x00c5, 0x5e84, 0x0d95, 0x37f2, 0x63e6, 0xaea8, 0xa513, 0x158c, 0x74ce, 0x1526, 0x0cb0, 0x5e02, 0xeb65, 0x91e4, 0xe127, 0x41c7, 0x3100, 0xd1e7, 0x0092, 0x80c2, 0xc388, 0x8fec, 0x010d, 0x648d, 0xa6b7, 0xe34c, 0xb990, 0x16fb, 0xe986, 0x67cc, 0x4325, 0x698d, 0x5a74, 0x28c7, 0x0345, 0x3ead, 0x3c30, 0xc282, 0x6415, 0x0000};
MPINT R2p1val = {.val = R2p1valval, .order = 64, .size = 65, .sign = 1};

// INVp1val = -(p1val)^(-1) mod 2^1024
HALF_WORD INVp1valval[] = {0x0001, 0x0000, 0x0000, 0x0000, 0x5382, 0xece6, 0x6651, 0x4928, 0xadea, 0x5520, 0x0d87, 0x3ac5, 0x59df, 0x2b7d, 0xbf9d, 0x240e, 0x3465, 0xcf12, 0x8201, 0xbe54, 0x47cd, 0x5c7f, 0x20ec, 0xe6fd, 0x2f8d, 0xa448, 0xd49d, 0xbf28, 0x6aba, 0x23b3, 0xb2d8, 0x4c32, 0x0c37, 0x45b0, 0x6eb3, 0x17f2, 0xe43c, 0x8112, 0x7b1d, 0x80f6, 0xa3a4, 0x1c49, 0x90fd, 0x57e4, 0xba70, 0xfd07, 0xba88, 0x4396, 0x9853, 0xd1f2, 0xe82c, 0xe792, 0x3c1f, 0x1a26, 0x9b3e, 0x5fc4, 0xfb98, 0x1084, 0xa0bd, 0x33c2, 0xbef2, 0xaf01, 0xf0e0, 0xc486, 0x0000};
MPINT INVp1val = {.val = INVp1valval, .order = 64, .size = 65, .sign = 1};

MPPRIME prime1 = {
	.num = {.val = val1, .order = 64, .size = 65, .sign = 1},
	.inv = {.val = INVp1valval, .order = 64, .size = 65, .sign = 1},
	.r_square = {.val = R2p1valval, .order = 64, .size = 65, .sign = 1}
};

void test_parsempint() {

	char astr[] = "-6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7";
	char bstr[] = "a6ac6b647f196ff62ba772bf59dcc541b209763baa515a5dd62ff591c600bb3cd5be3358392022ee2fe1e9510fa9f77ee61055e2fb8934a40a276d4f803bc74fcb7b972ff8ae2539448d96c0a85f85950ab7d5ff7ff41402ce4132234fe7144ff2c72743bb0c393699dee4b73e0f2dcdba792f01aa2dd5290c2e86fd8f8fd92b0000000000000000000";
	char cstr[] = "0";
	char dstr[] = "7bce1234";

	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* c = new_mpint();
	MPINT* d = new_mpint();
	MPINT* e = new_mpint();
	MPINT* f = new_mpint();

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(c, cstr, 1);
	parse_mpint(d, dstr, 8);
	word_mpint(e, 0, 1);
	word_mpint(f, 0x7bce1234,1);

	print_mpint(a);
	print_mpint(b);
	kprintf("\n");
	kprintf("parse, Test 1: %s\n", cmp_mpint(c, e) == 0 ? strOK : strFAIL);
	kprintf("parse, Test 2: %s\n", cmp_mpint(d, f) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(c);
	delete_mpint(d);
	delete_mpint(e);
	delete_mpint(f);
}

void test_cmpmpint() {

	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* c = new_mpint();
	MPINT* d = new_mpint();
	MPINT* e = new_mpint();
	MPINT* f = new_mpint();

	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cd028f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22";
	char bstr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22";
	char cstr[] = "a6ac6b647f196ff62ba772bf59dcc541b209763baa515a5dd62ff591c600bb3cd5be3358392022ee2fe1e9510fa9e77ee61055e2fb8934a40a276d4f803bc74fcb7b972ff8ae2539448d96c0a85f85950ab7d5ff7ff41402ce4132234fe7144ff2c72743bb0c393699dee4b73e0f2dcdba792f01aa2dd5290c2e86fd8f8fd92b0000";
	char dstr[] = "a6ac6b647f196ff62ba772bf59dcc541b209763baa515a5dd62ff591c600bb3cd5be3358392022ee2fe1e9510fa9f77ee61055e2fb8934a40a276d4f803bc74fcb7b972ff8ae2539448d96c0a85f85950ab7d5ff7ff41402ce4132234fe7144ff2c72743bb0c393699dee4b73e0f2dcdba792f01aa2dd5290c2e86fd8f8fd92b0000";
	char estr[] = "1";
	char fstr[] = "-ff7520";

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(d, dstr, strnlen(dstr, 10000));
	parse_mpint(e, estr, strnlen(estr, 10000));
	parse_mpint(f, fstr, strnlen(fstr, 10000));

	kprintf("cmp, Test 1: %s\n", unsigncmp_mpint(a, b) == -1 ? strOK : strFAIL);
	kprintf("cmp, Test 2: %s\n", unsigncmp_mpint(a, a) == 0 ? strOK : strFAIL);
	kprintf("cmp, Test 3: %s\n", unsigncmp_mpint(c, a) == 1 ? strOK : strFAIL);
	kprintf("cmp, Test 4: %s\n", unsigncmp_mpint(b, d) == -1 ? strOK : strFAIL);
	kprintf("cmp, Test 5: %s\n", cmp_mpint(e, f) == 1 ? strOK : strFAIL);
	kprintf("cmp, Test 6: %s\n", unsigncmp_mpint(e, f) == -1 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(c);
	delete_mpint(d);
	delete_mpint(e);
	delete_mpint(f);
}

void test_sum() {

	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* c = new_mpint();
	MPINT* d = new_mpint();
	MPINT* e = new_mpint();
	MPINT* f = new_mpint();
	MPINT* g = new_mpint();
	MPINT* h = new_mpint();

	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char bstr[] = "a6ac6b647f196ff62ba772bf59dcc541b209763baa515a5dd62ff591c600bb3cd5be3358392022ee2fe1e9510fa9f77ee61055e2fb8934a40a276d4f803bc74fcb7b972ff8ae2539448d96c0a85f85950ab7d5ff7ff41402ce4132234fe7144ff2c72743bb0c393699dee4b73e0f2dcdba792f01aa2dd5290c2e86fd8f8fd92b";
	char cstr[] = "113ccac60fee65107cbe758714cac55b584942a70dbaec3213f96034d24fde018234802e8f2dddfbd3eedbf1ec2397319ea6012a10ba23dd12bc63e2e8d2ae728d278826c9afedb653450369e104a5876c330e2faea821d45013dec5d75ec83ac22ecd17e8023157f15c6dd17ec20db3da420efd8a7200799e6308f38b3520677";
	char dstr[] = "-398c2a67ff4c8ee48b678d0d670d34cddf7ec20678f3f19a6cc9e7d667039661883463c77f62661f20d613835d1a7be3e1c09924eb702b76e8889c70734ca776c47eabf3565d6f0d54caf6e34074b2b3523ec90415660ac09b4477e929e1a4f3c2a17d08f5f55cee1df6ec568ffd805dd0d16e2aad3ba2b8322c7ec26bcdabdf";
	char gstr[] = "fbcdabdf";
	char hstr[] = "113ccac60fee65107cbe758714cac55b584942a70dbaec3213f96034d24fde018234802e8f2dddfbd3eedbf1ec2397319ea6012a10ba23dd12bc63e2e8d2ae728d278826c9afedb653450369e104a5876c330e2faea821d45013dec5d75ec83ac22ecd17e8023157f15c6dd17ec20db3da420efd8a7200799e6308f37b7845a98";


	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(d, dstr, strnlen(dstr, 10000));
	parse_mpint(e, astr, strnlen(astr, 10000));
	parse_mpint(f, astr, strnlen(astr, 10000));
	parse_mpint(g, gstr, strnlen(gstr, 10000));
	parse_mpint(h, hstr, strnlen(hstr, 10000));

	kprintf("sum, Test 1: %s\n", add_mpint(a, b) == TRUE && cmp_mpint(a, c) == 0 ? strOK : strFAIL);
	kprintf("sum, Test 2: %s\n", sub_mpint(f, e) == TRUE && iszero_mpint(f) == 1 ? strOK : strFAIL);
	kprintf("sum, Test 3: %s\n", sub_mpint(e, b) == TRUE && cmp_mpint(e, d) == 0 ? strOK : strFAIL);
	kprintf("sum, Test 4: %s\n", diff_mpint(g, c, -1) == TRUE && cmp_mpint(g, h) == 0 ? strOK : strFAIL);
	kprintf("\n");

	print_mpint(g);
	print_mpint(h);

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(c);
	delete_mpint(d);
	delete_mpint(e);
	delete_mpint(f);
	delete_mpint(g);
	delete_mpint(h);
}

void test_mulmpint() {
	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char bstr[] = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000d92b";
	char abtstr[] = "ab80f2be2231becc146f6bb5ba4f70ebf00d5bc7787e3d2a35c821fd88c3158fd7178ac107ea3a888f3845e89de029b15061cf91ecfbcc1444ba276dde2950608fe1e8c5adf9d79d3f3387468132c14898564b8d4bb4ab56ca4e91fefe4ed94c039f68ef7b9071347180abca38fbfedb71a3a085c97021c05751833e9a0307c4";
	char abstr[] = "5c92ab80f2be2231becc146f6bb5ba4f70ebf00d5bc7787e3d2a35c821fd88c3158fd7178ac107ea3a888f3845e89de029b15061cf91ecfbcc1444ba276dde2950608fe1e8c5adf9d79d3f3387468132c14898564b8d4bb4ab56ca4e91fefe4ed94c039f68ef7b9071347180abca38fbfedb71a3a085c97021c05751833e9a0307c4";
	char cstr[] = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000d5290c2e86fd8f8fd92b";
	char actstr[] = "a84012fbb8b8f68ce90bd5fa4b051528c550c0cad0e61296e8c9dd84159d0d5d65bae86043becc50f93b57917c06370e36ff00b1a2bdacb4f018432b716265829edbb8ecef31a18d92c665296c61376126392c43ec2a6ded192dbbb0ebc4dace821545e2a5324dd2e15f5f8618fd1c7ff874ec9b0ea926d5a798801f5b7707c4";
	char acstr[] = "5add556dd7286a8701d8a84012fbb8b8f68ce90bd5fa4b051528c550c0cad0e61296e8c9dd84159d0d5d65bae86043becc50f93b57917c06370e36ff00b1a2bdacb4f018432b716265829edbb8ecef31a18d92c665296c61376126392c43ec2a6ded192dbbb0ebc4dace821545e2a5324dd2e15f5f8618fd1c7ff874ec9b0ea926d5a798801f5b7707c4";
	char fstr[] = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	char fftstr[] = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
	char ffstr[] = "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";

	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* abt = new_mpint();
	MPINT* ab = new_mpint();
	MPINT* myab = new_mpint();
	MPINT* c = new_mpint();
	MPINT* act = new_mpint();
	MPINT* ac = new_mpint();
	MPINT* myac = new_mpint();
	MPINT* f = new_mpint();
	MPINT* fft = new_mpint();
	MPINT* ff = new_mpint();
	MPINT* myff = new_mpint();

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(abt, abtstr, strnlen(abtstr, 10000));
	parse_mpint(ab, abstr, strnlen(abstr, 10000));
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(act, actstr, strnlen(actstr, 10000));
	parse_mpint(ac, acstr, strnlen(acstr, 10000));
	parse_mpint(f, fstr, strnlen(fstr, 10000));
	parse_mpint(fft, fftstr, strnlen(fftstr, 10000));
	parse_mpint(ff, ffstr, strnlen(ffstr, 10000));


	kprintf("mul, Test 1: %s\n", mul_mpint(myab, a, b, TRUE) == TRUE && cmp_mpint(myab, abt) == 0 ? strOK : strFAIL);
	word_mpint(myab, 0,1);
	kprintf("mul, Test 2: %s\n", mul_mpint(myab, a, b, FALSE) == TRUE && cmp_mpint(myab, ab) == 0 ? strOK : strFAIL);
	kprintf("mul, Test 3: %s\n", mul_mpint(myac, a, c, TRUE) == TRUE && cmp_mpint(myac, act) == 0 ? strOK : strFAIL);
	word_mpint(myac, 0,1);
	kprintf("mul, Test 4: %s\n", mul_mpint(myac, a, c, FALSE) == TRUE && cmp_mpint(myac, ac) == 0 ? strOK : strFAIL);
	kprintf("mul, Test 5: %s\n", mul_mpint(myff, f, f, TRUE) == TRUE && cmp_mpint(myff, fft) == 0 ? strOK : strFAIL);
	word_mpint(myff, 0,1);
	kprintf("mul, Test 6: %s\n", mul_mpint(myff, f, f, FALSE) == TRUE && cmp_mpint(myff, ff) == 0 ? strOK : strFAIL);
	word_mpint(myff, 0,1);
	kprintf("mul, Test 7: %s\n", square_mpint(myff, f) == TRUE && cmp_mpint(myff, ff) == 0 ? strOK : strFAIL);
	word_mpint(myab, 0,1);
	word_mpint(myac, 0,1);
	kprintf("mul, Test 8: %s\n", square_mpint(myab, a) == TRUE && mul_mpint(myac, a, a, FALSE) == TRUE && cmp_mpint(myac, myab) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(abt);
	delete_mpint(ab);
	delete_mpint(myab);
	delete_mpint(c);
	delete_mpint(act);
	delete_mpint(ac);
	delete_mpint(myac);
	delete_mpint(f);
	delete_mpint(fft);
	delete_mpint(ff);
	delete_mpint(myff);
}

void test_shiftmpint() {

	char fstr[] = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	char fr25str[] = "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	char fl25str[] = "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe000000";
	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char ar4str[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4";
	char al3str[] = "3690207e3fe67088d01ff2d8f967c839e9455a1a98aeb461b4b306ddaf7e926da6c4e7c85cdede678785eae6d947bdcd8227de5f080c849690cf686f86778fec837e759e51285b15f7e14feeb3f56970dc3c867db54704a1197e5d1d1302b7ae1812d51d628b6e243df3fc305708d6b7f4d3e06b7e7919386d01041d91e116a60";
	char al53str[] = "da4081f8ff99c223407fcb63e59f20e7a515686a62bad186d2cc1b76bdfa49b69b139f21737b799e1e17ab9b651ef736089f797c2032125a433da1be19de3fb20df9d67944a16c57df853fbacfd5a5c370f219f6d51c128465f974744c0adeb8604b54758a2db890f7cff0c15c235adfd34f81adf9e464e1b404107647845a980000000000000";



	MPINT* a1 = new_mpint();
	MPINT* a2 = new_mpint();
	MPINT* a3 = new_mpint();
	MPINT* a4 = new_mpint();
	MPINT* ar4 = new_mpint();
	MPINT* al53 = new_mpint();
	MPINT* al3 = new_mpint();
	MPINT* f1 = new_mpint();
	MPINT* f2 = new_mpint();
	MPINT* fr25 = new_mpint();
	MPINT* fl25 = new_mpint();

	parse_mpint(a1, astr, strnlen(astr, 10000));
	parse_mpint(a2, astr, strnlen(astr, 10000));
	parse_mpint(a3, astr, strnlen(astr, 10000));
	parse_mpint(a4, astr, strnlen(astr, 10000));
	parse_mpint(ar4, ar4str, strnlen(ar4str, 10000));
	parse_mpint(al53, al53str, strnlen(al53str, 10000));
	parse_mpint(al3, al3str, strnlen(al3str, 10000));
	parse_mpint(f1, fstr, strnlen(fstr, 10000));
	parse_mpint(f2, fstr, strnlen(fstr, 10000));
	parse_mpint(fr25, fr25str, strnlen(fr25str, 10000));
	parse_mpint(fl25, fl25str, strnlen(fl25str, 10000));


	kprintf("shift, Test 1: %s\n", rshift_mpint(f1, 25) == TRUE && cmp_mpint(f1, fr25) == 0 ? strOK : strFAIL);
	kprintf("shift, Test 2: %s\n", lshift_mpint(f2, 25) == TRUE && cmp_mpint(f2, fl25) == 0 ? strOK : strFAIL);
	kprintf("shift, Test 3: %s\n", rshift_mpint(a1, 4) == TRUE && cmp_mpint(a1, ar4) == 0 ? strOK : strFAIL);
	kprintf("shift, Test 4: %s\n", lshift_mpint(a2, 53) == TRUE && cmp_mpint(a2, al53) == 0 ? strOK : strFAIL);
	kprintf("shift, Test 5: %s\n", safelshift_mpint(a3, 3) == TRUE && cmp_mpint(a3, al3) == 0 ? strOK : strFAIL);
	kprintf("shift, Test 6: %s\n", rshift_mpint(a3, 3) == TRUE && cmp_mpint(a3, a4) == 0 ? strOK : strFAIL);
	kprintf("shift, Test 7: %s\n", rshifthw_mpint(a3, 6) == TRUE && rshift_mpint(a4, 6*16) == TRUE && cmp_mpint(a3, a4) == 0 ? strOK : strFAIL);
	kprintf("\n");


	delete_mpint(a1);
	delete_mpint(a2);
	delete_mpint(a3);
	delete_mpint(a4);
	delete_mpint(ar4);
	delete_mpint(al53);
	delete_mpint(al3);
	delete_mpint(f1);
	delete_mpint(f2);
	delete_mpint(fr25);
	delete_mpint(fl25);
}

void test_montgomery_mpint() {

	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char aRstr[] = "1d62792a31472bc9bbd0c956c7db56463c5264d04ea46b94822364462df291a92df59bd057e47ceaf955f25cddb5368b2f8a4d185092b5e4b969c55ff3b2df4e0172e48118915dea210bc6e6e1edf8a082b505942794e5f4439b946659304f136fad966b6926d015edc9d52bcb16e2d070003299b5372bd59c17f649cf51e5ab";
	char Rstr[] = "36f0255dde973dcb3b399d747f23e32ed6fdb1f77598338bfdf44159c4ec64ddaeb5f78671cbfb22106ae64c32c5bce4cfd4f5920da0ebc8b01eca9292ae3dba1b7a4a899da181390bb3bd1659c81294f400a3490bf9481211c79404a576605a5160dbee83b4e019b6d799ae1319ac7e0000000000000001";
	char bstr[] = "f9fbf8bba82e2de37effaac3c523a3e1ea102fc5ab19bd36e71fb276c031c896762e033b15da36e82e0d0ad4da0b2b859ebb95d537e1c0016d0bd63efe95ea1aaf4b19b3953eb86be62757841e2af96d6223f904a5b49ae07cfa8acf9d64c3268ce00909533a05e39b357530a91fe9084d42200b5847335f1f90fefe7e444156";
	char abstr[] ="c684d5aa03895f671ccd6dd24303eb1cea3236458ee9f0b3080c8aacb0381b6b8ea9051a5d5bb3f8625c8e83854bc433b8df587caa64421adea2e5a36f2413ba1468917a1d3a07d9012a8b19dcf6cc1a5b602f6d175380f86268f5aad06496d1e1a987d8e7c9d3f190f31bd694ad7d2244927877fb196106d1c381b6b1cb4c7b";
	char xRstr[] = "4e51213ee3495a2d81f47491733c61fbf6f460e3e2daadc2c50970a10198d91295d027f66ac9d38dec241557304986adc2513af1d63f7483898b4f0b0af32bf5cf5ab984539530bcec8f4069fb27824a8554d805d366f3855f3c1044597932cf6bc396a181222a2acf2955c1e48cb591a27272f97a0ef89e26ae3ffb44ba559f";
	char yRstr[] = "682d4bfa9cf40a4795623f88b3dcf51db5ccd31b4c76a1c356bf8a3f0a3823f0a292958251a49c3ca7e754cfdf59d13a8f25efb1cfa138d9575dbef351212aba583394644818427becca32efa24add290c63c01d313426d040c38e70f15d440ec88680518f43041ff578af343c25c49a87cb4805073cb2514262543471a572fe";
	char xyRstr[] = "3abddf0189dad210560aafd4e3d29ca084bc726a5d3ec6d09307c298d3ac9e346c386f12bdcc76b8c383fc8515287d78a2c946e1610a47e82d7f457e72436fe6299d0e09c24f69386e6d7c4a08dc3acb71db0866b66359659a768aee50ba38de92e27602ab8feb00c7e691304086c5254b9e1fc1ec33a2e06afdc2d99de28148";

	MPINT* a1 = new_mpint();
	MPINT* a2 = new_mpint();
	MPINT* a3 = new_mpint();
	MPINT* a4 = new_mpint();
	MPINT* b = new_mpint();
	MPINT* ab = new_mpint();
	MPINT* aR1 = new_mpint();
	MPINT* aR2 = new_mpint();
	MPINT* v1 = new_mpint();
	MPINT* v2 = new_mpint();
	MPINT* R1 = new_mpint();
	MPINT* R2 = new_mpint();
	MPINT* xR = new_mpint();
	MPINT* yR = new_mpint();
	MPINT* xyR = new_mpint();

	parse_mpint(a1, astr, strnlen(astr, 10000));
	parse_mpint(a2, astr, strnlen(astr, 10000));
	parse_mpint(a3, astr, strnlen(astr, 10000));
	parse_mpint(a4, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(ab, abstr, strnlen(abstr, 10000));
	parse_mpint(aR1, aRstr, strnlen(aRstr, 10000));
	parse_mpint(aR2, aRstr, strnlen(aRstr, 10000));
	word_mpint(v1, 1, 1);
	parse_mpint(R1, Rstr, strnlen(Rstr, 10000));
	word_mpint(v2, 1, 1);
	parse_mpint(R2, Rstr, strnlen(Rstr, 10000));
	parse_mpint(xR, xRstr, strnlen(xRstr, 10000));
	parse_mpint(yR, yRstr, strnlen(yRstr, 10000));
	parse_mpint(xyR, xyRstr, strnlen(xyRstr, 10000));

	kprintf("montgomery, Test 1: %s\n", montgomery_mpint(a1, &R2p1val, &p1val, &INVp1val) == TRUE && cmp_mpint(a1, aR1) == 0 ? strOK : strFAIL);
	kprintf("montgomery, Test 2: %s\n", montgomery_inv_mpint(aR2, &p1val, &INVp1val) == TRUE && cmp_mpint(a2, aR2) == 0 ? strOK : strFAIL);
	kprintf("montgomery, Test 3: %s\n", montgomery_mpint(v1, &R2p1val, &p1val, &INVp1val) == TRUE && cmp_mpint(v1, R1) == 0 ? strOK : strFAIL);
	kprintf("montgomery, Test 4: %s\n", montgomery_inv_mpint(R2, &p1val, &INVp1val) == TRUE && cmp_mpint(R2, v2) == 0 ? strOK : strFAIL);
	kprintf("montgomery, Test 5: %s\n", montgomery_mpint(a3, a4, &p1val, &INVp1val) == TRUE && montgomery_square_mpint(a4, &p1val, &INVp1val) == TRUE && cmp_mpint(a1, aR1) == 0 ? strOK : strFAIL);
	kprintf("montgomery, Test 6: %s\n",  montgomery_mpint(b, &R2p1val, &p1val, &INVp1val) == TRUE && \
			montgomery_mpint(b, aR1, &p1val, &INVp1val) == TRUE && \
			montgomery_inv_mpint(b, &p1val, &INVp1val) == TRUE && \
			cmp_mpint(b, ab) == 0 ? strOK : strFAIL);
	kprintf("montgomery, Test 7: %s\n", montgomery_mpint(xR, yR, &((DH_G1.p).num), &((DH_G1.p).inv)) == TRUE && cmp_mpint(xR, xyR) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a1);
	delete_mpint(aR1);
	delete_mpint(a2);
	delete_mpint(aR2);
	delete_mpint(a3);
	delete_mpint(a4);
	delete_mpint(b);
	delete_mpint(ab);
	delete_mpint(v1);
	delete_mpint(R1);
	delete_mpint(v2);
	delete_mpint(R2);
	delete_mpint(xR);
	delete_mpint(yR);
	delete_mpint(xyR);
}

void test_pow_mpint() {
	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char bstr[] = "10";
	char cstr[] = "a6ac6b647f196ff62ba772bf59dcc541b209763baa515a5dd62ff591c600bb3cd5be3358392022ee2fe1e9510fa9f77ee61055e2fb8934a40a276d4f803bc74fcb7b972ff8ae2539448d96c0a85f85950ab7d5ff7ff41402ce4132234fe7144ff2c72743bb0c393699dee4b73e0f2dcdba792f01aa2dd5290c2e86fd8f8fd92b";
	char dstr[] = "10000000000000000000000000";
	char aP10str[] = "f96a37c0c2fb864b860f4c644034b64a88b2f98b56ef52b495213624b1d14aa20d28976fd15552aac55daf6799bf930eb84ac8b9701c6eba4492153a7b426b520a3a533ebcb98b96bc0d1f167f03ac97f11dc557d1477b999e1a4496cc8b1c65401183781b0c808ca2fc81fd63cc76899ec2107454e7b2c1c6179877f8f76751";
	char aP3str[] = "7c24370b61ddbd13c50722a087172ef2baddb68879be665a0ed24740d1c6257bc74ab77b0d9b6adcc24d1dfd02cdef73fef6895338f92dd82fc898e288451ea8dbfa3f2597cfc75c1c7d3c6e0101db1618048394eb4eb0ed1212144f1103e9fb00ff5cbb7fa45f818a6aa8372450aa724c04ed600144bd6ac9720320c62e5513";
	char aPbstr[] ="c684d5aa03895f671ccd6dd24303eb1cea3236458ee9f0b3080c8aacb0381b6b8ea9051a5d5bb3f8625c8e83854bc433b8df587caa64421adea2e5a36f2413ba1468917a1d3a07d9012a8b19dcf6cc1a5b602f6d175380f86268f5aad06496d1e1a987d8e7c9d3f190f31bd694ad7d2244927877fb196106d1c381b6b1cb4c7b";
	char aPcstr[] = "227da5cc96ddf5254e5f28e23b16909d966cc60f03ee3affcec271846dd465b826aaeaddc632c06d177f1955f745e0465ee5561dc4559ddf22f053e4ff4f1f35633a190f1774c9c47ea445c2420256beb2236ff7f67e043336186f57f3fa42c0df7c95fc38f333fc27feef4c3e321a3270992d63fbc5b8fecc901515a52caaae";
	char aPdstr[] = "8a52b0e3fb543a9242f192040c2cb714d8851af5a15e7cd716935c9de1917cf6155fb866cb83100d6589b4828001de8252de6dacb86a1ef3e084745dbba956702c7cb085b6a842c2cf9fe9a069f7e7ac7f23503150f8b9aaf929ff853d1b847a91382894f87255d5e67d9438dfad02d0d1973b06ad59525a6e5c0e721d780c4";

	char rstr[] = "71ebb70a8810c5472a7e22560a891b2ac999c32d246d8f8c7d13eafb78ea7f893ab98810a9749df6e4185fc2663bccf28883e0035f975422bfe5f574cbef0eda1f628e6ff0c344e332f4294c45ec3b5db29f458ac21e01e00e19e3a5b5c9f63761167ee69f27a26fa25604e9bc97fcb65bd82dac1a9868a8e3ef758b8e859b5f";
	char ystr[] = "43415a41c655b5b6af3ea86808183e80bff01109269a0d6293e0aa8fcea61f0b91834090d7afe4bd52b22466f63381c93c4c3f8500359f07a11af4a25dffb30ba6793304195bdac75cf65c887384c58af07722121583456c48843489a8c286fd637e2f6c97520519471e225c4468d7aee1a563c24d651bea4ec93ca3b243c841";
	char ryPstr[] = "11dcf80906d4a97589595b305b5749419d79f3404de02c56767eafa6e0b1111a5dee03371d7a17552e6633205266459c74d605a5a238a90aa37f7f90a572465e67372c51c1bdcace4858e31c201a19d6a336807802542daba4abe7110905e1964ebbab84f1c250e5b6dae345499ac811a16f85cf4a4deb064136adc45fd6cde7";

	MPINT* a1 = new_mpint();
	MPINT* a2 = new_mpint();
	MPINT* a3 = new_mpint();
	MPINT* a4 = new_mpint();
	MPINT* a5 = new_mpint();
	MPINT* a6 = new_mpint();
	MPINT* a7 = new_mpint();
	MPINT* b = new_mpint();
	MPINT* c = new_mpint();
	MPINT* d = new_mpint();
	MPINT* aP10 = new_mpint();
	MPINT* aP3 = new_mpint();
	MPINT* aPb = new_mpint();
	MPINT* aPc = new_mpint();
	MPINT* aPd = new_mpint();
	MPINT* v0 = new_mpint();
	MPINT* v1 = new_mpint();
	MPINT* r = new_mpint();
	MPINT* y = new_mpint();
	MPINT* ryP = new_mpint();

	parse_mpint(a1, astr, strnlen(astr, 10000));
	parse_mpint(a2, astr, strnlen(astr, 10000));
	parse_mpint(a3, astr, strnlen(astr, 10000));
	parse_mpint(a4, astr, strnlen(astr, 10000));
	parse_mpint(a5, astr, strnlen(astr, 10000));
	parse_mpint(a6, astr, strnlen(astr, 10000));
	parse_mpint(a7, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(d, dstr, strnlen(dstr, 10000));
	parse_mpint(aP10, aP10str, strnlen(aP10str, 10000));
	parse_mpint(aP3, aP3str, strnlen(aP3str, 10000));
	parse_mpint(aPb, aPbstr, strnlen(aPbstr, 10000));
	parse_mpint(aPc, aPcstr, strnlen(aPcstr, 10000));
	parse_mpint(aPd, aPdstr, strnlen(aPdstr, 10000));
	word_mpint(v0, 0 , 1);
	word_mpint(v1, 1 , 1);
	parse_mpint(r, rstr, strnlen(rstr, 10000));
	parse_mpint(y, ystr, strnlen(ystr, 10000));
	parse_mpint(ryP, ryPstr, strnlen(ryPstr, 10000));

	kprintf("pow, Test 1: %s\n", powmod_mpint(a1, v0, &prime1, REAL) == TRUE && cmp_mpint(a1, v1) == 0 ? strOK : strFAIL);
	kprintf("pow, Test 2: %s\n", powmod_mpint(a2, v1, &prime1, REAL) == TRUE && cmp_mpint(a2, a3) == 0 ? strOK : strFAIL);
	word_mpint(v1, 10, 1);
	kprintf("pow, Test 3: %s\n", powmod_mpint(a3, v1, &prime1, REAL) == TRUE && cmp_mpint(a3, aP10) == 0 ? strOK : strFAIL);
	word_mpint(v1, 3, 1);
	kprintf("pow, Test 4: %s\n", powmod_mpint(a4, v1, &prime1, REAL) == TRUE && cmp_mpint(a4, aP3) == 0 ? strOK : strFAIL);
	kprintf("pow, Test 5: %s\n", powmod_mpint(a5, b, &prime1, REAL) == TRUE && cmp_mpint(a5, aPb) == 0 ? strOK : strFAIL);
	kprintf("pow, Test 6: %s\n", powmod_mpint(a6, c, &prime1, REAL) == TRUE && cmp_mpint(a6, aPc) == 0 ? strOK : strFAIL);
	kprintf("pow, Test 7: %s\n", powmod_mpint(a7, d, &prime1, REAL) == TRUE && cmp_mpint(a7, aPd) == 0 ? strOK : strFAIL);
	kprintf("pow, Test 8: %s\n", powmod_mpint(r, y, &(DH_G1.p), REAL) == TRUE && cmp_mpint(r, ryP) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a1);
	delete_mpint(a2);
	delete_mpint(a3);
	delete_mpint(a4);
	delete_mpint(a5);
	delete_mpint(a6);
	delete_mpint(a7);
	delete_mpint(b);
	delete_mpint(c);
	delete_mpint(d);
	delete_mpint(aP10);
	delete_mpint(aP3);
	delete_mpint(aPb);
	delete_mpint(aPc);
	delete_mpint(aPd);
	delete_mpint(v0);
	delete_mpint(v1);
	delete_mpint(r);
	delete_mpint(y);
	delete_mpint(ryP);
}

void test_powmodmpint_performances() {

	char astr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char bstr[] = "a6ac6b647f196ff62ba772bf59dcc541b209763baa515a5dd62ff591c600bb3cd5be3358392022ee2fe1e9510fa9f77ee61055e2fb8934a40a276d4f803bc74fcb7b972ff8ae2539448d96c0a85f85950ab7d5ff7ff41402ce4132234fe7144ff2c72743bb0c393699dee4b73e0f2dcdba792f01aa2dd5290c2e86fd8f8fd92b";
	char fstr[] = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";

	int32 it = 50, i;
	uint32 start, end;

	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* f = new_mpint();

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(f, fstr, strnlen(fstr, 10000));
	kprintf("Start performances test\n");

	start = clktime;
	for (i = 0 ; i < it ; ++i) {
		powmod_mpint(a, b, &prime1, REAL);
	}
	end = clktime;
	kprintf("Average: Time elapsed for %d iterations: %u\n", it, end - start);

	start = clktime;
	for (i = 0 ; i < it ; ++i) {
		powmod_mpint(a, f, &prime1, REAL);
	}
	end = clktime;
	kprintf("WCET: Time elapsed for %d iterations: %u\n", it, end - start);

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(f);
}

void test_divmpint() {
	char astr[] = "ffcbd6515c624f17ea57af";
	char bstr[] = "3ec291aebd788542";
	char abqstr[] = "4136677";
	char abrstr[] = "13f1189013e61a01";
	char cstr[] = "9942b54259566087365b913bc3ed637fe7a77e490193c036f0b8163f7227235539ee50046593e2b2391023674a8309af1a29312439781170e8d9b1968b791dbdea70ee2a703ab31c8b913d4b69d94ca5a6e8465248ea36e0ec5ad582c32763e1fa2fcf12c36fe198";
	char p1valcqstr[] = "1ab9c9158af51d75da736d6bcfcb0b152036a64bcd060456f";
	char p1valcrstr[] = "1af4e5a8e8417a081e82895ff629ed579bcd146517a46a617108b3668bd4224dd3cea9e3ed67012de0ab14198655091cc08c01d98da22acd720372fd655cef51aa9520893ccb4d1c6549511245c14ed865d1d567b1d9402e7b9b1571958275235167eb5001af3717";
	char dstr[] = "36f0255dde973dcb3b399d747f23e32ed6fdb1f77598338bfdf44159c4ec64ddaeb5f78671cbfb22106ae64c32c5bce4cfd4f5920da0ebc8b01eca9292ae3dba1b7a4a899da181390bb3bd1659c81294f400a3490bf9481211c79404a576605a5160dbee83b4e019b6d799ae1319ac7e0000000000000001";
	char p1valdqstr[] = "4a8e8270b84b742c7";
	char p1valdrstr[] = "2704f22eb10f72dd56e89014773b565e06a79fa0860e801c87b26b843e864bc029e6d327ecefee3ecd1c44b45cb59ebfa41d172f84390160bbd0ee37437611aea8c793351bddcff51601955781c19d9ee12941ec98d035fb71bf324b9e12fa1e36a422ef81e3719637168f5f2f66c18b5717d8f47b48bd38";



	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* c = new_mpint();
	MPINT* d = new_mpint();
	MPINT* q = new_mpint();
	MPINT* r = new_mpint();
	MPINT* abq = new_mpint();
	MPINT* abr = new_mpint();
	MPINT* p1valcq = new_mpint();
	MPINT* p1valcr = new_mpint();
	MPINT* p1valdq = new_mpint();
	MPINT* p1valdr = new_mpint();

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(d, dstr, strnlen(dstr, 10000));
	parse_mpint(abq, abqstr, strnlen(abqstr, 10000));
	parse_mpint(abr, abrstr, strnlen(abrstr, 10000));
	parse_mpint(p1valcq, p1valcqstr, strnlen(p1valcqstr, 10000));
	parse_mpint(p1valcr, p1valcrstr, strnlen(p1valcrstr, 10000));
	parse_mpint(p1valdq, p1valdqstr, strnlen(p1valdqstr, 10000));
	parse_mpint(p1valdr, p1valdrstr, strnlen(p1valdrstr, 10000));

	kprintf("div, Test 1: %s\n", unsigndiv_mpint(a, b, q, r) == TRUE && cmp_mpint(abq, q) == 0 && cmp_mpint(abr, r) == 0 ? strOK : strFAIL);
	kprintf("div, Test 2: %s\n", unsigndiv_mpint(&p1val, c, q, r) == TRUE && cmp_mpint(p1valcq, q) == 0 && cmp_mpint(p1valcr, r) == 0 ? strOK : strFAIL);
	kprintf("div, Test 3: %s\n", unsigndiv_mpint(&p1val, d, q, r) == TRUE && cmp_mpint(p1valdq, q) == 0 && cmp_mpint(p1valdr, r) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(c);
	delete_mpint(d);
	delete_mpint(q);
	delete_mpint(r);
	delete_mpint(abq);
	delete_mpint(abr);
	delete_mpint(p1valcq);
	delete_mpint(p1valcr);
	delete_mpint(p1valdq);
	delete_mpint(p1valdr);
}

void test_modmpint() {
	char astr[] = "ffcbd6515c624f17ea57af";
	char bstr[] = "3ec291aebd788542";
	char abrstr[] = "13f1189013e61a01";
	char cstr[] = "9942b54259566087365b913bc3ed637fe7a77e490193c036f0b8163f7227235539ee50046593e2b2391023674a8309af1a29312439781170e8d9b1968b791dbdea70ee2a703ab31c8b913d4b69d94ca5a6e8465248ea36e0ec5ad582c32763e1fa2fcf12c36fe198";
	char p1valcrstr[] = "1af4e5a8e8417a081e82895ff629ed579bcd146517a46a617108b3668bd4224dd3cea9e3ed67012de0ab14198655091cc08c01d98da22acd720372fd655cef51aa9520893ccb4d1c6549511245c14ed865d1d567b1d9402e7b9b1571958275235167eb5001af3717";


	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* c = new_mpint();
	MPINT* r = new_mpint();
	MPINT* abr = new_mpint();
	MPINT* p1valcr = new_mpint();

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(abr, abrstr, strnlen(abrstr, 10000));
	parse_mpint(p1valcr, p1valcrstr, strnlen(p1valcrstr, 10000));

	kprintf("mod, Test 1: %s\n", unsignmod_mpint(r, a, b) == TRUE && cmp_mpint(abr, r) == 0 ? strOK : strFAIL);
	kprintf("mod, Test 2: %s\n", unsignmod_mpint(r, &p1val, c) == TRUE && cmp_mpint(p1valcr, r) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(c);
	delete_mpint(r);
	delete_mpint(abr);
	delete_mpint(p1valcr);
}


void test_invmpint() {
	char astr[] = "61";
	char bstr[] = "19";
	char binvastr[] = "42";
	char cstr[] = "6d2040fc7fcce111a03fe5b1f2cf9073d28ab435315d68c369660dbb5efd24db4d89cf90b9bdbccf0f0bd5cdb28f7b9b044fbcbe1019092d219ed0df0cef1fd906fceb3ca250b62befc29fdd67ead2e1b8790cfb6a8e094232fcba3a26056f5c3025aa3ac516dc487be7f860ae11ad6fe9a7c0d6fcf23270da02083b23c22d4c";
	char cinvp1valstr[] = "69e2c1eb9c4ab57ec124badc230d14109a51a754ecf1506b9f861f44b34331b92b12998889c1433af91e7ff99570da1a5a61aa10e84bdb450745eb9fa002e94804f243cd12cdd55feb3aff828fe4b77e9c1a7ae8f07386f009d90b6e44e989e03f1782589e502f091e0af6ae9f10bb3a9d061d06ab0620dca4e3a998bfa248f5";


	MPINT* a = new_mpint();
	MPINT* b = new_mpint();
	MPINT* binva = new_mpint();
	MPINT* c = new_mpint();
	MPINT* cinvp1val = new_mpint();

	parse_mpint(a, astr, strnlen(astr, 10000));
	parse_mpint(b, bstr, strnlen(bstr, 10000));
	parse_mpint(binva, binvastr, 2);
	parse_mpint(c, cstr, strnlen(cstr, 10000));
	parse_mpint(cinvp1val, cinvp1valstr, strnlen(cinvp1valstr, 10000));

	kprintf("inv, Test 1: %s\n", invmod_mpint(b, a) == TRUE && cmp_mpint(b, binva) == 0 ? strOK : strFAIL);
	kprintf("inv, Test 2: %s\n", invmod_mpint(c, &p1val) == TRUE && cmp_mpint(c, cinvp1val) == 0 ? strOK : strFAIL);
	kprintf("\n");

	delete_mpint(a);
	delete_mpint(b);
	delete_mpint(binva);
	delete_mpint(c);
	delete_mpint(cinvp1val);

}

void test_powutil() {

	MPPRIME p;
	init_mpprime(&p);

	p.num.val = p1val.val;
	p.num.order = p1val.order;
	p.num.size = p1val.size;
	p.num.sign = p1val.sign;

	bool8 st = precompprime_mpint(&p);
	kprintf("utilpow, Test 1: %s\n", st == TRUE && cmp_mpint(&(p.inv), &INVp1val) == 0 && cmp_mpint(&(p.r_square), &R2p1val) == 0 ? strOK : strFAIL);
	kprintf("\n");

	clear_mpint(&(p.inv));
	clear_mpint(&(p.r_square));

}

void launch_mpint () {
	int32 mem = memlist.mlength;

	test_parsempint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_cmpmpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_sum();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_mulmpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_shiftmpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_montgomery_mpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_pow_mpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_divmpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_modmpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_invmpint();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	mem = memlist.mlength;
	test_powutil();
	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}
	//  mem = memlist.mlength;
	//  test_powmodmpint_performances();
	//  if (mem != memlist.mlength) {
	//    kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	//  }
	kprintf("Test Done!\n");
}
