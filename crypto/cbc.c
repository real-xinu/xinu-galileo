#include <xinu.h>

bool8 cipher_cbc_enc(cid32 cid, byte* iv, void* keyblob, byte* msg, byte* ciphertext, int32 length) {

  int32 i, block_size, n_block, b;
  uint32* wtmsg, *wtiv, *wtciph;

  /* The length must be a multiple of the block size */
  block_size = ciphtab[cid].block_size;
  if (length <= 0 || length % block_size != 0) {
#ifdef CRYPTO_DEBUG
    kprintf("cbc_enc: length not a multiple of block size %d : %d\n", length, block_size);
#endif
    return FALSE;
  }

  /* we use uint32 for xoring */
  n_block = length/block_size;
  block_size /= sizeof(uint32);

  wtciph = (uint32 *)ciphertext;
  wtmsg  = (uint32 *)msg;
  wtiv   = (uint32 *)iv;

  /* xor first block with IV */
  for (i = 0 ; i < block_size ; i++) {
    *wtciph++ = *wtmsg++ ^ *wtiv++;
  }
  /* Encrypt first block */
  ciphtab[cid].enc(keyblob, ciphertext, ciphertext);

  /* set IV to be first block now */
  wtiv = (uint32 *)ciphertext;

  /* Encrypt other blocks */
  for (b = 1 ; b < n_block ; b++) {
    for (i = 0 ; i < block_size ; i++) {
      *wtciph++ = *wtmsg++ ^ *wtiv++;
    }
    ciphtab[cid].enc(keyblob, (byte *)wtiv, (byte *)wtiv);
  }

  /* update IV */
  memcpy(iv, (byte *)(wtciph - block_size), block_size * sizeof(uint32));

  return TRUE;
}

bool8 cipher_cbc_dec(cid32 cid, byte* iv, void* keyblob, byte* ciphertext, byte* msg, int32 length) {
  int32 i, block_size, n_block, b;
  uint32* wtmsg, *wtiv, *next_msgblock;
  byte iv_tmp[MAX_KEYS_SIZE];

  /* The length must be a multiple of the block size */
  block_size = ciphtab[cid].block_size;
  if (length <= 0 || length % block_size != 0) {
#ifdef CRYPTO_DEBUG
    kprintf("cbc_dec: length not a multiple of block size %d : %d\n", length, block_size);
#endif
    return FALSE;
  }

  /* save next IV */
  memcpy(iv_tmp, ciphertext + length - block_size, block_size);

  /* we use uint32 for xoring */
  n_block = length/block_size;
  block_size /= sizeof(uint32);

  wtiv = (uint32 *)(ciphertext + length) - block_size - 1;
  wtmsg  = (uint32 *)(msg + length) - 1;
  next_msgblock = wtmsg - block_size + 1;

  /* decrypt from last to first */
  for (b = 1 ; b < n_block ; b++) {
    ciphtab[cid].dec(keyblob, (byte *)(wtiv + 1), (byte *)next_msgblock);
    for (i = 0 ; i < block_size ; i++) {
      *wtmsg-- ^= *wtiv--;
    }
    next_msgblock -= block_size;
  }

  wtmsg = (uint32 *)msg;
  wtiv  = (uint32 *)iv;
  ciphtab[cid].dec(keyblob, ciphertext, msg);
  for (i = 0 ; i < block_size ; i++) {
    *wtmsg++ ^= *wtiv++;
  }

  /* save next IV */
  memcpy(iv, iv_tmp, block_size * sizeof(uint32));

  return TRUE;
}
