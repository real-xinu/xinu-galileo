/* lexan.c  - lexan */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lexan  -  Ad hoc lexical analyzer to divide command line into tokens
 *------------------------------------------------------------------------
 */

int32	lexan (
	  char		*line,		/* Input line terminated with	*/
					/*   NEWLINE or NULLCH		*/
	  int32		len,		/* Length of the input line,	*/
					/*   including NEWLINE		*/
	  char		*tokbuf,	/* Buffer into which tokens are	*/
					/*   stored with a null		*/
					/*   following each token	*/
	  int32		tok[],		/* Array of pointers to the	*/
					/*   start of each token	*/
	  int32		toktyp[]	/* Array that gives the type	*/
					/*   of each token		*/
	)
{
	char	quote;			/* Character for quoted string  */
	uint32	ntok;			/* Number of tokens found	*/
	char	*p;			/* Pointer that walks along the	*/
					/*   input line			*/
	int32	tbindex;		/* Index into tokbuf		*/
	char	ch;			/* Next char from input line	*/

	/* Start at the beginning of the line with no tokens */

	ntok = 0;
	p = line;
	tbindex = 0;

	/* While not yet at end of line, get next token */

	while ( (*p != NULLCH) && (*p != SH_NEWLINE) ) {

		/* If too many tokens, return error */

		if (ntok >= SHELL_MAXTOK) {
			return SYSERR;
		}

		/* Skip white space before token */

		while ( (*p == SH_BLANK) || (*p == SH_TAB) ) {
			p++;
		}

		/* Stop parsing at end of line (or end of string) */

		ch = *p;
		if ( (ch==SH_NEWLINE) || (ch==NULLCH) ) {
			return ntok;
		}

		/* Set next entry in tok array to be an index to the	*/
		/*   current location in the token buffer		*/

		tok[ntok] = tbindex;	/* The start of the token	*/

		/* Set the token type */

		switch (ch) {

		    case SH_AMPER:	toktyp[ntok] = SH_TOK_AMPER;
					tokbuf[tbindex++] = ch;
					tokbuf[tbindex++] = NULLCH;
					ntok++;
					p++;
					continue;

		    case SH_PIPE:	toktyp[ntok] = SH_TOK_PIPE;
					tokbuf[tbindex++] = ch;
					tokbuf[tbindex++] = NULLCH;
					ntok++;
					p++;
					continue;

		    case SH_LESS:	toktyp[ntok] = SH_TOK_LESS;
					tokbuf[tbindex++] = ch;
					tokbuf[tbindex++] = NULLCH;
					ntok++;
					p++;
					continue;

		    case SH_GREATER:	toktyp[ntok] = SH_TOK_GREATER;
					tokbuf[tbindex++] = ch;
					tokbuf[tbindex++] = NULLCH;
					ntok++;
					p++;
					continue;

		    default:		toktyp[ntok] = SH_TOK_OTHER;
		};

		/* Handle quoted string (single or double quote) */

		if ( (ch==SH_SQUOTE) || (ch==SH_DQUOTE) ) {
			quote = ch;	/* Remember opening quote */

			/* Copy quoted string to arg area */

			p++;	/* Move past starting quote */

			while ( ((ch=*p++) != quote) && (ch != SH_NEWLINE)
					&& (ch != NULLCH) ) {
				tokbuf[tbindex++] = ch;
			}
			if (ch != quote) {  /* String missing end quote	*/
				return SYSERR;
			}

			/* Finished string - count token and go on	*/

			tokbuf[tbindex++] = NULLCH; /* Terminate token	*/
			ntok++;		/* Count string as one token	*/
			continue;	/* Go to next token		*/
		}

		/* Handle a token other than a quoted string		*/

		tokbuf[tbindex++] = ch;	/* Put first character in buffer*/
		p++;

		while ( ((ch = *p) != SH_NEWLINE) && (ch != NULLCH)
			&& (ch != SH_LESS)  && (ch != SH_GREATER)
			&& (ch != SH_BLANK) && (ch != SH_TAB)
			&& (ch != SH_AMPER) && (ch != SH_SQUOTE)
			&& (ch != SH_DQUOTE)&& (ch != SH_PIPE))	{
				tokbuf[tbindex++] = ch;
				p++;
		}

		/* Report error if other token is appended */

		if (       (ch == SH_SQUOTE) || (ch == SH_DQUOTE)
			|| (ch == SH_LESS)   || (ch == SH_GREATER) ) {
			return SYSERR;
		}

		tokbuf[tbindex++] = NULLCH;	/* Terminate the token	*/

                ntok++;				/* Count valid token	*/

	}
	return ntok;
}
