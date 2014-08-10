#
# Make the Xinu configuration program
#

CC	= gcc
LEX	= flex
YACC	= bison -y	# Flag enables yacc-compatible filenames
CFLAGS	= 
LFLAGS	= -lfl

#
# Name of the configuration program
#

CONFIG	= config

all:		conf.h conf.c xboot_conf.c xboot_conf.h

${CONFIG}:	lex.yy.c y.tab.c
		$(CC) ${CFLAGS} -o $@ y.tab.c ${LFLAGS}

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
		
xboot_conf.c:	config Configuration_xboot
		./config Configuration_xboot xboot_conf.c xboot_conf.h
		
xboot_conf.h:	config Configuration_xboot
		./config Configuration_xboot xboot_conf.c xboot_conf.h
		
xboot_install:	xboot_conf.c xboot_conf.h
		cp -p xboot_conf.h ../include/conf.h
		cp -p xboot_conf.c ../system/conf.c
