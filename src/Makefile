# name of project
PROG = proj

.PATH: ${.CURDIR}/game
.PATH: ${.CURDIR}/rtc
.PATH: ${.CURDIR}/timer
.PATH: ${.CURDIR}/keyboard
.PATH: ${.CURDIR}/mouse
.PATH: ${.CURDIR}/graphic

# source code files to be compiled
SRCS = main.c game.c

# additional compilation flags
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile
.include <minix.lcom.mk>

