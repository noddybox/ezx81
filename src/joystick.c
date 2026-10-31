/*

    espec - Sinclair Spectrum emulator

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
#include "joystick.h"
#include "gui.h"
#include "util.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* ---------------------------------------- STATICS
*/
static SDL_Joystick	*joystick;
static JoystickCallback	registered_callback;
static int		state[JOY_NumButtons];

/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void ButtonPress(JoystickButton button, int pressed)
{
    state[button] = pressed;

    if (registered_callback)
    {
    	registered_callback(button, pressed);
    }
}

/* ---------------------------------------- EXPORTED FUNCTIONS
*/
void JOYInit(void)
{
    int num;

    SDL_JoystickEventState(SDL_ENABLE);

    num = SDL_NumJoysticks();

    if (num > 0)
    {
	joystick = SDL_JoystickOpen(0);
    }
}

void JOYSelect(void)
{
    int num;
    char **list;
    int select;

    num = SDL_NumJoysticks();

    if (num < 1)
    {
    	GUIMessage(eMessageBox, "ERROR", "There are no attached joysticks");
	return;
    }

    list = Malloc(sizeof *list * num);

    for(int f = 0; f < num; f++)
    {
	const char *name;

    	list[f] = Malloc(80);

	name = SDL_JoystickNameForIndex(f);

	if (!name)
	{
	    name = "Unknown";
	}

	sprintf(list[f], "%-32.32s", name);
    }

    select = GUIListSelect("SELECT A JOYSTICK", num, list);

    if (select == -1)
    {
    	return;
    }

    if (joystick)
    {
    	SDL_JoystickClose(joystick);
	joystick = NULL;
    }

    joystick = SDL_JoystickOpen(select);

    if (!joystick)
    {
    	GUIMessage(eMessageBox, "ERROR", "Could not attach joystick");
	return;
    }
}


void JOYPump(SDL_Event *e)
{
    switch (e->type)
    {
	case SDL_JOYAXISMOTION:
	    if (e->jaxis.axis == 0)
	    {
		if (e->jaxis.value < -1000)
		{
		    ButtonPress(JOY_Left, TRUE);
		    ButtonPress(JOY_Right, FALSE);
		}
		else if (e->jaxis.value > 1000)
		{
		    ButtonPress(JOY_Left, FALSE);
		    ButtonPress(JOY_Right, TRUE);
		}
		else
		{
		    ButtonPress(JOY_Left, FALSE);
		    ButtonPress(JOY_Right, FALSE);
		}
	    }
	    else if (e->jaxis.axis == 1)
	    {
		if (e->jaxis.value < -1000)
		{
		    ButtonPress(JOY_Up, TRUE);
		    ButtonPress(JOY_Down, FALSE);
		}
		else if (e->jaxis.value > 1000)
		{
		    ButtonPress(JOY_Up, FALSE);
		    ButtonPress(JOY_Down, TRUE);
		}
		else
		{
		    ButtonPress(JOY_Up, FALSE);
		    ButtonPress(JOY_Down, FALSE);
		}
	    }
	    break;

	case SDL_JOYBUTTONDOWN:
	    if (e->jbutton.button < 5)
	    {
	    	ButtonPress(JOY_Button1 + e->jbutton.button, TRUE);
	    }
	    break;

	case SDL_JOYBUTTONUP:
	    if (e->jbutton.button < 5)
	    {
	    	ButtonPress(JOY_Button1 + e->jbutton.button, FALSE);
	    }
	    break;

	default:
	    break;
    }
}


int JOYState(JoystickButton button)
{
    return state[button];
}

void JOYRegister(JoystickCallback callback)
{
    registered_callback = callback;
}
