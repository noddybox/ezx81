# kbs - Simple, easily fooled, POP3 spam filter
# 
# Copyright (C) 2003  Ian Cowburn (ianc@noddybox.demon.co.uk)
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 
# -------------------------------------------------------------------------
# 
# $Id: Makefile,v 1.3 2003-12-19 01:14:08 ianc Exp $
# 


# This CFLAGS assumes that gcc is being used.
# Simply comment out if not, and replace as needed.
# Other CFLAGS lines *are* required.
#
CFLAGS	=	-g -Wall -Werror -pedantic -ansi

TARGET	=	ezx81

Z80LIB	=	z80/z80.a

SOURCE	=	main.c		\
		zx81.c		\
		config.c	\
		gfx.c		\
		exit.c

OBJECTS	=	main.o		\
		zx81.o		\
		config.o	\
		gfx.o		\
		exit.o

EMMA	=	emma

CFLAGS +=	-Iz80 `sdl-config --cflags`

LIBS	=	$(Z80LIB) `sdl-config --libs`


$(TARGET): $(OBJECTS) $(Z80LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

$(EMMA): $(EMMA).o $(Z80LIB)
	$(CC) $(CFLAGS) -o $(EMMA) $(EMMA).o $(Z80LIB)

$(Z80LIB): z80/*.[ch]
	cd z80; make ; cd ..

clean:
	rm -f $(TARGET) $(EMMA) $(EMMA).o $(OBJECTS) core
	cd z80; make clean; cd ..

depend:
	makedepend  -- $(CFLAGS) -- $(SOURCE) $(EMMA).c
	if test -e Makefile ; then rm -f Makefile.bak ; fi
	cd z80; make depend ; cd ..

# DO NOT DELETE THIS LINE -- make depend depends on it
