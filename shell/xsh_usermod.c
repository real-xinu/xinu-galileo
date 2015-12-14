/* xsh_usermod.c - xsh_usermod */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

bool8 new_user(char* username, char* password);
bool8 delete_user(char* username);

/* from the ssh_userauth.c file */
int32 write_file(char* filename, bool8 new, byte* buff, int32 size);
int32 find_user(char* username, byte* buff, int32 buffsize, int32* bread);
int32 load_file(char* filename, byte** buff, int32 extra);

/* from the ssh_userauth_password.c file */
void generate_hash(byte* salt, char* username, int32 usize, char* password, int32 psize, byte* diggest);


/*------------------------------------------------------------------------
 * xsh_usermod - manage user accounts
 *------------------------------------------------------------------------
 */
shellcmd xsh_usermod(int nargs, char *args[])
{
	/* For argument '--help', emit help about the 'usermod' command	*/

	if (nargs > 4 || (nargs == 2 && strncmp(args[1], "--help", 7) == 0)) {
		printf("Use: %s [username password]\n\n", args[0]);
		printf("Description:\n");
		printf("\tManage users accounts\n");
		printf("\t-a [username] [password]\tadd user\n");
		printf("\t-d [username]\t\t\tdelete user\n");
		return 0;
	}

	if (nargs == 4 && strncmp(args[1], "-a", 2) == 0) {
		if (new_user(args[2], args[3]) == TRUE) {
			printf("User %s added with success\n", args[2]);
		}
		return 0;
	}

	if (nargs == 3 && strncmp(args[1], "-d", 2) == 0) {
		if (delete_user(args[2]) == TRUE) {
			printf("User %s removed with success\n", args[2]);
		}
		return 0;
	}

	fprintf(stderr, "%s: too many arguments\n", args[0]);
	fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
	return 1;
}

bool8 does_file_exist(char* filename) {
	int32 file;

	if ((file = open(SSH_FILES, filename, "ro")) == SYSERR) {
		return FALSE;
	} else {
		close(file);
		return TRUE;
	}
}

int32 delete_file(char* filename) {
	int32 file;

	if ((file = open(SSH_FILES, filename, "wo")) == SYSERR) {
		return SYSERR;
	}

	// Read complete file
	if (control(SSH_FILES, RFS_CTL_DEL, file, 0) == SYSERR) {
		close(file);
		return SYSERR;
	}
	close(file);
	return OK;
}



