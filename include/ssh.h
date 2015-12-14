#include <ssh_constante.h>

#ifndef SSH_H
#define SSH_H

// #define SSH_DEBUG
// #define SSH_DHDEBUG

#define SSH_PORT            22222

#define SSH_BLOCKS          1
#define SSH_PORT_SIZE       10
#define SSH_BUFF_SIZE       8192
#define SSH_BUFF_NB         (SSH_BLOCKS * SSH_PORT_SIZE)
#define BUFFERS_LENGTH      34995

#define MAX_KEX_BUFF_LENGTH (510 + 4000 + 2048 + 1024*2 + 128)

// Maximum buffer length need for only ID message
#define MAX_MSG_LENGTH (4 + 1 + 1 + 4 + MAX_CIPHER_BLOCK + MAX_HASH_DIGGEST)
#define MAX_BUFFER_LENGTH(payload) (MAX_MSG_LENGTH + payload)

#define MAX_DISCONNECT_MSG_LENGTH     128
#define MAX_DISCONNECT_BUFFER_LENGTH  (4 + 1 + 1 + 4 + 4 + MAX_DISCONNECT_MSG_LENGTH + 4 + MAX_CIPHER_BLOCK + MAX_HASH_DIGGEST)
/* packlen + pad_length + msg_type + msg_code + msg_length + msg + padding + MAC */

#define MAX_DEBUG_MSG_LENGTH     60
#define MAX_DEBUG_BUFFER_LENGTH  (4 + 1 + 1 + 1 + 4 + MAX_DEBUG_MSG_LENGTH + 4  + 4 + MAX_CIPHER_BLOCK + MAX_HASH_DIGGEST)
/* packlen + pad_length + msg_type + display + msg_length + msg + language + padding + MAC */

#define SIDE(x) ((x) & 0xFF)
#define STATUS(x) ((x) >> 8)
#define SET_STATUS(x, st) (((x) & 0xFF) | ((st) << 8));


#define SSH_FREE      0
#define SSH_CLIENT    1
#define SSH_SERVER    2
#define SSH_CLOSING   3

#define IN_CLEAR      0
#define ESTABLISHED   1
#define AUTHENTICATED 2

/* interna communication */
#define PREFIX_PROC_MSG     (0xcafe)
#define NEW_KEY_SENT        (0xcafe1234)
#define NEW_KEY_RECEIVED    (0xcafe5678)
#define PROC_DIED           (0xcafebabe)

/* Device used for loading file */
#define SSH_FILES RFILESYS

/* processes */
#define SSH_DEAMON_PRIO         26
#define SSH_SERVER_IN_PRIO      26
#define SSH_SERVER_USER_PRIO    25
#define SSH_DEAMON_STACK_SIZE   4096
#define SSH_SERVER_STACK_SIZE   16384

/* ssh_rand  */
#define SSH_RESEED  10
void ssh_initrand();
void ssh_initnewctx(randctx* master, randctx* ctx);

typedef int32 sshid32;

struct sshcontrol {
	sid32   sshmutex;
	bpid32  bpin;
	bpid32  bpout;
	randctx sshctx;
};

#pragma pack(1)
struct ssh_binary_packet {
	uint32  packet_length;
	byte    padding_length;
	byte    buff[BUFFERS_LENGTH];
};
#pragma pack()

/* ssh_userauth */
#define MIN_NAME_LENGTH 1
#define MAX_NAME_LENGTH 16
#define NB_AUTH_METHODS 4

/* ssh_channel */
#define NB_CHANNELS     2
#define IS_VALID_CHANNEL(x) ((0 <= (x)) && ((x) < NB_CHANNELS))

struct sshchannel {
	int32  state;
	uint32  program;
	pid32   ppid;
	sid32   sem;
	did32   pty;
	uint32  s_channel;
	uint32  win_size_in;
	uint32  win_size_out;
	uint32  max_pcksize;
};

struct sshent {
	/* TLP layer */
	/* State - CLIENT - SERVER - FREE - CLOSING */
	/* ESTABLISHED - IN_CLEAR */
	uint16                      state;
	sid32                       sem_ent;

	/* TCP pseudo devide id. */
	int32                       tcp_id;

	/* Id of the processes */
	pid32                       in_proc;
	pid32                       user_proc;
	int32                       port;

	/* TLP buffers. */
	//    struct ssh_binary_packet    out;
	uint32                      seq_out;
	struct ssh_binary_packet    in;
	uint32                      seq_in;

	/* Key exchange. */
	int32                       kexid;
	byte                        session_id[MAX_HASH_DIGGEST];
	int32                       sid_length;

	/* public key */
	int32                       pkid;

	/* Encryption */
	cid32                       ciphid_in;
	cid32                       ciphid_out;
	byte                        iv_in[MAX_KEYS_SIZE];
	byte                        iv_out[MAX_KEYS_SIZE];
	byte*                       enckeyblob_in;
	byte*                       enckeyblob_out;

