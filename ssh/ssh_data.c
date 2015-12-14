#include <xinu.h>

/*-------------------------------------------------------------------------------------
 * ssh_write_string - formats as the SSH spec and writes the string into a buffer.
 *                  - returns the number of bytes written.
 *-------------------------------------------------------------------------------------
 */
int32 ssh_write_string(byte *buff, const char* string, int32 len) {
	int32 i;

	/* Length */
	*((uint32 *) buff) = (uint32)htonl(len);
	buff += 4;

	/* add string */
	for (i = 0 ; i < len ; ++i)
		*buff++ = *string++;

	return len + 4;

}

/*-------------------------------------------------------------------------------------
 * ssh_read_string - reads the string formatted as the SSH spec from a buffer up to
 *                   len character
 *                 - returns the number of bytes read if len was bigger than the
 *                   actual length (!! length + 4)
 *-------------------------------------------------------------------------------------
 */
int32 ssh_read_string(byte *buff, char* dst, int len) {
	int32 i;
	uint32 length;

	length = (uint32)ntohl(*((uint32 *) buff));
	buff += 4;

	if (len > length) {
		i = length;
		dst[length] = '\0';
	} else
		i = len;


	for (; i > 0 ; --i)
		*dst++ = *buff++;

	return length + 4;
}

/*-------------------------------------------------------------------------------------
 * ssh_write_mpint - formats as the SSH spec and writes the mpint into a buffer.
 *                 - returns the number of bytes written.
 *-------------------------------------------------------------------------------------
 */
//int32 ssh_write_mpint(byte *buff, MPINT* mpint) {
//
//    // Buff size is not checked here
//    return hton_mpint(buff, mpint, 0x7FFFFFFF);
//}

/*-------------------------------------------------------------------------------------
 * ssh_read_mpint - reads the string formatted as the SSH spec from a buffer.
 *                - returns the number of bytes read (!! length + 4)
 *-------------------------------------------------------------------------------------
 */
//int32 ssh_read_mpint(byte *buff, MPINT* dst) {
//    return ntoh_mpint(dst, buff);
//}

/*-------------------------------------------------------------------------------------
 * ssh_write_list - formats and writes the name list into the buffer
 *                - returns the number of byte written.
 *-------------------------------------------------------------------------------------
 */
int32 ssh_write_list(byte *buff, char** list, int32 len) {
	uint32 *length = (uint32 *)buff;
	uint32 i, ret;

	/* Length space */
	buff += 4;

	/* add list of string */
	for (i = 0 ; i < len ; i++) {
		buff += strcopy(buff, list[i]);
		*(buff++) = ',';
	}
	ret = buff - (byte *)length - (len > 0 ? 5 : 4);
	*length = (uint32)htonl(ret);

	return ret + 4;

}

/*-------------------------------------------------------------------------------------
 * ssh_write_list_part - formats and writes the name list into the buffer
 *                     - returns the number of byte written.
 *-------------------------------------------------------------------------------------
 */
int32 ssh_write_list_part(byte *buff, char** list, int32* send, int32 len) {
	uint32 *length = (uint32 *)buff;
	uint32 i, ret;

	/* Length space */
	buff += 4;

	/* add list of string */
	for (i = 0 ; i < len ; i++) {
		if (send[i] > 0) {
			buff += strcopy(buff, list[i]);
			*(buff++) = ',';
		}
	}
	/* compute length (remove last comma if any) */
	ret = buff - (byte *)length - (len > 0 ? 5 : 4);

	/* update the length */
	*length = (uint32)htonl(ret);

	return ret + 4;

}
