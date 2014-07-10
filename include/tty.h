/* tty.h */

#define	TY_OBMINSP		20	/* min space in buffer before	*/
					/* processes awakened to write	*/
#define	TY_EBUFLEN		20	/* size of echo queue		*/

/* Size constants */

#ifndef	Ntty
#define	Ntty		1		/* number of serial tty lines	*/
#endif
#ifndef	TY_IBUFLEN
#define	TY_IBUFLEN	128		/* num.	chars in input queue	*/
#endif
#ifndef	TY_OBUFLEN
#define	TY_OBUFLEN	64		/* num.	chars in output	queue	*/
#endif

/* Mode constants for input and output modes */

#define	TY_IMRAW	'R'		/* raw mode => nothing done	*/
#define	TY_IMCOOKED	'C'		/* cooked mode => line editing	*/
#define	TY_IMCBREAK	'K'		/* honor echo, etc, no line edit*/
#define	TY_OMRAW	'R'		/* raw mode => normal processing*/

struct	ttycblk	{			/* tty line control block	*/
	char	*tyihead;		/* next input char to read	*/
	char	*tyitail;		/* next slot for arriving char	*/
	char	tyibuff[TY_IBUFLEN];	/* input buffer (holds one line)*/
	sid32	tyisem;			/* input semaphore		*/
	char	*tyohead;		/* next output char to xmit	*/
	char	*tyotail;		/* next slot for outgoing char	*/
	char	tyobuff[TY_OBUFLEN];	/* output buffer		*/
	sid32	tyosem;			/* output semaphore		*/
	char	*tyehead;		/* next echo char to xmit	*/
	char	*tyetail;		/* next slot to deposit echo ch	*/
	char	tyebuff[TY_EBUFLEN];	/* echo buffer			*/
	char	tyimode;		/* input mode raw/cbreak/cooked	*/
	bool8	tyiecho;		/* is input echoed?		*/
	bool8	tyieback;		/* do erasing backspace on echo?*/
	bool8	tyevis;			/* echo control chars as ^X ?	*/
	bool8	tyecrlf;		/* echo CR-LF for newline?	*/
	bool8	tyicrlf;		/* map '\r' to '\n' on input?	*/
	bool8	tyierase;		/* honor erase character?	*/
	char	tyierasec;		/* erase character (backspace)	*/
	bool8	tyeof;			/* honor EOF character?		*/
	char	tyeofch;		/* EOF character (usually ^D)	*/
	bool8	tyikill;		/* honor line kill character?	*/
	char	tyikillc;		/* line kill character		*/
	int32	tyicursor;		/* current cursor position	*/
	bool8	tyoflow;		/* honor ostop/ostart?		*/
	bool8	tyoheld;		/* output currently being held?	*/
	char	tyostop;		/* character that stops output	*/
	char	tyostart;		/* character that starts output	*/
	bool8	tyocrlf;		/* output CR/LF for LF ?	*/
	char	tyifullc;		/* char to send when input full	*/
};
extern	struct	ttycblk	ttytab[];

/* Characters with meaning to the tty driver */

#define	TY_BACKSP	'\b'		/* Backspace character		*/
#define	TY_BELL		'\07'		/* Character for audible beep	*/
#define	TY_EOFCH	'\04'		/* Control-D is EOF on input	*/
#define	TY_BLANK	' '		/* Blank			*/
#define	TY_NEWLINE	'\n'		/* Newline == line feed		*/
#define	TY_RETURN	'\r'		/* Carriage return character	*/
#define	TY_STOPCH	'\023'		/* Control-S stops output	*/
#define	TY_STRTCH	'\021'		/* Control-Q restarts output	*/
#define	TY_KILLCH	'\025'		/* Control-U is line kill	*/
#define	TY_UPARROW	'^'		/* Used for control chars (^X)	*/
#define	TY_FULLCH	TY_BELL		/* char to echo when buffer full*/

/* Tty control function codes */

#define	TC_NEXTC	3		/* look ahead 1 character	*/
#define	TC_MODER	4		/* set input mode to raw	*/
#define	TC_MODEC	5		/* set input mode to cooked	*/
#define	TC_MODEK	6		/* set input mode to cbreak	*/
#define	TC_ICHARS	8		/* return number of input chars	*/
#define	TC_ECHO		9		/* turn on echo			*/
#define	TC_NOECHO	10		/* turn off echo		*/
