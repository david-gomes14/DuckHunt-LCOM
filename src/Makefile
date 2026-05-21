# name of project
PROG = proj

.PATH: ${.CURDIR}/game
.PATH: ${.CURDIR}/rtc
.PATH: ${.CURDIR}/timer
.PATH: ${.CURDIR}/keyboard
.PATH: ${.CURDIR}/mouse
.PATH: ${.CURDIR}/graphic
.PATH: ${.CURDIR}/utils

# source code files to be compiled
SRCS = main.c game.c target.c graphic.c timer.c kbc.c mouse.c utils.c

# additional compilation flags
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile
.include <minix.lcom.mk>

