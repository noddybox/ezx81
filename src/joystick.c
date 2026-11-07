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

/* ---------------------------------------- STATICS
*/
static SDL_GameController	*joystick = NULL;
static JoystickCallback		registered_callback;
static int			state[JOY_NumButtons];

/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static JoystickButton MapButton(SDL_GameControllerButton button)
{
    switch(button)
    {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
	    return JOY_Up;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
	    return JOY_Down;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
	    return JOY_Left;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
	    return JOY_Right;
	case SDL_CONTROLLER_BUTTON_A:
	    return JOY_Button_A;
	case SDL_CONTROLLER_BUTTON_B:
	    return JOY_Button_B;
	case SDL_CONTROLLER_BUTTON_X:
	    return JOY_Button_X;
	case SDL_CONTROLLER_BUTTON_Y:
	    return JOY_Button_Y;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
	    return JOY_Left_Shoulder;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
	    return JOY_Right_Shoulder;
    	default:
	    return JOY_NumButtons;
    }
}

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

    SDL_GameControllerEventState(SDL_ENABLE);

    num = SDL_NumJoysticks();

    if (num > 0)
    {
	joystick = SDL_GameControllerOpen(0);
    }
}

void JOYSelect(void)
{
    const char *not_contoller = "Not a game controller";
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

	if (SDL_IsGameController(f))
	{
	    name = SDL_GameControllerNameForIndex(f);

	    if (!name)
	    {
		name = "Unknown";
	    }
	}
	else
	{
	    name = not_contoller;
	}

	sprintf(list[f], "%-32.32s", name);
    }

    select = GUIListSelect("SELECT A JOYSTICK", num, GUI_LIST_CAST(list));

    for(int f = 0; f < num; f++)
    {
    	free(list[f]);
    }

    free(list);

    if (select == -1 || !SDL_IsGameController(select))
    {
    	return;
    }

    if (joystick)
    {
    	SDL_GameControllerClose(joystick);
	joystick = NULL;
    }

    joystick = SDL_GameControllerOpen(select);

    if (!joystick)
    {
    	GUIMessage(eMessageBox, "ERROR", "Could not attach joystick");
	return;
    }
}


void JOYPump(SDL_Event *e)
{
    JoystickButton button;

    switch (e->type)
    {
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
	    button = MapButton(e->cbutton.button);

	    if (button != JOY_NumButtons)
	    {
	    	ButtonPress(button, e->cbutton.state == SDL_PRESSED);
	    }
	    break;

	case SDL_CONTROLLERDEVICEADDED:
	    if (!joystick)
	    {
		if (SDL_IsGameController(e->cdevice.which))
		{
		    joystick = SDL_GameControllerOpen(e->cdevice.which);

		    if (joystick)
		    {
			GUIToast("Attached inserted joystick");
		    }
		}
	    }
	    break;

	case SDL_CONTROLLERDEVICEREMOVED:
	    if (joystick)
	    {
		SDL_GameControllerClose(joystick);
		joystick = NULL;
		GUIToast("Joystick removed");
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

int JOYConnected(void)
{
    return joystick != NULL;
}

const char *JOYButtonName(JoystickButton button)
{
    static const char *name[] =
    {
    	"Left",
    	"Right",
    	"Up",
    	"Down",
    	"X",
    	"Y",
    	"A",
    	"B",
    	"Left Shoulder",
    	"Right Shoulder",
    };

    return name[button];
}
