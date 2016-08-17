#########################################################################
#									#
#									#
#  Makefile for the remote disk server (rdserver)			#
#									#
#									#
#########################################################################

# Ensure one and only one of the following are set
#DEBUGFLAGS= 	-DDEBUG
DEBUGFLAGS=

rdserver:	rdserver.o rdopen.o rdclose.o rdread.o rdwrite.o	\
		rddelete.o rdofile.o snderr.o sndok.o
		gcc -o rdserver rdserver.o rdopen.o			\
			rdclose.o rdread.o rdwrite.o rddelete.o		\
			rdofile.o snderr.o sndok.o

rdserver.o:	rdserver.c
		gcc $(DEBUGFLAGS) -c rdserver.c

clean:
		rm -f *.o .,* core rdserver

rdopen.o:	rdopen.c
		gcc $(DEBUGFLAGS) -c rdopen.c

rdclose.o:	rdclose.c
		gcc $(DEBUGFLAGS) -c rdclose.c

rdread.o:	rdread.c
		gcc $(DEBUGFLAGS) -c rdread.c

rdwrite.o:	rdwrite.c
		gcc $(DEBUGFLAGS) -c rdwrite.c

rddelete.o:	rddelete.c
		gcc $(DEBUGFLAGS) -c rddelete.c

rdofile.o:	rdofile.c
		gcc $(DEBUGFLAGS) -c rdofile.c

snderr.o:	snderr.c
		gcc $(DEBUGFLAGS) -c snderr.c

sndok.o:	sndok.c
		gcc $(DEBUGFLAGS) -c sndok.c
