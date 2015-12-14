#include <xinu.h>

void none_init(struct hash_state* state) {
  return;
}
void none_update(struct hash_state* state, byte *msg, uint32 length) {
  return;
}
void none_finish(struct hash_state* state, byte *diggest) {
  return;
}

struct hashent hashtab[3] = {
  {.init = none_init, .update = none_update, .finish = none_finish, .block_size = 0, .diggest_size = 0, .key_size = 0, .name = "none"},
  {.init = sha1_init, .update = sha1_update, .finish = sha1_finish, .block_size = 64, .diggest_size = 20, .key_size = 20, .name = "sha1"},
  {.init = sha1_init, .update = sha1_update, .finish = sha1_96_finish, .block_size = 64, .diggest_size = 12, .key_size = 20, .name = "sha1-96"}
};

void hmac_init(hid32 hid, struct hash_state* state, byte *key, uint32 key_length) {
  struct hashent *hash = &(hashtab[hid]);

  int32 i;
  byte buff[MAX_HASH_BLOCK];

  if (key_length > hash->block_size) {
    panic("hmac: key longer than block size\n");
    return;
  }

  (hash->init)(state);

  /* Fill the buffer. */
  for (i = 0 ; i < key_length ; i++)
    buff[i] = 0xFF & (key[i] ^ IPAD);
  for ( ; i < hash->block_size ; i++)
    buff[i] = IPAD;

  (hash->update)(state, buff, hash->block_size);
  return;
}

void hmac_update(hid32 hid, struct hash_state* state,  byte *msg, uint32 length) {

  struct hashent *hash = &(hashtab[hid]);

  (hash->update)(state, msg, length);
}

void hmac_finish(hid32 hid, struct hash_state* state, byte *key, uint32 key_length, byte* diggest) {

  int32 i;
  byte buff[MAX_HASH_BLOCK];
  struct hashent *hash = &(hashtab[hid]);

  /* finish inner hash */
  (hash->finish)(state, diggest);

  /* compute outer hash */
  if (key_length > hash->block_size) {
    panic("hmac: key longer than block size\n");
    return;
  }
  /* Modify buffer */
  for (i = 0 ; i < key_length ; i++)
    buff[i] = 0xFF & (key[i] ^ OPAD);
  for ( ; i < hash->block_size ; i++)
    buff[i] = OPAD;

  /* Outer hash. */
  (hash->init)(state);
  (hash->update)(state, buff, hash->block_size);
  (hash->update)(state, diggest, hash->diggest_size);
  (hash->finish)(state, diggest);

  return;
}
