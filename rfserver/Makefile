rfserver:	rfserver.o rsdelete.o rsmkdir.o rsopen.o		\
		rsrmdir.o rsread.o rsstat.o rstrunc.o rsclose.o		\
		rswrite.o rsofile.o snderr.o sndok.o rsodir.o rsdirread.o
		gcc -o rfserver rfserver.o rsdelete.o			\
			rsmkdir.o rsopen.o rsrmdir.o rsread.o		\
			rsstat.o rstrunc.o rswrite.o rsofile.o		\
			snderr.o sndok.o rsclose.o rsodir.o rsdirread.o
		mv rfserver ../programs

rfserver.o:	rfserver.c
		gcc -c rfserver.c

passiveUDP.o:	passiveUDP.c
		gcc -c passiveUDP.c

passivesock.o:	passivesock.c
		gcc -c passivesock.c

errexit.o:	errexit.c
		gcc -c errexit.c

clean:
		rm -f *.o .,* core
		rm -f ../programs/rfserver

rsdelete.o:	rsdelete.c
		gcc -c rsdelete.c

rsmkdir.o:	rsmkdir.c
		gcc -c rsmkdir.c

rsopen.o:	rsopen.c
		gcc -c rsopen.c

rsofile.o:	rsofile.c
		gcc -c rsofile.c

rsrmdir.o:	rsrmdir.c
		gcc -c rsrmdir.c

rsread.o:	rsread.c
		gcc -c rsread.c

rsstat.o:	rsstat.c
		gcc -c rsstat.c

rstrunc.o:	rstrunc.c
		gcc -c rstrunc.c

rswrite.o:	rswrite.c
		gcc -c rswrite.c

snderr.o:	snderr.c
		gcc -c snderr.c

sndok.o:	sndok.c
		gcc -c sndok.c

rsclose.o:	rsclose.c
		gcc -c rsclose.c

rsodir.o:	rsodir.c
		gcc -c rsodir.c

rsdirread.o:	rsdirread.c
		gcc -c rsdirread.c
