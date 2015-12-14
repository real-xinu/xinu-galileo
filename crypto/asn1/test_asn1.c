#include <xinu.h>
#include <asn1.h>

void test_id() {

	uint32 id;
	byte class, form;
	byte output[8];
	byte exp[8];

	/* test 1 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0x22;
	if (1 != asn1write_identifier(output, 2, UNIVERSAL, CONSTRUCTED) ||
			memcmp(output, exp, 1) != 0) {
		kprintf("\tTest write ID (1): FAIL\n");
		return;
	}

	if (1 != asn1read_identifier(&id, &class, &form, output) ||
			id != 2 || class != UNIVERSAL || form != CONSTRUCTED) {
		kprintf("\tTest read ID (1): FAIL (%d, %x, %x)\n", id, class, form);
		return;
	}

	/* test 2 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0xBF;
	exp[1] = 0x1F;
	if (2 != asn1write_identifier(output, 31, CONTEXTSPEC, CONSTRUCTED) ||
			memcmp(output, exp, 2) != 0) {
		kprintf("\tTest write ID (2): FAIL\n");
		xdump_array(output, 8);
		return;
	}
	if (2 != asn1read_identifier(&id, &class, &form, output) ||
			id != 31 || class != CONTEXTSPEC || form != CONSTRUCTED) {
		kprintf("\tTest read ID (2): FAIL\n");
		return;
	}

	/* test 3 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0xBF;
	exp[1] = 0x88;
	exp[2] = 0x80;
	exp[3] = 0x80;
	exp[4] = 0x80;
	exp[5] = 0x01;
	if (6 != asn1write_identifier(output, 0x80000001, CONTEXTSPEC, CONSTRUCTED) ||
			memcmp(output, exp, 6) != 0) {
		kprintf("\tTest write ID (3): FAIL\n");
		xdump_array(output, 8);
		return;
	}
	if (6 != asn1read_identifier(&id, &class, &form, output) ||
			id != 0x80000001 || class != CONTEXTSPEC || form != CONSTRUCTED) {
		kprintf("\tTest read ID (3): FAIL (%08x, %x, %x)\n", id, class, form);
		return;
	}

	/* test 3 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0xBF;
	exp[1] = 0x8A;
	exp[2] = 0xD5;
	exp[3] = 0xAA;
	exp[4] = 0xD5;
	exp[5] = 0x2A;
	if (6 != asn1write_identifier(output, 0xAAAAAAAA, CONTEXTSPEC, CONSTRUCTED) ||
			memcmp(output, exp, 6) != 0) {
		kprintf("\tTest write ID (4): FAIL\n");
		xdump_array(output, 8);
		return;
	}
	if (6 != asn1read_identifier(&id, &class, &form, output) ||
			id != 0xAAAAAAAA || class != CONTEXTSPEC || form != CONSTRUCTED) {
		kprintf("\tTest read ID (4): FAIL (%08x, %x, %x)\n", id, class, form);
		return;
	}
	kprintf("\tTest write ID: OK\n");
}

void test_length() {

	byte output[8];
	byte exp[8];
	uint32 length;

	/* test 1 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0x10;
	if (1 != asn1write_length(output, 16, DEFINITE) ||
			memcmp(output, exp, 1) != 0) {
		kprintf("\tTest write length (1): FAIL\n");
		return;
	}
	if (1 != asn1read_length(&length, output) ||
			length != 16) {
		kprintf("\tTest read length (1): FAIL\n");
		return;
	}

	/* test 2 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0x81;
	exp[1] = 0x80;
	if (2 != asn1write_length(output, 0x80, DEFINITE) ||
			memcmp(output, exp, 2) != 0) {
		kprintf("\tTest write length (2): FAIL\n");
		xdump_array(output, 8);
		return;
	}
	if (2 != asn1read_length(&length, output) ||
			length != 0x80) {
		kprintf("\tTest read length (2): FAIL (%x)\n", length);
		return;
	}

	/* test 3 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0x84;
	exp[1] = 0x80;
	exp[2] = 0x00;
	exp[3] = 0x00;
	exp[4] = 0x01;
	if (5 != asn1write_length(output, 0x80000001, DEFINITE) ||
			memcmp(output, exp, 5) != 0) {
		kprintf("\tTest write length (3): FAIL\n");
		xdump_array(output, 8);
		return;
	}
	if (5 != asn1read_length(&length, output) ||
			length != 0x80000001) {
		kprintf("\tTest read length (3): FAIL\n");
		return;
	}

	/* test 3 */
	memset(output, 0, sizeof(output));
	memset(exp, 0, sizeof(exp));

	exp[0] = 0x82;
	exp[1] = 0xAA;
	exp[2] = 0xAA;
	if (3 != asn1write_length(output, 0xAAAA, DEFINITE) ||
			memcmp(output, exp, 3) != 0) {
		kprintf("\tTest write length (4): FAIL\n");
		xdump_array(output, 8);
		return;
	}
	if (3 != asn1read_length(&length, output) ||
			length != 0xAAAA) {
		kprintf("\tTest read length (3): FAIL\n");
		return;
	}
	kprintf("\tTest length: OK\n");
}

void launch_asn1tests() {
	kprintf("Launch ASN1 tests\n");
	test_id();
	test_length();
}
