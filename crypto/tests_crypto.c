#include <xinu.h>

int cmp_hash(byte* v1, byte *val, int32 length) {
	int32 i;
	i = 0;
	while (i < length && v1[i] == val[i])
		i++;

	return i == length;
}

void sha1_tests() {

	byte diggest[20];
	struct hash_state state;

	sha1_init(&state);
	sha1_update(&state, (byte *)"", 0);
	sha1_finish(&state, diggest);

	byte val[] = "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09";
	kprintf("SHA1, Test 1: %s\n", cmp_hash(diggest, val, 20) ? "OK" : "FAIL");


	sha1_init(&state);
	sha1_update(&state, (byte *)"a", 1);
	sha1_finish(&state, diggest);
	byte val1[] = "\x86\xf7\xe4\x37\xfa\xa5\xa7\xfc\xe1\x5d\x1d\xdc\xb9\xea\xea\xea\x37\x76\x67\xb8";
	kprintf("SHA1, Test 2: %s\n", cmp_hash(diggest, val1, 20) ? "OK" : "FAIL");

	sha1_init(&state);
	sha1_update(&state, (byte *)"abcd", 4);
	sha1_finish(&state, diggest);
	byte val2[] = "\x81\xfe\x8b\xfe\x87\x57\x6c\x3e\xcb\x22\x42\x6f\x8e\x57\x84\x73\x82\x91\x7a\xcf";
	kprintf("SHA1, Test 3: %s\n", cmp_hash(diggest, val2, 20) ? "OK" : "FAIL");

	sha1_init(&state);
	sha1_update(&state, (byte *)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 61);
	sha1_finish(&state, diggest);
	byte val3[] = "\xae\xab\x14\x1d\xb2\x8a\xf3\x35\x32\x83\xb5\xcc\xb2\xa3\x22\xdf\x0b\x9b\x5f\x56";
	kprintf("SHA1, Test 4: %s\n", cmp_hash(diggest, val3, 20) ? "OK" : "FAIL");

	sha1_init(&state);
	sha1_update(&state, (byte *)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 117);
	sha1_finish(&state, diggest);
	byte val4[] = "\x95\xf8\xde\x0e\xea\x68\x49\x77\x81\xd5\x3a\x36\x8a\xd9\xcd\xe0\x35\xe8\xc6\x51";
	kprintf("SHA1, Test 5: %s\n", cmp_hash(diggest, val4, 20) ? "OK" : "FAIL");

	kprintf("\n");
}

void hmac_tests() {
	byte diggest[20];
	struct hash_state state;

	hmac_init(SHA1, &state, (byte *)"a", 1);
	hmac_update(SHA1, &state, (byte *)"a", 1);
	hmac_finish(SHA1, &state, (byte *)"a", 1, diggest);
	byte val[] = "\x39\x02\xed\x84\x7f\xf2\x89\x30\xb5\xf1\x41\xab\xfa\x8b\x47\x16\x81\x25\x36\x73";
	kprintf("HMAC_SHA1, Test 1: %s\n", cmp_hash(diggest, val, 20) ? "OK" : "FAIL");

	hmac_init(SHA1, &state, (byte *)"", 0);
	hmac_update(SHA1, &state, (byte *)"", 0);
	hmac_finish(SHA1, &state, (byte *)"", 0, diggest);
	byte val1[] = "\xfb\xdb\x1d\x1b\x18\xaa\x6c\x08\x32\x4b\x7d\x64\xb7\x1f\xb7\x63\x70\x69\x0e\x1d";
	kprintf("HMAC_SHA1, Test 2: %s\n", cmp_hash(diggest, val1, 20) ? "OK" : "FAIL");

	hmac_init(SHA1, &state, (byte *)"Purdue University", 17);
	hmac_update(SHA1, &state, (byte *)"Xinu is not Unix", 16);
	hmac_finish(SHA1, &state, (byte *)"Purdue University", 17, diggest);
	byte val2[] = "\xf1\xdd\x79\x8a\x13\xeb\x75\x8b\xce\xa3\x21\x58\xaf\xbf\x20\x22\x13\xc7\xed\x57";
	kprintf("HMAC_SHA1, Test 3: %s\n", cmp_hash(diggest, val2, 20) ? "OK" : "FAIL");
	kprintf("\n");

}

void des_ip_test() {
	int32 i;

	/* initial permutation */
	byte testvector[][8] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}, {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0}};
	uint32 ipvector[][2] = {{0xFFFFFFFF, 0xFFFFFFFF}, {0xFFFFFFFF, 0x00000000}, {0x00000000, 0xFFFFFFFF}, {0xccff6600, 0xf0aa7855}};
	uint32 left, right;
	byte out[8];

	generate_desip_table();

	for (i = 0 ; i < 4 ; i++) {
		des_ip(testvector[i], &left, &right);
		des_ipinv(left, right, out);
		kprintf("IP DES test %d: %s\n\n", 2 * i, testvector[i][0] == out[0] && testvector[i][1] == out[1] && ipvector[i][0] == left &&  ipvector[i][1] == right ? "OK" : "Fail");

		fast_des_ip(testvector[i], &left, &right);
		fast_des_ipinv(left, right, out);
		kprintf("IP DES fast test %d: %s\n\n", 2 *i + 1, testvector[i][0] == out[0] && testvector[i][1] == out[1] && ipvector[i][0] == left &&  ipvector[i][1] == right ? "OK" : "Fail");
	}

}

