/*

    ezx81 - X11 ZX81 emulator

    Copyright (C) 2003  Ian Cowburn (ianc@noddybox.demon.co.uk)

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

    Config file

*/

#ifndef EZX81_CONFIG_H
#define EZX81_CONFIG_H


/* Integer settings
*/
typedef enum
{
    CONF_FULLSCREEN,
    CONF_MEMSIZE,
    CONF_FRAMES_PER_SEC,
    CONF_SCALE,
    CONF_TRACE
} IConfigVar;


/* String settings
*/
typedef enum
{
    CONF_ROMFILE,
    CONF_TAPEDIR
} SConfigVar;


/* Read config file
*/
void		ConfigRead(void);


/* Get integer setting
*/
int		IConfig(IConfigVar v);


/* Get string setting
*/
const char	*SConfig(SConfigVar v);


#endif


/* END OF FILE */
