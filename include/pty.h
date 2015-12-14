/* pty.h */

#define	PTY_OBMINSP	20		/* Min space in buffer before	*/
/*   processes awakened to write*/
#define	PTY_EBUFLEN	20		/* Size of echo queue		*/

/* Size constants */

#ifndef	Npty
#define	Npty        1		/* Number of serial pty lines	*/
#endif
#ifndef	PTY_IBUFLEN
#define	PTY_IBUFLEN 128		/* Num. chars in input queue	*/
#endif
#ifndef	PTY_OBUFLEN
#define	PTY_OBUFLEN 64		/* Num.	chars in output	queue	*/
#endif

/* Mode constants for input and output modes */

#define	PTY_IMRAW       'R'		/* Raw input mode => no edits	*/
#define	PTY_IMCOOKED    'C'		/* Cooked mode => line editing	*/
#define	PTY_IMCBREAK    'K'		/* Honor echo, etc, no line edit*/
#define	PTY_OMRAW       'R'		/* Raw output mode => no edits	*/

typedef void (* OUT_PTY) (void* arg, byte* buff, int32 size);

struct	ptycblk	{			/* Pty line control block	*/
	char	*pyihead;		/* Next input char to read	*/
	char	*pyitail;		/* Next slot for arriving char	*/
	char	pyibuff[PTY_IBUFLEN];	/* Input buffer (holds one line)*/
	sid32	pyisem;			/* Input semaphore		*/
	OUT_PTY out;
	void*   arg_out;
	char	pyimode;		/* Input mode raw/cbreak/cooked	*/
	bool8	pyiecho;		/* Is input echoed?		*/
	bool8	pyieback;		/* Do erasing backspace on echo?*/
	bool8	pyevis;			/* Echo control chars as ^X ?	*/
	bool8	pyecrlf;		/* Echo CR-LF for newline?	*/
	bool8	pyicrlf;		/* Map '\r' to '\n' on input?	*/
	bool8	pyierase;		/* Honor erase character?	*/
	char	pyierasec;		/* Primary erase character	*/
	char	pyierasec2;		/* Alternate erase character	*/
	bool8	pyeof;			/* Honor EOF character?		*/
	char	pyeofch;		/* EOF character (usually ^D)	*/
	bool8	pyikill;		/* Honor line kill character?	*/
	char	pyikillc;		/* Line kill character		*/
	int32	pyicursor;		/* Current cursor position	*/
	bool8	pyoflow;		/* Honor ostop/ostart?		*/
	bool8	pyoheld;		/* Output currently being held?	*/
	char	pyostop;		/* Character that stops output	*/
	char	pyostart;		/* Character that starts output	*/
	bool8	pyocrlf;		/* Output CR/LF for LF ?	*/
	char	pyifullc;		/* Char to send when input full	*/
};
extern	struct	ptycblk	ptytab[];

/* Characters with meaning to the pty driver */

#define	PTY_BACKSP	'\b'		/* Backspace character		*/
#define	PTY_BACKSP2	'\177'		/* Alternate backspace char.	*/
#define	PTY_BELL		'\07'		/* Character for audible beep	*/
#define	PTY_EOFCH	'\04'		/* Control-D is EOF on input	*/
#define	PTY_BLANK	' '		/* Blank			*/
#define	PTY_NEWLINE	'\n'		/* Newline == line feed		*/
#define	PTY_RETURN	'\r'		/* Carriage return character	*/
#define	PTY_STOPCH	'\023'		/* Control-S stops output	*/
#define	PTY_STRTCH	'\021'		/* Control-Q restarts output	*/
#define	PTY_KILLCH	'\025'		/* Control-U is line kill	*/
#define	PTY_UPARROW	'^'		/* Used for control chars (^X)	*/
#define	PTY_FULLCH	TY_BELL		/* Char to echo when buffer full*/

/* Pty control function codes */

#define	PC_NEXTC        3		/* Look ahead 1 character	*/
#define	PC_MODER        4		/* Set input mode to raw	*/
#define	PC_MODEC        5		/* Set input mode to cooked	*/
#define	PC_MODEK        6		/* Set input mode to cbreak	*/
#define	PC_ICHARS       8		/* Return number of input chars	*/
#define	PC_ECHO         9		/* Turn on echo			*/
#define	PC_NOECHO       10		/* Turn off echo		*/
#define PC_OUTPUT_FUNC  11      /* Set ouput function */
#define PC_RESET        12      /* Set ouput function */