void des_estep_test() {
	byte buff[8];
	byte key[8];
	int32 i;

	/* test 1 */
	memset(buff, 0, 8);
	memset(key, 0, 8);
	des_estep(0xFFFFFFFF, key, buff);
	i = 0;
	while (i < 8 && buff[i] == 0x3F) {
		i++;
	}
	kprintf("Estep 1: %s\n", i == 8 ? "OK" : "Fail");

	/* test 2 */
	memset(buff, 0, 8);
	memset(key, 0, 8);
	des_estep(0x66666666, key, buff);
	i = 0;
	while (i < 8 && buff[i] == 0x0c) {
		i++;
	}
	kprintf("Estep 2: %s\n", i == 8 ? "OK" : "Fail");

	/* test 3 */
	memset(buff, 0, 8);
	memset(key, 0, 8);
	des_estep(0x99999999, key, buff);
	i = 0;
	while (i < 8 && buff[i] == 0x33) {
		i++;
	}
	kprintf("Estep 3: %s\n", i == 8 ? "OK" : "Fail");

	/* test 4 */
	memset(buff, 0, 8);
	memset(key, 0, 8);
	des_estep(0xaaaaaaaa, key, buff);
	i = 0;
	while (i < 8 && buff[i] == 0x15) {
		i++;
	}
	kprintf("Estep 4: %s\n", i == 8 ? "OK" : "Fail");

	/* test 5 */
	memset(buff, 0, 8);
	memset(key, 0, 8);
	des_estep(0x55555555, key, buff);
	i = 0;
	while (i < 8 && buff[i] == 0x2a) {
		i++;
	}
	kprintf("Estep 5: %s\n", i == 8 ? "OK" : "Fail");
	kprintf("\n");
}

/* test based on: http://people.csail.mit.edu/rivest/pubs/Riv85.txt */
void test_des() {
	byte X[] = "\x94\x74\xB8\xE8\xC7\x3B\xCA\x7D";
	byte Y[8];
	int32 i;

	generate_desip_table();
	generate_desperm_table();

	kprintf("Start test with:\n");
	xdump_array(X, 8);

	for (i = 0 ; i < 8 ; i++) {
		des_encrypt(X, X, Y);
		des_decrypt(Y, Y, X);
	}

	kprintf("Result should be: 1B1A2DDB4C642438\n");
	xdump_array(X, 8);

}

void test_tdea() {
	byte plaintext[] = {0x6B, 0x20, 0x62, 0x72, 0x6F, 0x77, 0x6E, 0x20}; // "k brown "
	byte ciphertext[8];
	byte nplaintext[8];

	byte keys[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
		0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01,
		0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23};

	generate_desip_table();
	generate_desperm_table();


	kprintf("TDEA test:\n");

	kprintf("\nkey:\n");
	xdump_array(keys, 24);

	kprintf("\nplaintext:\n");
	xdump_array(plaintext, 8);

	tdea_encrypt(keys, plaintext, ciphertext);

	kprintf("\nciphertext:\n");
	xdump_array(ciphertext, 8);

	tdea_decrypt(keys, ciphertext, nplaintext);

	kprintf("\nretrieved plaintext:\n");
	xdump_array(nplaintext, 8);
	kprintf("\n");
}

void test_cbc_mode() {

	byte iv[] = "a secret";
	byte iv2[] = "a secret";
	byte key[] = "!It's a key for 3DES ;)!";
	byte plaintext[] = "This is a test for CBC mode with 3DES in Xinu!!!";
	byte ciphertext[48];

	struct tdea_keyblob keyblob;
	tdea_generatekeys(key, &keyblob);

	kprintf("\nThe key is:\n");
	xdump_array(key, 24);

	kprintf("\nThe iv is:\n");
	xdump_array(iv, 8);

	kprintf("\nThe plaintext is:\n");
	xdump_array(plaintext, 48);

	if (cipher_cbc_enc(TDEA, iv, &keyblob, plaintext, ciphertext, 48) == FALSE) {
		return;
	}

	kprintf("\nThe ciphertext is:\n");
	xdump_array(ciphertext, 48);

	if (cipher_cbc_dec(TDEA, iv2, &keyblob, ciphertext, ciphertext, 48) == FALSE) {
		return;
	}

	kprintf("\nThe plaintext is:\n");
	xdump_array(ciphertext, 48);
}

