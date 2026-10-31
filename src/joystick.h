/*

    ezx81 - ZX81 emulator

    Copyright (C) 2026  Ian Cowburn (deathstation9000@gmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    -------------------------------------------------------------------------

    Joystick handling

*/
#ifndef EZX81_JOYSTICK_H
#define EZX81_JOYSTICK_H

#include <SDL.h>

/* Buttons.  Supports up to 4 buttons so that it still fits in a byte.
*/
typedef enum
{
    JOY_Right,
    JOY_Left,
    JOY_Up,
    JOY_Down,
    JOY_Button1,
    JOY_Button2,
    JOY_Button3,
    JOY_Button4,
    JOY_NumButtons
} JoystickButton;

/* Callback for joystick events
*/
typedef void (*JoystickCallback)(JoystickButton button, int pressed);


/* Initialise after SDL has been initiliased
*/
void		JOYInit(void);

/* Select a joystick
*/
void		JOYSelect(void);

/* Pump joystick events
*/
void		JOYPump(SDL_Event *e);

/* Get the state of a button
*/
int		JOYState(JoystickButton button);

/* Register a callback for joystick events
*/
void		JOYRegister(JoystickCallback callback);


#endif