	/* MAC */
	hid32                       macid_in;
	hid32                       macid_out;
	byte                        mackey_in[MAX_KEYS_SIZE];
	byte                        mackey_out[MAX_KEYS_SIZE];

	/* compression (not used) */
	int32                       comid_in;
	int32                       comid_out;

	/* PRGN */
	randctx                     randctx;

	/* AUTH layer */
	char                        username[MAX_NAME_LENGTH];
	int32                       usize;
	int32                       auth_methods[NB_AUTH_METHODS];
	int32                       service_act;

	/* CONNECTION layer */
	struct sshchannel           channels[NB_CHANNELS];

};

struct buffer {
	byte*   buff;
	byte*   current;
	byte*   end;
};

extern struct sshent sshtab[];
extern struct sshcontrol Ssh;
/* tests */
void launch_sshtests();

/* Utilities functions. */
void xdump_binary_packet(struct ssh_binary_packet *pck, bool8 enc, int32 len);
void xdump_array(byte* array, int32 length);
uint32 strcopy(byte* dst, const char* src);


/* ssh_data. */
int32 ssh_write_string(byte *buff, const char* list, int32 len);
int32 ssh_read_string(byte *buff, char *dst, int len);
#define ssh_read_mpint(buff, dst) ntoh_mpint(dst, buff)
//int32 ssh_read_mpint(byte *buff, MPINT *dst);
#define ssh_write_mpint(buff, mpint) hton_mpint(buff, mpint, 0x7FFFFFFF)
//int32 ssh_write_mpint(byte *buff, MPINT *mpint);
int32 ssh_write_list(byte *buff, char** list, int32 len);
int32 ssh_write_list_part(byte *buff, char** list, int32* send, int32 len);

/* ssh_init. */
bool8 ssh_init();


/* ssh_exchange_version. */
#define SSH_VERSION "SSH-2.0-XinuSSH_1.0\r\n"
bool8 ssh_version_exchange(struct sshent *sshb, struct buffer *kex_buff);

/* ssh_tlp */

#define MIN_PADDING 4
#define HEADER_SIZE 5
#define ispowerof2(x) (((x) & (x - 1)) == 0)

bool8 ssh_tlp_out(struct sshent *sshb, struct ssh_binary_packet* out, uint32 payload_length);
bool8 ssh_tlp_in(struct sshent *sshb, struct ssh_binary_packet* in);

/* ssh_connect */

sshid32 ssh_connect(uint32 addr, uint16 port);
sshid32 ssh_accept(int32 tcp_id);

/* Algorithms negociation */
#define KEX_ALGO      0
#define SER_HOST_ALGO 1
#define ENC_ALGO_CS   2
#define ENC_ALGO_SC   3
#define MAC_ALGO_CS   4
#define MAC_ALGO_SC   5
#define COM_ALGO_CS   6
#define COM_ALGO_SC   7


bool8 ssh_algorithms_negociation(struct sshent *sshb, struct buffer* kex_buff);

/* algorithms_negociation_parse */
int32 parse_kex_algo(char* name, int32 len);
int32 parse_pke_algo(char* name, int32 len);
int32 parse_enc_algo(char * name, int32 len);
int32 parse_mac_algo(char * name, int32 len);
int32 parse_com_algo(char * name, int32 len);

/* ssh_kex */

#define DHG1	(0x1)
#define DHG14	(0x2)

bool8 ssh_key_exchange(struct sshent* sshb, struct buffer* kex_buff);
bool8 ssh_generate_key(struct sshent* sshb, byte* shared_secret, int32 length, byte* H, hid32 hashid);
bool8 allocate_buff(struct buffer* kbuff, int32 size);
void clear_buffer(struct buffer* kbuff);
bool8 append_buff(struct buffer* buff, byte *tab, int32 len);
bool8 addint_buff(struct buffer* buff, int32 len);

/* ssh_dh */
bool8 c_diffie_hellman(struct sshent *sshb, struct buffer* kex_buff, DH_GROUPMP* group, hid32 hashid);
bool8 s_diffie_hellman(struct sshent *sshb, struct buffer* kex_buff, DH_GROUPMP* group, hid32 hashid);

/* public key exchange */
#define DSS	1
#define RSA	2

/* ssh_dss */
int32 read_dsscert(byte *buff, struct dsacert* certificate);
int32 write_dsscert(byte *buff, struct dsacert* certificate);
int32 read_dsssign(byte* buff, struct dsasign* sign);
int32 write_dsssign(byte* buff, struct dsasign* sign);
void delete_dsscert(struct dsacert* cert);

/* ssh_keys */
#define KEY_FOLDER	("keys/")
#define DSA_KEY		("dsa.pem")