void dsa_test(randctx* ctx) {

  char pstr[] = "8841581186f6a6f3b140ee97d9d57837b50a184fc69634f9284640a88c18246b872effe27e3abe7fa1610b857c35ec3c0aa6be2945a59d32e81a268f10efd10c4c87a7c8b396bc48d98d6471cf9e77e94f373a2481be19e3555c58ae29f4a01bef3e7e473c9f6578a8e8f9a7d5030087b517177b0a989611bd0a960e03b9a5f1";
  char qstr[] = "90ac7ff8c0eb4f291c17db19feebfb92cdb44a8b";
  char gstr[] = "8047da407e700a80a72b87a7c4a4e77f5cc95a899a9bbc039a7760041df3e089f4b879ee1a70790dd66b12ae9504fcc66a0ff5118357e3b42995f12268752e057cd21decee9389cc4d8f668d85b3af633fa8cb7b23e9b07ccef29cdc0d1df12c36674499992bc9b01464be688a748eec7b5a365bbf2d5b4acb89002ac7224350";

  MPINT gm;
  MPPRIME pm;
  MPINT qm;

  init_mpint(&gm);
  init_mpint(&qm);
  init_mpprime(&pm);

  parse_mpint(&gm, gstr, strnlen(gstr, 10000));
  parse_mpint(&(pm.num), pstr, strnlen(pstr, 10000));
  parse_mpint(&qm, qstr, strnlen(qstr, 10000));

  precompprime_mpint(&pm);

  char msg[] = "abcdefghijklmnopqrst";
  struct dsasign sign;
  struct dsacert xcert;
  struct dsapriv xpriv;
  int32 k = 20;
  int32 it = k;
  int32 start, end;

  init_dsapriv(&xpriv);
  init_dsasign(&sign);
  init_dsacert(&xcert);

  xpriv.p.num.val   = xcert.p.num.val   = pm.num.val;
  xpriv.p.num.order = xcert.p.num.order = pm.num.order;
  xpriv.p.num.sign  = xcert.p.num.sign  = pm.num.sign;
  xpriv.p.num.size  = xcert.p.num.size  = pm.num.size;
  xpriv.p.inv.val   = xcert.p.inv.val   = pm.inv.val;
  xpriv.p.inv.order = xcert.p.inv.order = pm.inv.order;
  xpriv.p.inv.sign  = xcert.p.inv.sign  = pm.inv.sign;
  xpriv.p.inv.size  = xcert.p.inv.size  = pm.inv.size;
  xpriv.p.r_square.val   = xcert.p.r_square.val   = pm.r_square.val;
  xpriv.p.r_square.order = xcert.p.r_square.order = pm.r_square.order;
  xpriv.p.r_square.sign  = xcert.p.r_square.sign  = pm.r_square.sign;
  xpriv.p.r_square.size  = xcert.p.r_square.size  = pm.r_square.size;

  xpriv.q.val   = xcert.q.val   = qm.val;
  xpriv.q.order = xcert.q.order = qm.order;
  xpriv.q.sign  = xcert.q.sign  = qm.sign;
  xpriv.q.size  = xcert.q.size  = qm.size;
  xpriv.g.val   = xcert.g.val   = gm.val;
  xpriv.g.order = xcert.g.order = gm.order;
  xpriv.g.sign  = xcert.g.sign  = gm.sign;
  xpriv.g.size  = xcert.g.size  = gm.size;
  rand_mpint(&(xpriv.x), qm.order - 1, ctx);
  copy_mpint(&(xcert.y), &gm);
  powmod_mpint(&(xcert.y), &(xpriv.x), &pm, REAL);

  start = clktime;
  for (; k > 0 ; k--) {
      init_dsasign(&sign);
      if (dsa_sign((byte *)msg, 20, &xpriv, SHA1, ctx, &sign) == FALSE) {
          kprintf("Sign Fail\n");
          break;
      }

      if (dsa_check((byte *)msg, 20, &xcert, SHA1, &sign) == FALSE) {
          kprintf("Check Fail\n");
          break;
      }
      clear_dsasign(&sign);
  }
  end = clktime;

  kprintf("dsa, Test 1: %s\n", k == 0 ? "OK" : "FAIL");
  if (k == 0)
      kprintf("%d signatures & checks in %ds\n", it, end - start);
  kprintf("\n");


  clear_mpprime(&pm);
  clear_mpint(&qm);
  clear_mpint(&gm);
  clear_dsasign(&sign);
  clear_dsacert(&xcert);
  clear_dsapriv(&xpriv);
}

void launch_cryptotests() {
	int32 i;
	randctx ctx;
	init_randompool();

	kprintf("\nStart crypto tests\n");

	init_context(&ctx);
	for (i = 0 ; i < 8 ; i++)
		kprintf("%04x ", crand(&ctx));
	kprintf("\n");

	memset(&ctx, 0, sizeof(randctx));
	byte* seed = (byte *)ctx.randrsl;

	for (i = 0 ; i < 32 ; ++i) {
		seed[i] = i & 0xFF;
	}
	randinit(&ctx, TRUE);
	for (i = 0 ; i < 8 ; i++)
		kprintf("%04x ", crand(&ctx));
	kprintf("\n");

	sha1_tests();
	hmac_tests();

	des_ip_test();
	des_estep_test();
	test_des();
	test_tdea();
	test_cbc_mode();

	int32 mem = memlist.mlength;
	dsa_test(&ctx);

	if (mem != memlist.mlength) {
		kprintf("Memory leak!! %u, %u\n", mem, memlist.mlength);
	}

	kprintf("\nTest Done!\n");
}
