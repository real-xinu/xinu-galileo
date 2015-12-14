#include <xinu.h>

bool8 allocate_buff(struct buffer* kbuff, int32 size) {

  if ((kbuff->buff = (byte *)getmem(size)) == (byte *)SYSERR) {
#ifdef SSH_DEBUG
    kprintf("allocate_buff: OutOfMemory\n");
#endif
    return FALSE;
  }

  kbuff->current = kbuff->buff;
  kbuff->end = kbuff->buff + size;

  return TRUE;
}

void clear_buffer(struct buffer* kbuff) {

  freemem((char *)kbuff->buff, (int32) (kbuff->end - kbuff->buff));
  kbuff->end = NULL;
  kbuff->current = NULL;
}

/*-------------------------------------------------------------------------------------
 * append_buff - add the given bytes into the buffer
 *-------------------------------------------------------------------------------------
 */
bool8 append_buff(struct buffer* buff, byte *tab, int32 len) {
  int32 i;

  if (buff->current + len > buff->end) {
#ifdef SSH_DEBUG
    kprintf("\nappend_buff: buffer full\n");
#endif
    return FALSE;
  }

  for (i = 0 ; i < len ; i++)
    *(buff->current)++ = *tab++;

  return TRUE;
}

/*-------------------------------------------------------------------------------------
 * addint_buff - add the given integer into the buffer
 *-------------------------------------------------------------------------------------
 */
bool8 addint_buff(struct buffer* buff, int32 len) {

  if (buff->current + 4 > buff->end) {
#ifdef SSH_DEBUG
    kprintf("\naddint_buff: buffer full\n");
#endif
    return FALSE;
  }

  *((int32 *) (buff->current)) = htonl(len);
  buff->current += 4;

  return TRUE;
}
