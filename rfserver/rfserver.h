/* rfserver.h - definintions for the remote file server (on Unix) */


extern	int	sock;			/* UDP socket used to receive	*/
					/*   requests & send responses	*/

extern	struct	sockaddr_in senderip;	/* the sender's address taken	*/
					/*   the current UDP packet	*/

extern	int addrlen;			/* size of the above address	*/

#define	MAXFILES	64		/* maximum open files		*/

#define	MAXMSG		2048		/* maximum message size		*/

struct	fentry {			/* entry in table of open files	*/
	int	desc;			/* descriptor or -1 if not used	*/
	DIR	*dirptr;		/* Pointer to open directory	*/
	char	name[RF_NAMLEN+1];	/* null-terminated file name	*/
};
extern	struct	fentry ofiles[MAXFILES];/* open file table		*/
extern	int	findex;			/* index of file in ofiles	*/
extern	struct	fentry	*fptr;		/* pointer to entry in ofiles	*/
extern	int	fnext;			/* index of next entry to kill	*/
					/* when	table is full		*/

extern	int	sock;			/* UDP socket used to receive	*/
					/*   requests & send responses	*/
extern	struct	sockaddr_in senderip;	/* the sender's address taken	*/
					/*   the current UDP packet	*/

extern	int		passiveUDP(const char *service);
extern	void		snderr(	struct rf_msg_hdr *,
				struct rf_msg_hdr *,
				int);
extern	void		sndok(	struct rf_msg_hdr *,
				struct rf_msg_hdr *,
				int);
extern	void	rsread  (struct rf_msg_rreq *, struct rf_msg_rres *);
extern	void	rswrite (struct rf_msg_wreq *, struct rf_msg_wres *);
extern	void	rsopen  (struct rf_msg_oreq *, struct rf_msg_ores *);
extern	void	rsdelete(struct rf_msg_dreq *, struct rf_msg_dres *);
extern	void	rstrunc (struct rf_msg_treq *, struct rf_msg_tres *);
extern	void	rsstat  (struct rf_msg_sreq *, struct rf_msg_sres *);
extern	void	rsmkdir (struct rf_msg_mreq *, struct rf_msg_mres *);
extern	void	rsrmdir (struct rf_msg_xreq *, struct rf_msg_xres *);
extern	int	rsofile	(char *, int);
