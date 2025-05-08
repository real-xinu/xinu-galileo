#
# Make the Xinu configuration program
#

CC	= gcc
LEX	= flex
YACC	= bison -y	# Flag enables yacc-compatible filenames
CFLAGS	= 

#
# Name of the configuration program
#

CONFIG	= config

all:		conf.h conf.c

${CONFIG}:	lex.yy.c y.tab.c
		$(CC) -o $@ y.tab.c ${LFLAGS} -lfl

lex.yy.c:	config.l
		$(LEX) config.l

y.tab.c:	config.y
		$(YACC) config.y

clean:
		rm -f config lex.yy.c y.tab.c

conf.h:		config Configuration
		./config Configuration conf.c conf.h

conf.c:		config Configuration
		./config Configuration conf.c conf.h

install:	conf.h conf.c
		cp -p conf.h ../include
		cp -p conf.c ../system