bool8 new_user(char* username, char* password) {
	byte* fptr;
	int32 fsize;
	int32 line, bread;
	int32 usize, psize;
	int32 i;
	struct pwent pw;
	int32 entrysize;

	// /* Check if a backup file exists */
	// if (does_file_exist(PASSWD_FILE_BACK)) {
	//     fprintf(stderr, "Error: A backup file exists for the PASSWORD file.\nPlease take necessary action\n");
	//     return FALSE;
	// }

	// if (does_file_exist(SHADOW_FILE_BACK)) {
	//     fprintf(stderr, "Error: A backup file exists for the SHADOW file.\nPlease take necessary action\n");
	//     return FALSE;
	// }

	/* argument check */
	if ((usize = strnlen(username, MAX_NAME_LENGTH)) == MAX_NAME_LENGTH || \
			usize < MIN_NAME_LENGTH || \
			(psize = strnlen(password, MAX_PASSWORD_LENGTH)) == MAX_PASSWORD_LENGTH || \
			psize < MIN_PASSWORD_LENGTH) {
		fprintf(stderr, "Error: username or/and password length incorrects\n"
				"\tUsername length: between %d and %d\n"
				"\tPassword length between %d and %d\n",
				MIN_NAME_LENGTH, MAX_NAME_LENGTH, MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
		return FALSE;
	}

	/* load password file with extra for new user */
	if ((fsize = load_file(PASSWD_FILE, &fptr, usize + 1)) == SYSERR) {
		fprintf(stderr, "Error: can not load PASSWORD file\n");
		return FALSE;
	}

	/* Verify if the user is already there */
	if ((line = find_user(username, fptr, fsize, &bread)) & 0x80000000) {
		fprintf(stderr, "Error: username '%s' already exists\n", username);
		freemem((char *)fptr, fsize + usize + 1);
		return FALSE;
	}

	/* create a backup copy */
	if (write_file(PASSWD_FILE_BACK, TRUE, fptr, fsize) == SYSERR) {
		fprintf(stderr, "Error: can not backup PASSWORD file\n");
		freemem((char *)fptr, fsize + usize + 1);
		return FALSE;
	}

	/* insert new name */
	for (i = fsize  - 1; i >= bread ; i--) {
		fptr[i + usize + 1] = fptr[i];
	}
	for (i = 0 ; i < usize ; i++) {
		fptr[bread + i] = username[i];
	}
	fptr[bread + i] = '\n';

	/* write the file back */
	if (write_file(PASSWD_FILE, FALSE, fptr, fsize + usize + 1) == SYSERR) {
		fprintf(stderr, "Error: can not write to PASSWORD file.\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		freemem((char *)fptr, fsize + usize + 1);
		return FALSE;
	}
	freemem((char *)fptr, fsize + usize + 1);

	/* generate salt */
	get_urandom(pw.salt, SALT_SIZE);

	/* generate hash */
	generate_hash(pw.salt, username, usize, password, psize, pw.pwhash);

	/* Update the shadow file with extra room for new entry */
	entrysize = BASE64_ENCODED_SIZE(sizeof(struct pwent)) + 1;
	if ((fsize = load_file(SHADOW_FILE, &fptr, entrysize)) == SYSERR) {
		fprintf(stderr, "Error: can not load SHADOW file.\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		return FALSE;
	}

	/* create a backup copy */
	if (write_file(SHADOW_FILE_BACK, TRUE, fptr, fsize) == SYSERR) {
		fprintf(stderr, "Error: can not backup SHADOW file\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		freemem((char *)fptr, fsize + entrysize);
		return FALSE;
	}

	/* insert new entry */
	kprintf("line %d\n", line);
	for (i = fsize - 1 ; i >= line * entrysize ; i--) {
		fptr[i + entrysize] = fptr[i];
	}
	base64_encode((byte *) (&pw), sizeof(struct pwent), &(fptr[line * entrysize]));
	fptr[i + entrysize] = '\n';

	/* write the file back */
	if (write_file(SHADOW_FILE, FALSE, fptr, fsize + entrysize) == SYSERR) {
		fprintf(stderr, "Error: can not write to SHADOW file.\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		fprintf(stderr, "\tPlease restore SHADOW file (mv %s %s)\n", SHADOW_FILE_BACK, SHADOW_FILE);
		freemem((char *)fptr, fsize + entrysize);
		return FALSE;
	}
	freemem((char *)fptr, fsize + entrysize);

	delete_file(SHADOW_FILE_BACK);
	delete_file(PASSWD_FILE_BACK);

	return TRUE;
}

bool8 delete_user(char* username) {
	byte* fptr;
	int32 fsize;
	int32 line, bread;
	int32 usize;
	int32 i;
	int32 entrysize;

	/* Check if a backup file exists */
	if (does_file_exist(PASSWD_FILE_BACK)) {
		fprintf(stderr, "Error: A backup file exists for the PASSWORD file.\nPlease take necessary action\n");
		return FALSE;
	}

	if (does_file_exist(SHADOW_FILE_BACK)) {
		fprintf(stderr, "Error: A backup file exists for the SHADOW file.\nPlease take necessary action\n");
		return FALSE;
	}

	/* argument check */
	if ((usize = strnlen(username, MAX_NAME_LENGTH)) == MAX_NAME_LENGTH || \
			usize < MIN_NAME_LENGTH) {
		fprintf(stderr, "username '%s' doesn't exist\n", username);
		return FALSE;
	}

	/* load password file */
	if ((fsize = load_file(PASSWD_FILE, &fptr, 0)) == SYSERR) {
		fprintf(stderr, "Error: can not load PASSWORD file\n");
		return FALSE;
	}

	/* Verify that the user is already there */
	if (!((line = find_user(username, fptr, fsize, &bread)) & 0x80000000)) {
		fprintf(stderr, "username '%s' doesn't exist\n", username);
		freemem((char *)fptr, fsize);
		return FALSE;
	}

	/* create a backup copy */
	if (write_file(PASSWD_FILE_BACK, TRUE, fptr, fsize) == SYSERR) {
		fprintf(stderr, "Error: can not backup PASSWORD file\n");
		freemem((char *)fptr, fsize);
		return FALSE;
	}

	/* remove name */
	for (i = bread; i < fsize - usize - 1 ; i++) {
		fptr[i] = fptr[usize + 1 + i];
	}


	/* write the file back */
	if (delete_file(PASSWD_FILE) == SYSERR || \
			write_file(PASSWD_FILE, TRUE, fptr, fsize - (usize + 1)) == SYSERR) {
		fprintf(stderr, "Error: can not write to PASSWORD file.\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		freemem((char *)fptr, fsize);
		return FALSE;
	}
	freemem((char *)fptr, fsize);

	/* Update the shadow file */
	entrysize = BASE64_ENCODED_SIZE(sizeof(struct pwent)) + 1;
	if ((fsize = load_file(SHADOW_FILE, &fptr, 0)) == SYSERR) {
		fprintf(stderr, "Error: can not load SHADOW file.\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		return FALSE;
	}

	/* create a backup copy */
	if (write_file(SHADOW_FILE_BACK, TRUE, fptr, fsize) == SYSERR) {
		fprintf(stderr, "Error: can not backup SHADOW file\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		freemem((char *)fptr, fsize);
		return FALSE;
	}

	/* remove entry */
	for (i = line * entrysize ; i < fsize - entrysize ; i++) {
		fptr[i] = fptr[i + entrysize];
	}

	/* write the file back */
	if (delete_file(SHADOW_FILE) == SYSERR || \
			write_file(SHADOW_FILE, TRUE, fptr, fsize - entrysize) == SYSERR) {
		fprintf(stderr, "Error: can not write to SHADOW file.\n");
		fprintf(stderr, "\tPlease restore PASSWORD file (mv %s %s)\n", PASSWD_FILE_BACK, PASSWD_FILE);
		fprintf(stderr, "\tPlease restore SHADOW file (mv %s %s)\n", SHADOW_FILE_BACK, SHADOW_FILE);
		freemem((char *)fptr, fsize);
		return FALSE;
	}
	freemem((char *)fptr, fsize);

	delete_file(SHADOW_FILE_BACK);
	delete_file(PASSWD_FILE_BACK);

	return TRUE;
}
