TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
       src/mem.c \
       src/mutil.c \
       src/random.c \
       src/archive.c \
       src/font.c \
       src/trans.c \
       src/loadscr.c \
       src/menu.c \
       src/stage.c \
       src/psx/psx.c \
       src/psx/io.c \
       src/psx/gfx.c \
       src/psx/audio.c \
       src/psx/pad.c \
       src/psx/timer.c \
       src/psx/movie.c \
       src/stage/dummy.c \
       src/stage/week0.c \
       src/stage/week1.c \
       src/stage/week2.c \
       src/stage/week3.c \
       src/stage/week4.c \
       src/stage/week5.c \
       src/stage/week6.c \
       src/stage/week7.c \
       src/stage/week8.c \
       src/stage/week9.c \
       src/animation.c \
       src/character.c \
       src/character/bambi.c \
       src/character/bambib.c \
       src/character/bambic.c \
       src/character/bambid.c \
       src/character/dave.c \
       src/character/daveb.c \
       src/character/davec.c \
       src/character/daved.c \
       src/character/cheat.c \
       src/character/unfair.c \
       src/character/bf.c \
       src/character/bfweeb.c \
       src/character/speaker.c \
       src/character/split.c \
       src/character/gf.c \
       src/character/gf2.c \
       src/object.c \
       src/object/combo.c \
       src/object/splash.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
#LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
#LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk