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
# $Id: Makefile,v 1.4 2003-12-19 19:31:57 ianc Exp $
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
		gui.c		\
		exit.c

OBJECTS	=	main.o		\
		zx81.o		\
		config.o	\
		gfx.o		\
		gui.o		\
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

main.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
main.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
main.o: /usr/include/stdio.h /usr/local/include/SDL/SDL.h
main.o: /usr/local/include/SDL/SDL_main.h /usr/local/include/SDL/SDL_types.h
main.o: /usr/local/include/SDL/SDL_getenv.h
main.o: /usr/local/include/SDL/SDL_error.h
main.o: /usr/local/include/SDL/begin_code.h
main.o: /usr/local/include/SDL/close_code.h
main.o: /usr/local/include/SDL/SDL_rwops.h /usr/local/include/SDL/SDL_timer.h
main.o: /usr/local/include/SDL/SDL_audio.h
main.o: /usr/local/include/SDL/SDL_byteorder.h
main.o: /usr/local/include/SDL/SDL_cdrom.h
main.o: /usr/local/include/SDL/SDL_joystick.h
main.o: /usr/local/include/SDL/SDL_events.h
main.o: /usr/local/include/SDL/SDL_active.h
main.o: /usr/local/include/SDL/SDL_keyboard.h
main.o: /usr/local/include/SDL/SDL_keysym.h
main.o: /usr/local/include/SDL/SDL_mouse.h /usr/local/include/SDL/SDL_video.h
main.o: /usr/local/include/SDL/SDL_mutex.h /usr/local/include/SDL/SDL_quit.h
main.o: /usr/local/include/SDL/SDL_version.h z80/z80.h zx81.h gfx.h gui.h
main.o: config.h exit.h
zx81.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
zx81.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
zx81.o: /usr/include/stdio.h /usr/include/string.h /usr/include/strings.h
zx81.o: zx81.h z80/z80.h config.h exit.h
config.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
config.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
config.o: /usr/include/stdio.h /usr/include/string.h /usr/include/strings.h
config.o: exit.h config.h
gfx.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
gfx.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
gfx.o: /usr/include/stdio.h /usr/include/string.h /usr/include/strings.h
gfx.o: /usr/include/stdarg.h gfx.h /usr/local/include/SDL/SDL.h
gfx.o: /usr/local/include/SDL/SDL_main.h /usr/local/include/SDL/SDL_types.h
gfx.o: /usr/local/include/SDL/SDL_getenv.h /usr/local/include/SDL/SDL_error.h
gfx.o: /usr/local/include/SDL/begin_code.h
gfx.o: /usr/local/include/SDL/close_code.h /usr/local/include/SDL/SDL_rwops.h
gfx.o: /usr/local/include/SDL/SDL_timer.h /usr/local/include/SDL/SDL_audio.h
gfx.o: /usr/local/include/SDL/SDL_byteorder.h
gfx.o: /usr/local/include/SDL/SDL_cdrom.h
gfx.o: /usr/local/include/SDL/SDL_joystick.h
gfx.o: /usr/local/include/SDL/SDL_events.h
gfx.o: /usr/local/include/SDL/SDL_active.h
gfx.o: /usr/local/include/SDL/SDL_keyboard.h
gfx.o: /usr/local/include/SDL/SDL_keysym.h /usr/local/include/SDL/SDL_mouse.h
gfx.o: /usr/local/include/SDL/SDL_video.h /usr/local/include/SDL/SDL_mutex.h
gfx.o: /usr/local/include/SDL/SDL_quit.h /usr/local/include/SDL/SDL_version.h
gfx.o: exit.h config.h font.h
gui.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
gui.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
gui.o: /usr/include/stdio.h /usr/include/string.h /usr/include/strings.h
gui.o: /usr/include/stdarg.h gui.h /usr/local/include/SDL/SDL.h
gui.o: /usr/local/include/SDL/SDL_main.h /usr/local/include/SDL/SDL_types.h
gui.o: /usr/local/include/SDL/SDL_getenv.h /usr/local/include/SDL/SDL_error.h
gui.o: /usr/local/include/SDL/begin_code.h
gui.o: /usr/local/include/SDL/close_code.h /usr/local/include/SDL/SDL_rwops.h
gui.o: /usr/local/include/SDL/SDL_timer.h /usr/local/include/SDL/SDL_audio.h
gui.o: /usr/local/include/SDL/SDL_byteorder.h
gui.o: /usr/local/include/SDL/SDL_cdrom.h
gui.o: /usr/local/include/SDL/SDL_joystick.h
gui.o: /usr/local/include/SDL/SDL_events.h
gui.o: /usr/local/include/SDL/SDL_active.h
gui.o: /usr/local/include/SDL/SDL_keyboard.h
gui.o: /usr/local/include/SDL/SDL_keysym.h /usr/local/include/SDL/SDL_mouse.h
gui.o: /usr/local/include/SDL/SDL_video.h /usr/local/include/SDL/SDL_mutex.h
gui.o: /usr/local/include/SDL/SDL_quit.h /usr/local/include/SDL/SDL_version.h
gui.o: gfx.h exit.h
exit.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
exit.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
exit.o: /usr/include/stdarg.h exit.h /usr/local/include/SDL/SDL.h
exit.o: /usr/local/include/SDL/SDL_main.h /usr/local/include/SDL/SDL_types.h
exit.o: /usr/local/include/SDL/SDL_getenv.h
exit.o: /usr/local/include/SDL/SDL_error.h
exit.o: /usr/local/include/SDL/begin_code.h
exit.o: /usr/local/include/SDL/close_code.h
exit.o: /usr/local/include/SDL/SDL_rwops.h /usr/include/stdio.h
exit.o: /usr/local/include/SDL/SDL_timer.h /usr/local/include/SDL/SDL_audio.h
exit.o: /usr/local/include/SDL/SDL_byteorder.h
exit.o: /usr/local/include/SDL/SDL_cdrom.h
exit.o: /usr/local/include/SDL/SDL_joystick.h
exit.o: /usr/local/include/SDL/SDL_events.h
exit.o: /usr/local/include/SDL/SDL_active.h
exit.o: /usr/local/include/SDL/SDL_keyboard.h
exit.o: /usr/local/include/SDL/SDL_keysym.h
exit.o: /usr/local/include/SDL/SDL_mouse.h /usr/local/include/SDL/SDL_video.h
exit.o: /usr/local/include/SDL/SDL_mutex.h /usr/local/include/SDL/SDL_quit.h
exit.o: /usr/local/include/SDL/SDL_version.h
emma.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
emma.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
emma.o: /usr/include/stdio.h z80/z80.h
