#########################################################################
#									#
#									#
#  Makefile for the remote file system server (rfserver)		#
#									#
#									#
#########################################################################

# Ensure one and only one of the following are set
#DEBUGFLAGS= 	-DDEBUG
DEBUGFLAGS= 	

rfserver:	rfserver.o rsdelete.o rsmkdir.o rsopen.o		\
		rsrmdir.o rsread.o rsstat.o rstrunc.o rsclose.o		\
		rswrite.o rsofile.o snderr.o sndok.o rsodir.o rsdirread.o
		gcc -o rfserver rfserver.o rsdelete.o			\
			rsmkdir.o rsopen.o rsrmdir.o rsread.o		\
			rsstat.o rstrunc.o rswrite.o rsofile.o		\
			snderr.o sndok.o rsclose.o rsodir.o rsdirread.o

rfserver.o:	rfserver.c
		gcc $(DEBUGFLAGS) -c rfserver.c

passiveUDP.o:	passiveUDP.c
		gcc $(DEBUGFLAGS) -c passiveUDP.c

passivesock.o:	passivesock.c
		gcc $(DEBUGFLAGS) -c passivesock.c

errexit.o:	errexit.c
		gcc $(DEBUGFLAGS) -c errexit.c

clean:
		rm -f *.o .,* core rfserver

rsdelete.o:	rsdelete.c
		gcc $(DEBUGFLAGS) -c rsdelete.c

rsmkdir.o:	rsmkdir.c
		gcc $(DEBUGFLAGS) -c rsmkdir.c

rsopen.o:	rsopen.c
		gcc $(DEBUGFLAGS) -c rsopen.c

rsofile.o:	rsofile.c
		gcc $(DEBUGFLAGS) -c rsofile.c

rsrmdir.o:	rsrmdir.c
		gcc $(DEBUGFLAGS) -c rsrmdir.c

rsread.o:	rsread.c
		gcc $(DEBUGFLAGS) -c rsread.c

rsstat.o:	rsstat.c
		gcc $(DEBUGFLAGS) -c rsstat.c

rstrunc.o:	rstrunc.c
		gcc $(DEBUGFLAGS) -c rstrunc.c

rswrite.o:	rswrite.c
		gcc $(DEBUGFLAGS) -c rswrite.c

snderr.o:	snderr.c
		gcc $(DEBUGFLAGS) -c snderr.c

sndok.o:	sndok.c
		gcc $(DEBUGFLAGS) -c sndok.c

rsclose.o:	rsclose.c
		gcc $(DEBUGFLAGS) -c rsclose.c

rsodir.o:	rsodir.c
		gcc $(DEBUGFLAGS) -c rsodir.c

rsdirread.o:	rsdirread.c
		gcc $(DEBUGFLAGS) -c rsdirread.c
