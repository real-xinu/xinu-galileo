#include <xinu.h>

/*-------------------------------------------------------------------------------------
 * ssh_debug - send debug message.
 *-------------------------------------------------------------------------------------
 */
void ssh_debug(struct sshent* sshb, bool8 display, char* msg, int32 length) {

  byte buff[MAX_DEBUG_BUFFER_LENGTH];
  struct ssh_binary_packet* bpck = (struct ssh_binary_packet *) buff;
  byte* wt = bpck->buff;

  *wt++ = SSH_MSG_DEBUG;
  *wt++ = display & 0x1;

  if (length > MAX_DEBUG_MSG_LENGTH)
    length = MAX_DEBUG_MSG_LENGTH;

  length = ssh_write_string(wt, msg, length);
  wt += length;
  length += ssh_write_string(wt, NULL, 0);

  ssh_tlp_out(sshb, bpck, 5 + length);

  return;
}
