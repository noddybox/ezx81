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
# $Id: Makefile,v 1.11 2006-09-16 22:54:59 ianc Exp $
# 


# This CFLAGS assumes that gcc is being used.
# Simply comment out if not, and replace as needed.
# Other CFLAGS lines *are* required.
#
CFLAGS	=	-g -Wall -Werror -pedantic -ansi

TARGET	=	ezx81

SOURCE	=	main.c		\
		zx81.c		\
		config.c	\
		gfx.c		\
		gui.c		\
		memmenu.c	\
		util.c		\
		exit.c		\
		expr.c		\
		z80.c		\
		z80_decode.c	\
		z80_dis.c

OBJECTS	=	main.o		\
		zx81.o		\
		config.o	\
		gfx.o		\
		gui.o		\
		memmenu.o	\
		util.o		\
		exit.o		\
		expr.o		\
		z80.o		\
		z80_decode.o	\
		z80_dis.o

CFLAGS +=	`sdl-config --cflags`

LIBS	=	`sdl-config --libs`


$(TARGET): $(OBJECTS) $(Z80LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

$(Z80LIB): z80/*.[ch]
	cd z80; make "EXTERNAL_CFLAGS=`sdl-config --cflags`" ; cd ..

clean:
	rm -f $(TARGET) $(TARGET).exe $(OBJECTS) core

depend:
	makedepend  -- $(CFLAGS) -- $(SOURCE)
	if test -e Makefile ; then rm -f Makefile.bak ; fi

# DO NOT DELETE THIS LINE -- make depend depends on it

main.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
main.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
main.o: /usr/include/machine/_types.h /usr/include/stdio.h
main.o: /usr/include/string.h /usr/include/strings.h
main.o: /usr/local/include/SDL11/SDL.h /usr/local/include/SDL11/SDL_main.h
main.o: /usr/local/include/SDL11/SDL_types.h
main.o: /usr/local/include/SDL11/SDL_getenv.h
main.o: /usr/local/include/SDL11/SDL_error.h
main.o: /usr/local/include/SDL11/begin_code.h
main.o: /usr/local/include/SDL11/close_code.h
main.o: /usr/local/include/SDL11/SDL_rwops.h
main.o: /usr/local/include/SDL11/SDL_timer.h
main.o: /usr/local/include/SDL11/SDL_audio.h
main.o: /usr/local/include/SDL11/SDL_byteorder.h
main.o: /usr/local/include/SDL11/SDL_cdrom.h
main.o: /usr/local/include/SDL11/SDL_joystick.h
main.o: /usr/local/include/SDL11/SDL_events.h
main.o: /usr/local/include/SDL11/SDL_active.h
main.o: /usr/local/include/SDL11/SDL_keyboard.h
main.o: /usr/local/include/SDL11/SDL_keysym.h
main.o: /usr/local/include/SDL11/SDL_mouse.h
main.o: /usr/local/include/SDL11/SDL_video.h
main.o: /usr/local/include/SDL11/SDL_mutex.h
main.o: /usr/local/include/SDL11/SDL_quit.h
main.o: /usr/local/include/SDL11/SDL_version.h z80.h z80_config.h zx81.h
main.o: gfx.h gui.h memmenu.h config.h exit.h
zx81.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
zx81.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
zx81.o: /usr/include/machine/_types.h /usr/include/stdio.h
zx81.o: /usr/include/string.h /usr/include/strings.h zx81.h z80.h
zx81.o: z80_config.h /usr/local/include/SDL11/SDL.h
zx81.o: /usr/local/include/SDL11/SDL_main.h
zx81.o: /usr/local/include/SDL11/SDL_types.h
zx81.o: /usr/local/include/SDL11/SDL_getenv.h
zx81.o: /usr/local/include/SDL11/SDL_error.h
zx81.o: /usr/local/include/SDL11/begin_code.h
zx81.o: /usr/local/include/SDL11/close_code.h
zx81.o: /usr/local/include/SDL11/SDL_rwops.h
zx81.o: /usr/local/include/SDL11/SDL_timer.h
zx81.o: /usr/local/include/SDL11/SDL_audio.h
zx81.o: /usr/local/include/SDL11/SDL_byteorder.h
zx81.o: /usr/local/include/SDL11/SDL_cdrom.h
zx81.o: /usr/local/include/SDL11/SDL_joystick.h
zx81.o: /usr/local/include/SDL11/SDL_events.h
zx81.o: /usr/local/include/SDL11/SDL_active.h
zx81.o: /usr/local/include/SDL11/SDL_keyboard.h
zx81.o: /usr/local/include/SDL11/SDL_keysym.h
zx81.o: /usr/local/include/SDL11/SDL_mouse.h
zx81.o: /usr/local/include/SDL11/SDL_video.h
zx81.o: /usr/local/include/SDL11/SDL_mutex.h
zx81.o: /usr/local/include/SDL11/SDL_quit.h
zx81.o: /usr/local/include/SDL11/SDL_version.h gfx.h gui.h config.h util.h
zx81.o: exit.h
config.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
config.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
config.o: /usr/include/machine/_types.h /usr/include/stdio.h
config.o: /usr/include/string.h /usr/include/strings.h exit.h config.h
gfx.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
gfx.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
gfx.o: /usr/include/machine/_types.h /usr/include/stdio.h
gfx.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdarg.h
gfx.o: gfx.h /usr/local/include/SDL11/SDL.h
gfx.o: /usr/local/include/SDL11/SDL_main.h
gfx.o: /usr/local/include/SDL11/SDL_types.h
gfx.o: /usr/local/include/SDL11/SDL_getenv.h
gfx.o: /usr/local/include/SDL11/SDL_error.h
gfx.o: /usr/local/include/SDL11/begin_code.h
gfx.o: /usr/local/include/SDL11/close_code.h
gfx.o: /usr/local/include/SDL11/SDL_rwops.h
gfx.o: /usr/local/include/SDL11/SDL_timer.h
gfx.o: /usr/local/include/SDL11/SDL_audio.h
gfx.o: /usr/local/include/SDL11/SDL_byteorder.h
gfx.o: /usr/local/include/SDL11/SDL_cdrom.h
gfx.o: /usr/local/include/SDL11/SDL_joystick.h
gfx.o: /usr/local/include/SDL11/SDL_events.h
gfx.o: /usr/local/include/SDL11/SDL_active.h
gfx.o: /usr/local/include/SDL11/SDL_keyboard.h
gfx.o: /usr/local/include/SDL11/SDL_keysym.h
gfx.o: /usr/local/include/SDL11/SDL_mouse.h
gfx.o: /usr/local/include/SDL11/SDL_video.h
gfx.o: /usr/local/include/SDL11/SDL_mutex.h
gfx.o: /usr/local/include/SDL11/SDL_quit.h
gfx.o: /usr/local/include/SDL11/SDL_version.h exit.h config.h font.h
gui.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
gui.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
gui.o: /usr/include/machine/_types.h /usr/include/stdio.h
gui.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdarg.h
gui.o: /usr/include/ctype.h /usr/include/_ctype.h /usr/include/runetype.h
gui.o: gui.h /usr/local/include/SDL11/SDL.h
gui.o: /usr/local/include/SDL11/SDL_main.h
gui.o: /usr/local/include/SDL11/SDL_types.h
gui.o: /usr/local/include/SDL11/SDL_getenv.h
gui.o: /usr/local/include/SDL11/SDL_error.h
gui.o: /usr/local/include/SDL11/begin_code.h
gui.o: /usr/local/include/SDL11/close_code.h
gui.o: /usr/local/include/SDL11/SDL_rwops.h
gui.o: /usr/local/include/SDL11/SDL_timer.h
gui.o: /usr/local/include/SDL11/SDL_audio.h
gui.o: /usr/local/include/SDL11/SDL_byteorder.h
gui.o: /usr/local/include/SDL11/SDL_cdrom.h
gui.o: /usr/local/include/SDL11/SDL_joystick.h
gui.o: /usr/local/include/SDL11/SDL_events.h
gui.o: /usr/local/include/SDL11/SDL_active.h
gui.o: /usr/local/include/SDL11/SDL_keyboard.h
gui.o: /usr/local/include/SDL11/SDL_keysym.h
gui.o: /usr/local/include/SDL11/SDL_mouse.h
gui.o: /usr/local/include/SDL11/SDL_video.h
gui.o: /usr/local/include/SDL11/SDL_mutex.h
gui.o: /usr/local/include/SDL11/SDL_quit.h
gui.o: /usr/local/include/SDL11/SDL_version.h gfx.h util.h exit.h
memmenu.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
memmenu.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
memmenu.o: /usr/include/machine/_types.h /usr/include/stdio.h
memmenu.o: /usr/include/string.h /usr/include/strings.h /usr/include/ctype.h
memmenu.o: /usr/include/_ctype.h /usr/include/runetype.h memmenu.h z80.h
memmenu.o: z80_config.h zx81.h /usr/local/include/SDL11/SDL.h
memmenu.o: /usr/local/include/SDL11/SDL_main.h
memmenu.o: /usr/local/include/SDL11/SDL_types.h
memmenu.o: /usr/local/include/SDL11/SDL_getenv.h
memmenu.o: /usr/local/include/SDL11/SDL_error.h
memmenu.o: /usr/local/include/SDL11/begin_code.h
memmenu.o: /usr/local/include/SDL11/close_code.h
memmenu.o: /usr/local/include/SDL11/SDL_rwops.h
memmenu.o: /usr/local/include/SDL11/SDL_timer.h
memmenu.o: /usr/local/include/SDL11/SDL_audio.h
memmenu.o: /usr/local/include/SDL11/SDL_byteorder.h
memmenu.o: /usr/local/include/SDL11/SDL_cdrom.h
memmenu.o: /usr/local/include/SDL11/SDL_joystick.h
memmenu.o: /usr/local/include/SDL11/SDL_events.h
memmenu.o: /usr/local/include/SDL11/SDL_active.h
memmenu.o: /usr/local/include/SDL11/SDL_keyboard.h
memmenu.o: /usr/local/include/SDL11/SDL_keysym.h
memmenu.o: /usr/local/include/SDL11/SDL_mouse.h
memmenu.o: /usr/local/include/SDL11/SDL_video.h
memmenu.o: /usr/local/include/SDL11/SDL_mutex.h
memmenu.o: /usr/local/include/SDL11/SDL_quit.h
memmenu.o: /usr/local/include/SDL11/SDL_version.h gfx.h gui.h util.h
util.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
util.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
util.o: /usr/include/machine/_types.h /usr/include/string.h
util.o: /usr/include/strings.h /usr/include/stdarg.h util.h
util.o: /usr/include/stdlib.h exit.h
exit.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
exit.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
exit.o: /usr/include/machine/_types.h /usr/include/stdarg.h exit.h
exit.o: /usr/local/include/SDL11/SDL.h /usr/local/include/SDL11/SDL_main.h
exit.o: /usr/local/include/SDL11/SDL_types.h
exit.o: /usr/local/include/SDL11/SDL_getenv.h
exit.o: /usr/local/include/SDL11/SDL_error.h
exit.o: /usr/local/include/SDL11/begin_code.h
exit.o: /usr/local/include/SDL11/close_code.h
exit.o: /usr/local/include/SDL11/SDL_rwops.h /usr/include/stdio.h
exit.o: /usr/local/include/SDL11/SDL_timer.h
exit.o: /usr/local/include/SDL11/SDL_audio.h
exit.o: /usr/local/include/SDL11/SDL_byteorder.h
exit.o: /usr/local/include/SDL11/SDL_cdrom.h
exit.o: /usr/local/include/SDL11/SDL_joystick.h
exit.o: /usr/local/include/SDL11/SDL_events.h
exit.o: /usr/local/include/SDL11/SDL_active.h
exit.o: /usr/local/include/SDL11/SDL_keyboard.h
exit.o: /usr/local/include/SDL11/SDL_keysym.h
exit.o: /usr/local/include/SDL11/SDL_mouse.h
exit.o: /usr/local/include/SDL11/SDL_video.h
exit.o: /usr/local/include/SDL11/SDL_mutex.h
exit.o: /usr/local/include/SDL11/SDL_quit.h
exit.o: /usr/local/include/SDL11/SDL_version.h
expr.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
expr.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
expr.o: /usr/include/machine/_types.h /usr/include/stdio.h
expr.o: /usr/include/string.h /usr/include/strings.h /usr/include/ctype.h
expr.o: /usr/include/_ctype.h /usr/include/runetype.h
z80.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
z80.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
z80.o: /usr/include/machine/_types.h /usr/include/string.h
z80.o: /usr/include/strings.h z80.h z80_config.h z80_private.h
z80_decode.o: /usr/include/stdlib.h /usr/include/sys/cdefs.h
z80_decode.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
z80_decode.o: /usr/include/machine/_types.h /usr/include/limits.h
z80_decode.o: /usr/include/sys/limits.h /usr/include/machine/_limits.h
z80_decode.o: /usr/include/sys/syslimits.h z80.h z80_config.h z80_private.h
z80_dis.o: z80_config.h /usr/include/stdio.h /usr/include/sys/cdefs.h
z80_dis.o: /usr/include/sys/_null.h /usr/include/sys/_types.h
z80_dis.o: /usr/include/machine/_types.h /usr/include/string.h
z80_dis.o: /usr/include/strings.h /usr/include/stdarg.h z80.h z80_private.h
