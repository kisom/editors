------------------------------------------------------------------------------
Edx mini emacs User Manual:

Copyright (C) 2002, Jean-Pierre Demailly <demailly@ujf-grenoble.fr>

Based upon the original package edx-0.56 by Terry Loveall, itself derived
from ee.c by Yijun Ding, copyright 1991.

The original work is source in the public domain and so is edx. 

------------------------------------------------------------------------------
Edx's main goal is to provide an emacs-like editor for the X Window system,
with a very small memory footprint. It uses only the libc and the basic
X routines from the X11 library. Its simple internal structure should make
it easy to adapt the code for any use.

Ems doesn't have menus, just an internal help window available through
the F1 key or the ^XH control sequence (Ctrl-X H)

All sequences Alt-? (Alt-A to Alt-Z) are free and available for private 
key bindings. They should be defined either in the system wide edxrc file 
or in the private ~/.edxrc file (check rc.example to see how this works).
The -rc option allows to load an arbitrary rcfile instead.

Command line options
====================

Usage: edx [-fn font] [-j line#] [-t tab#] [-w width] [-h height] [-b]
           [-bg color] [-fg color] [-hibg color] [-hifg color] [-cur color]
           [-rc rcfile] [-xid XID] [file]

Function key commands
=====================
F1 show help      F2  save          F3 open new file  F5 exec command
F6 Chgdir         F10 rxvt          


Special key commands
====================
Shift-Del cut     Shift-Ins paste   ^Ins copy block to X clipboard

Control key commands
====================

^A  goto bol      ^E  goto eol      ^G  goto line     ^O? switch a flag
^B  left          ^F  right         ^N  down          ^P  up
^D  del char      ^H  del prev char ^J  insert line   ^K  del to eol
^Q? literal char  ^S  find string   ^R  repeat find   ^T  transpose
^V  page down     ^W  cut block     ^Y  paste block   ^_  undo

Escape-? key commands
=====================

E B word left     E F word right    E N scroll down   E P scroll up
E < start of file E > end of file   E _ redo          E Q reformat paragr
E . set mark      E W copy block    E V page up       E % find & replace
E L lowcase block E U upcase block  E X switch marks

Ctrl-X  key commands
====================

^XC (^XQ) exit    ^XH show help     ^XS save          ^XF open new file
^XI insert file   ^XV write block   ^XW save as       ^XK del word
^XM togl rec mac  ^XP play macro    ^XT get tab size  ^XU delete line


Function keys
=============

   (key)     (description)                                   (same as)
----------------------------------------------------------------------
    F1      help                                                ^XH
    F2      file save (if modified)                             ^XS
    F3      open new file (prompts to save if file modified)    ^XF
    F5      prompt for and execute user command line
    F6      get and change directory
    F7      toggle mark block                                   Esc-.
    F8      toggle mark block                                   Esc-.
    F10     open an rxvt terminal in the current directory      Alt-Z
    Ins     toggle insert/overwrite
shf-Del     cut marked block to X clipboard
shf-Ins     paste from X clipboard
ctl-Ins     copy marked block to X clipboard
    Del     delete character under cursor or marked block       ^D
    Home    move cursor to beginning of line                    ^A
    End     move cursor to end of line                          ^E
    PgUp    move up one screen                                  Esc-V
    PgDn    move down one screen                                ^V

Navigation keys (arrows, Home, End, PgUp and PgDn) are operational.

Shift navigation marks text. 

^Home and ^End go to BOF and EOF, respectively.

Control left arrow and right arrow move by word.

Left (or right) mouse button click: set text cursor.

Left (or right) mouse button double click: select word under cursor.

Left (or right) mouse button click and drag: 
     marks and copies a text block to X clipboard.

Middle mouse button click: paste from X clipboard.

Middle mouse button drag: marks a text block.


Modes and flags:
================

Changing modes of operation is performed by ^O followed by one of the
displayed upper/lower case characters MFOCTBA. This will toggle the specific
flag. Modes are indicated as being on by displaying their upper case
character. The file modified M flag can be toggled off explicitly. The block
mark B active flag indicates a complex state. Toggle it off with the block
mark key sequences, NOT with ^OB.

M : file modified               set by anything that modifies file.
F : word wrap at text entry     toggle with ^OF
O : overwrite                   toggle with ^OO
C : search is case sensitive    toggle with ^OC
T : expand/compress tabs        toggle with ^OT
B : block mark active           toggle with ^OB
N : autoiNdent mode             toggle with ^ON
A : replace all occurences flag toggle with ^OA

The editor does display tab chars as multiple spaces. Tab (0x09) chars are
displayed as tabsize spaces. Default tab size is 4. To change tab-width to 8
the command line is  'edx -t 8'. To change from within the editor use ^KT.

To go to a specified line on initial file opening, the command line is
'edx -j 507 somefile'. Input a ^G to go to a line from within edx.

Turning on (F)ill mode enables wordwrap during text entry. Block reformat
wraps the text at the right screen edge until a double newline is encounterd.
To reformat a paragraph, place the cursor at the desired point of reformat and
enter a Esc-Q. To change the right margin use Esc-M.

As noted, undo and redo are available. Ctrl-_ for undo, Esc-_ for redo. 
A complete record of the edit session is maintained. Undoing all actions 
in the undo buffer will reset the Marked flag.

X clipboard is integrated. 

^K^C copies the marked block to the X clipboard and the right mouse button
pastes from it. 

^K^Y yanks the marked block from the text buffer to the block buffer.

^K^V copies the deleted block from the block buffer to the point of the text
cursor. 

Cut, copy and paste wrt the X clipboard by Shift-Del, Control-Insert and 
Shift-Insert, respectively.

Find and 'Search and Replace' will pick up any marked blocks, text under
the cursor or user input in that order. Found text is highlighted. Set replace
ALL flag wth ^OA option before running SAR to replace all occurences. 

For general dialog entry, if the first character entered is not ^H, ^C, End,
Esc or Enter, the dialog string is discarded. End moves the cursor to the end
of the dialog string (thereby recovering Bacspace'd over chars). There is
no insert char mode for dialogs.

For a complete understanding of the operation of edx, study the code. It is the
final authority on operation.

Remember, when all else fails READ THE SCREEN.
------------------------------------------------------------------------------
