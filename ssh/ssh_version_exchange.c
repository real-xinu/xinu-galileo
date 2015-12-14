#include <xinu.h>

bool8 check_version(const char *str, int32 nbytes) {
  if (nbytes <= 255 && str[0] == 'S' && str[1] == 'S' && str[2] == 'H' && str[3] == '-')
    return TRUE;

  return FALSE;
}

bool8 ssh_version_exchange(struct sshent *sshb, struct buffer *kex_buff) {
  int32 nbytes, lversion;
  char buff[256];

  sleepms(10); //Magic!!

  lversion = strnlen(SSH_VERSION, BUFFERS_LENGTH);
  if (tcp_send(sshb->tcp_id, SSH_VERSION, lversion) != lversion) {
#ifdef SSH_DEBUG
    kprintf("version_exchange: Fail sending SSH Version\n");
#endif
    return FALSE;
  }

  nbytes = 0;
  while (nbytes < 256 && tcp_recv(sshb->tcp_id, buff + nbytes, 1) == 1) {
      if (buff[nbytes++] == 0x0a)
          break;
  }

  if (nbytes == 0 || buff[nbytes-1] != 0x0a || check_version(buff, nbytes) == FALSE) {
#ifdef SSH_DEBUG
    kprintf("version_exchange: Fail receving SSH Version\n");
#endif
    return FALSE;
  }

  /* don't take cariage return */
  lversion -= 2;
  nbytes -= 2;

  /* Copy value into the kex buffer. */
  if (SIDE(sshb->state) == SSH_CLIENT) {
    if (addint_buff(kex_buff, lversion) == FALSE || \
        append_buff(kex_buff, (byte *)SSH_VERSION, lversion) == FALSE || \
        addint_buff(kex_buff, nbytes) == FALSE || \
        append_buff(kex_buff, (byte *)buff, nbytes) == FALSE) {
      return FALSE;
    }
  } else if (SIDE(sshb->state) == SSH_SERVER) {
    if (addint_buff(kex_buff, lversion) == FALSE || \
        append_buff(kex_buff, (byte *)buff, nbytes) == FALSE || \
        addint_buff(kex_buff, nbytes) == FALSE || \
        append_buff(kex_buff, (byte *)SSH_VERSION, lversion) == FALSE) {
      return FALSE;
    }
  } else {
    return FALSE;
  }

#ifdef SSH_DEBUG
  kprintf("version_exchange:\n");
 xdump_array(kex_buff->buff, (int32)(kex_buff->current - kex_buff->buff));
#endif

  return TRUE;
}
