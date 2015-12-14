#include <xinu.h>
#include <stdio.h>

/* from ssh_algorithms_negociation */
bool8 choose_algorithms(struct sshent* sshb, byte* cli_pck, int32 cli_len, byte* ser_pck, int32 ser_len);
bool8 choose_algorithm(struct sshent* sshb, int32 type, byte** cli_pck, int32* cli_len, byte** ser_pck, int32* ser_len);

void launch_sshtests() {
  struct sshent sshb;

  sshb.kexid = 0;

  char serv[] = "\x00\x00\x00,diffie-hellman-group1-sha1,diffie-hellman-group1-sha1-96";
  serv[3] = 56;
  char clie[] = "\x00\x00\x00,diffie-hellman-group14-sha256,diffie-hellman-group14-sha1-96,diffie-hellman-group1-sha256,diffie-hellman-group1-sha1-96";
  clie[3] = 119;

  byte* s = (byte *)serv;
  byte* c = (byte *)clie;

  int32 ser_len = 56;
  int32 cli_len = 119;

  choose_algorithm(&sshb, KEX_ALGO, &c, &cli_len, &s, &ser_len);

  kprintf("Algo choosed: %x\n", sshb.kexid);
  kprintf("Lenghts: %d %d\n", ser_len, cli_len);
  kprintf("\n");
}
