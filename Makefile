# The name to install the editor under.
EXEC=pe

CFLAGS=-Wall -O2
# To be /really/ fanatical on the executible's size...
#CFLAGS=-Os -fomit-frame-pointer -malign-loops=0 -malign-jumps=0 -malign-functions=0 -Wall

# Uncomment to force the screen size, in situtations were it is desirable or
# if your system lacks the TIOCGWINSZ ioctl.
#DUMBCONS= -DFORCE_COLS=80 -DFORCE_ROWS=24

# NetBSD, Linux, and nearly all other POSIX-based enviroments
LFLAGS= -ltermcap
# Try this on some older systems
#LFLAGS= -lcurses
# For very early Linux
#LFLAGS= -lbsd

#PREFIX= /usr/local
PREFIX= ${HOME}
BINDIR=	${PREFIX}/bin

HFILES=	estruct.h edef.h efunc.h ebind.h

CFILES=	basic.c buffer.c display.c file.c fileio.c line.c main.c \
	random.c region.c strlcpy.c search.c tcap.c termio.c window.c

OFILES=	basic.o buffer.o display.o file.o fileio.o line.o main.o \
	random.o region.o strlcpy.c search.o tcap.o termio.o window.o

all:	$(OFILES)
	$(CC) $(CFLAGS) $(OFILES) -o $(EXEC) $(LFLAGS)

#install: all
#	install -c -m 0755 -o bin -g bin -s $(EXEC) $(BINDIR)

install: all
	install -c -m 0755 -s $(EXEC) $(BINDIR)

clean:
	rm -f *.o *~ *.core *.bak $(EXEC)

basic.o: basic.c estruct.h edef.h
buffer.o: buffer.c estruct.h edef.h
display.o: display.c estruct.h edef.h
file.o: file.c estruct.h edef.h
fileio.o: fileio.c estruct.h
line.o: line.c estruct.h edef.h
main.o: main.c estruct.h edef.h efunc.h ebind.h
random.o: random.c estruct.h edef.h
region.o: region.c estruct.h edef.h
strlcpy.o: strlcpy.c
search.o: search.c estruct.h edef.h
tcap.o: tcap.c estruct.h edef.h
	$(CC) $(CFLAGS) $(DUMBCONS) -c $<
termio.o: termio.c
window.o: window.c estruct.h edef.h
