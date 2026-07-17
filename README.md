# ezx81

ezx81 is a basic emulation of the ZX81.  It was mainly done as a test bed for
my Z80 emulation.

## Usage

To run it simply run the generated `ezx81` executable.  It reads configuration
from `$HOME/.ezx81`, an example of which can be found in `example_config`.

In use the keyboard is as the ZX81, with the shift skeys acting as the ZX81
shift key.  The backspace and delete key is mapped to the ZX81 delete and the
cursor keys are mapped to the appropriate shifted number keys.

To quit the emulator press the ESCAPE key.

To get a quick help press F1.

As described in the help, the function keys do the following:

F1 - Display the help message

F2 - Display a message about the emulator.

F3 - Display the ZX81 keyboard for reference.

F11 - Display the Memory Menu.

F12 - Display an overlay of the current state of the Z80 CPU while the system
is running.

## .P Files

The emulator only supports the use of .P files, which are simple binary dumps
of memory.  It loads FILE.P from the snapshot directory defined in the config
file if you issue the command `LOAD "FILE"`.  Saving is not implemented yet.

## Memory Menu

The memory menu allows you to inspect the state of the running system, mainly
for debugging purposes.  At all points indicated you can press F1 to get an
overview of the available keys.

As a general note, when an address is prompted for you can enter expressions,
register names and can use different bases.  For instance `pc` will enter the
address as the current contents of the Program Counter.  By default numbers
are decimal.  To use hex number prefix the value with `0x`.

### Main Menu

The main memory menu offers these options:

#### 1 - Disassemble/Hex dump

Displays a disassembly or hex dump of memory, press F1 to get an overview of
the keys.

#### 2 - Disassemble to file

Saves a disassembly to a file.  It will prompt for a start address, and number
of bytes to disassemble and the name of an output file.  If no path is
specified the file will be output to the current directory.

#### 3 - Start/Stop trace log

This option will start and stop the tracing of CPU state to a trace file,
called `trace` which it will output to the current directory.

#### 4 - Playback trace log

This will allow you to play through the contents of a saved `trace` file.
Press F1 for details of the available key commands.  Note that the disassembly
will be memory as it is NOW rather than at the time of the trace.

#### 5 - Add a new breakpoint

Allows you to enter an address or an expression (e.g. `hl==0x1234`) which when
the PC hits that address or the expression is true the emulation will be
paused and the memory menu entered.

#### 6 - Set active breakpoints

Allows you to set which breakpoints are active.  Follow the on-screen prompts.

#### 7 - Remove a breakpoint

Allows you to remove a selected breakpoint.  Follow the on-screen prompts.

#### 8 - Clear all breakpoints

Prompts as to whether to clear all breakpoints.

#### 9 - Monitor

Allows you to monitor the running system, single stepping, running (and
optionally watching) the system, and doing so till a breakpoint is hit.  Press
F1 for details of the available key commands.

#### R - Reset

Reset the running system.

#### X - Exit (without confirm)

Exits the emulator without prompting you first.

#### ESCAPE - Return

Return to the running system.