extern struct dsacert xinu_DSAcert;
extern struct dsapriv xinu_DSApriv;
bool8 ssh_load_dsakey(struct dsapriv* key, struct dsacert* cert, char* dir, char* filename);


/* ssh_disconnect */
void ssh_disconnect(struct sshent* sshb, uint32 code, char* msg, int32 length);
void ssh_close(struct sshent* sshb);
void ssh_clean_entry(struct sshent *sshb);
void ssh_stop_userproc(struct sshent* sshb, umsg32 msg);

/* ssh_server */
process ssh_deamon();

/* ssh_msg */
/* handle received */
void ssh_msg_debug(struct ssh_binary_packet* in);
void ssh_msg_service_request(struct sshent* sshb, struct ssh_binary_packet* in);
void ssh_msg_disconnect(struct sshent* sshb, struct ssh_binary_packet* in);
#define SSH_NEWKEY_TIMEOUT	3000
void ssh_msg_newkeys(struct sshent* sshb, struct ssh_binary_packet* in);
/* send */
void ssh_send_msg_userauth_banner(struct sshent* sshb);
void ssh_send_msg(struct sshent* sshb, int32 msg_type);

/* ssh_service */
#define MAX_SERVICE_LENGTH	20
#define NB_SERVICES			3
#define CONNECTION			1
#define USER_AUTH			2

extern char* services[];

int32 ssh_service_id(char* in);
process ssh_service_accept(struct sshent* sshb);

/* ssh_userauth */
#define MAX_AUTH_METHOD_LENGTH	16

#define PASSWORD	1
#define PUBKEY		2

bool8 ssh_userauth(struct sshent* sshb);

// these values are for testing
#define MAX_AUTH_TRIES			3
#define AUTH_TIMEOUT_TOT_SEC	(1 * 60)
#define AUTH_TIMEOUT_LAP_MS		(1000)

/* ssh_userauth_password */
#define MIN_PASSWORD_LENGTH	4
#define MAX_PASSWORD_LENGTH	64
#define SALT_SIZE			8

#define PASSWD_FILE			("./passwd")
#define PASSWD_FILE_BACK	("./.passwd.back")
#define SHADOW_FILE			("./shadow")
#define SHADOW_FILE_BACK	("./.shadow.back")

struct pwent {
	byte  salt[SALT_SIZE];
	byte  pwhash[SHA1_DIGGEST_SIZE];
};

bool8 ssh_password_check(struct sshent* sshb, struct ssh_binary_packet* in, int32 nbytes, struct pwent* pw);

/* ssh_connection */
bool8 ssh_connection(struct sshent* sshb);

/* ssh_channels */
#define CHANNEL_TYPE_MAXLEN	8

#define NB_CHANNEL_TYPES	2

#define CHANNEL_CLOSED		(-2)
#define CHANNEL_CLOSING		(-1)
#define CHANNEL_FREE		0
#define CHANNEL_SESSION		1

#define CHANNEL_INIT_WINSIZE	1048576
#define CHANNEL_MAX_MSGSIZE		8196
#define CHANNEL_WINSIZE_THRES	(CHANNEL_MAX_MSGSIZE * 2)
#define CHANNEL_WINSIZE_ADD		CHANNEL_INIT_WINSIZE


#define NB_REQUESTS			3
#define REQUEST_MAXLEN		16

//#define NONE      0
#define CHANNEL_PTY			1
#define CHANNEL_SHELL		2

struct ssh_ptyarg {
	struct sshent*	sshb;
	int32			channel;
};



void ssh_channel_open_handler(struct sshent* sshb, struct ssh_binary_packet* in);
void ssh_channel_data_handler(struct sshent* sshb, struct ssh_binary_packet* in);
void ssh_channel_windowadjust(struct sshent* sshb, struct ssh_binary_packet* in);
void ssh_channel_close(struct sshent* sshb, struct ssh_binary_packet* in);
void close_channel(struct sshent* sshb, struct sshchannel* sshc);

/* ssh_requests */

void ssh_global_request_handler(struct sshent* sshb, struct ssh_binary_packet* in);
void ssh_channel_request_handler(struct sshent* sshb, struct ssh_binary_packet* in);
void ssh_channel_request_send_controlflow(struct sshent* sshb, struct sshchannel* sshc, bool8 onoff);
/* pty option */
#define NB_SSHPTY   1
did32 ssh_allocate_pty(struct ssh_ptyarg* args);
void ssh_ptyin(did32 pty, byte* buff, int32 length);

#define SSH_SHELL_PRIO  20
#define SSH_SHELL_STACK_SIZE  16384

#define MAX_PCKSIZE 1024

/* ssh_shell */
void ssh_createshell(struct sshent* sshb, struct sshchannel* sshc);

#endif // SSH_H
