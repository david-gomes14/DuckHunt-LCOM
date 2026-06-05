# name of project
PROG = proj

.PATH: ${.CURDIR}/game
.PATH: ${.CURDIR}/game/input
.PATH: ${.CURDIR}/game/menu
.PATH: ${.CURDIR}/game/state
.PATH: ${.CURDIR}/game/target
.PATH: ${.CURDIR}/game/score
.PATH: ${.CURDIR}/game/hud

.PATH: ${.CURDIR}/rtc
.PATH: ${.CURDIR}/timer
.PATH: ${.CURDIR}/keyboard
.PATH: ${.CURDIR}/mouse
.PATH: ${.CURDIR}/graphic
.PATH: ${.CURDIR}/utils
.PATH: ${.CURDIR}/serial_port

# source code files to be compiled
SRCS = main.c game.c menu.c input.c state.c target.c score.c hud.c graphic.c timer.c kbc.c mouse.c utils.c serial_port.c

# additional compilation flags
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile
.include <minix.lcom.mk>

