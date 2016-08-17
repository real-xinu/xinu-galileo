/* rdserver.h - definintions for the remote file server (on Unix) */


extern	int	sock;			/* UDP socket used to receive	*/
					/*   requests & send responses	*/

extern	struct	sockaddr_in senderip;	/* the sender's address taken	*/
					/*   the current UDP packet	*/

extern	int addrlen;			/* size of the above address	*/

#define	MAXDISKS	64		/* maximum open "disks"		*/

#define	MAXMSG		1024		/* maximum message size		*/

struct	fentry {			/* entry in table of open disks	*/
	int	desc;			/* descriptor or -1 if not used	*/
	char	name[RD_IDLEN+1];	/* null-terminated disk ID	*/
};
extern	struct	fentry ofiles[MAXDISKS];/* open file table (one file	*/
					/*  per open disk)		*/
extern	int	findex;			/* index of file in ofiles	*/
extern	struct	fentry	*fptr;		/* pointer to entry in ofiles	*/
extern	int	fnext;			/* index of next entry to kill	*/
					/* when	table is full		*/

extern	int	sock;			/* UDP socket used to receive	*/
					/*   requests & send responses	*/
extern	struct	sockaddr_in senderip;	/* the sender's address taken	*/
					/*   the current UDP packet	*/

extern	void		snderr(	struct rd_msg_hdr *,
				struct rd_msg_hdr *,
				int);
extern	void		sndok(	struct rd_msg_hdr *,
				struct rd_msg_hdr *,
				int);
extern	void	rdread  (struct rd_msg_rreq *, struct rd_msg_rres *);
extern	void	rdwrite (struct rd_msg_wreq *, struct rd_msg_wres *);
extern	void	rdopen  (struct rd_msg_oreq *, struct rd_msg_ores *);
extern	void	rdclose (struct rd_msg_creq *, struct rd_msg_cres *);
extern	void	rddelete(struct rd_msg_dreq *, struct rd_msg_dres *);
