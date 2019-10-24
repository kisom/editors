
/* mye.c, from ee.c, Yijun Ding, Copyright 1991
modified Dec. 15 2002. Copyright (C) 1998,2002 Terry Loveall, <loveall@qwest.net>

Code starts after the Users Manual.
*/

#define VERSION "1.03"
//#define DEBUG
/*
Compile under Linux with termcap lib support
        gcc mye.c -O2 -ltermcap -o mye ; strip mye

Compile under Linux for gdb with termcap lib support
        gcc mye.c. -O2 -g -ltermcap -o mye

Compile with MSVC:      cl -DMSVC -DSTATICBUF -Ox mye.c

------------------------------------------------------------------------------
Users Manual:

Based upon the original work ee.c of Yijun Ding, copyright 1991. His logic
simplicity was elegant. Any problems are mine.

The original work is public domain with source and so is mye.c. I would
appreciate hearing about any bug fixes or improvements.

This is a simple, basic text editor. It requires only three graphic support
routines from the OS: goto X-Y, set character attribute and clear screen. For
input it uses the old WordStar style control key sequences; i.e. ^Kh or ^K^H 
are all equivalant. It will compile on NT with MSVC and lcc and on Linux with 
gcc.

On Linux, it uses termios rather than curses, so will run with minimum library
support (like on a rescue/boot disk). Function keys under Linux probably won't
work depending upon execution context. They might work on the text console and
almost certainly won't on any kind of xterm.

For input, mye uses a version of the old WordStar style control key sequences;
i.e. ^k^h or ^kh, always lower case control chars. ^kc copies marked block to
clipboard, right mouse pastes from clipboard. ^ky deletes marked block to
block buffer, ^kv copies from from block buffer to cursor position.

Basic navigation is on the left of the keyboard:

  Q W E R      ^E    ^W  ^R
   A S D F   ^S  ^D             ^A   ^F
    Z X C      ^X    ^Z  ^C

    ^E - up
    ^X - down
    ^D - right
    ^S - left

    ^W - scroll down 1 line
    ^Z - scroll up 1 line

    ^R - page up
    ^C - page down

    ^F - word right
    ^A - word left

    ^Q^D - goto end of line
    ^Q^S - goto start of line

    ^Q^R - goto start of file
    ^Q^C - goto end of file

    ^Q^F - find
    ^Q^A - find and replace

Once you learn the 'magic diamond' of EXDS with ^Q extensions, function keys
and mouse become irrelevant. Following are the rest of the key commands:

    ^B - word wrap text until the next double newline
    ^G - delete character under cursor
    ^H - delete character left of cursor
    ^I - insert tab char
    ^J - newline
    ^K - prefix for file/block operations
    ^L - repeat last find
    ^M - insert newline
    ^N - insert newline
    ^O - prefix for display and change mode flags
    ^P - inline literal
    ^T - delete from cursor to start of next string
    ^U - undo
    ^V - toggle insert/overwrite mode
    ^Y - delete cursor line

    ^QI - goto line
    ^QL - refresh screen
    ^QM - get new right margin
    ^QY - delete from cursor to end of line

    ^KB - toggle mark block
    ^KC - copy marked block to selection
    ^KD - exit if file not modified
    ^KF - open new file
    ^KH - help
    ^KK - toggle mark block
    ^KM - record macro
    ^KN - get and change directory
    ^KP - play macro
    ^KQ - exit if file not modified
    ^KR - read a file and paste into cursor position
    ^KS - save and continue
    ^KT - get new tab size
    ^KU - redo
    ^KV - paste block buffer into cursor position
    ^KW - write marked block to file
    ^KX - exit if file not modified
    ^KY - cut marked block to block buffer

Under NT the following function keys are operational:

   (key)     (description)                                   (same as)
----------------------------------------------------------------------
    F2      file save (if modified)                             ^KS
    F3      open new file (prompts to save if file modified)    ^KF
    F7      toggle mark block                                   ^KB
    F8      toggle mark block                                   ^KK
    Insert  toggle insert/overwrite                             ^V
    Del     delete character under cursor or marked block       ^G
    Home    move cursor to beginning of line                    ^QS
    End     move cursor to end of line                          ^QD
    PgUp    move up one screen                                  ^R
    PgDn    move down one screen                                ^C

Modes:

Changing modes of operation is performed by ^o followed by one of the
displayed upper/lower case characters MFOCTBNRA. This will toggle the specific
flag. Modes are indicated as being on by displaying their upper case
character. The file modified M flag can be toggled off explicitly. The block
mark B active flag indicates a complex state. Toggle it off with the block
mark key sequences, NOT with ^oB.

M : file modified               set by anything that modifies file.
F : word wrap at text entry     toggle with ^oF
O : overwrite                   toggle with ^oO, ^V or function key Insert
C : search is case sensitive    toggle with ^oC
T : expand/compress tabs        toggle with ^oT
B : block mark active           toggle ^KB, ^KK, F7 or F8 (don't with ^oB)
N : autoiNdent active           toggle ^on
R : macro record                toggle ^KM (don't with ^oB)
A : replace all occurences flag toggle with ^oA

The editor does display tab chars as multiple spaces. Tab (0x09) chars are
displayed as tabsize spaces. Default tab size is 4. To change tab-width to 8
the command line is  'mye -t8'. To change from within the editor use ^KT.

To go to a specified line on initial file opening, the command line is
'mye -+507 somefile'. The minus plus sequence is required. Input a ^J to go to
a line from within e.

Turning on (F)ill mode enables wordwrap during text entry. Block reformat
wraps the text at the right screen edge until a double newline is encounterd.
To reformat a paragraph, place the cursor at the desired point of reformat and
enter a ^b. To change the right margin use ^Q^M.

As noted, undo and redo are available. U for undo, ^KU for redo. A complete
record of the edit session is maintained. Undoing all actions in the undo
buffer will reset the Marked flag.

Find and 'Search and Replace' will pick up any marked blocks, text under
the cursor or user input in that order. Found text is highlighted. Set replace
ALL flag wth ^oA option before running SAR to replace all occurences. 

For general dialog entry if the first character entered is not ^H, ^C, ^L, or 
Enter, the dialog string is discarded. ^L moves the cursor to the end of the 
dialog string.

For a complete understanding of the operation of mye, study the code. It is the
ultimate authority on operation.

Remember, when all else fails READ THE SCREEN.
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

/* Target dependent include files ********************************* */

#ifdef MSVC

#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <io.h>
#include <direct.h>

#else /* not MSVCd */

#include <termios.h>    /* use termios.h for termcap output */
#include <unistd.h>

#endif /* MSVC */

#define MAXVLINE 450
#define EOL     '\0'    /* end of line marker */
#define BLNK    ' '     /* blank */
#define LF      '\n'    /* new line */
#define NLEN    256     /* input buffer length */
#define LMAX    10000   /* max line length */
#define XINC    20      /* increament for x offset */
#define HLP     28      /* denotes Help to main_exec */

#define CHG 0   /* file Modified: =0 file not changed, !=0 file changed */
#define FIL 1   /* Fill */
#define OVR 2   /* character insert=0, Overwrite=1 */
#define CAS 3   /* Case sensitive: =0 no, !=0 yes */
#define TAB 4   /* Tab expand */
#define BLK 5   /* block mark active */

#define IND 6	/* auto indent flag */
#define REC 7	/* recording macro flag */
#define ALL 8	/* replace all flag */
#define WRD 9	/* find wdelims delimited word */
#define SHW 10  /* update & show entire screen */
#define NEW 11	/* editing a new file */
#define WIN 12	/* window: <0 same win, =0 load new */

#define AMAX	0xD0000 	/* main buffer size */
#define BMAX	0x10000 	/* block size */

/* enable/disable TERMINAL options */
#define TERMINAL

/* enable/disable NOBAK file option */
#define NOBAK

int funckey;		/* function key flag */
int phys_height;	/* screen physical height */
int exitf = 1;		/* exitf = 0 = exit */
char *cfdpath;		/* current file path */

/* character handler types */
static enum {
		MAIN,
		DIALOG,
		OPTIONS
} executive = MAIN; 			/* default character handler */

/* Help screen -- rewrite to suit your own tastes */

static char HELP_STR[] = "    mye Ver. "VERSION"\0\
Usage: mye [-+n(line#] [-tn(default tab n=4)] [file]\0\0\
^O[mfoctbnaw] (Mod, Fill, Ovrwrit, Case, Tab, Blk mk, Ndent, repl All, Word)\0\
*NOTE: M flag must be reset to exit or load new file.\0\
\0\
Shift-Del cut     ^Ins copy          Shift-Ins paste\0\
^QS goto bol      ^KQ ^KX exit       ^KH F1 show help\0\
^QD goto eol      ^KB mark block on  ^KR read file\0\
^QR start of file ^KK mark block off ^KF open new file\0\
^QC end of file   ^KY cut block      ^KS save & resume\0\
^QF find          ^KC copy block     ^KD reset M flag\0\
^QA find & repl   ^KV paste block\0\
^QY del to eol    ^KW write block\0\
^QM get margin    ^KT get tab size   ^M ^N insert newline\0\
^QU upper case blk ^QL lower case blk\0\
^A word left     ^R page up   ^Z scroll up      ^I insert tab\0\
^F word right    ^C page down ^W scroll dn      ^J goto line\0\
^T del word      ^D right     ^V toggle insert  ^L repeat find\0\
^G del char      ^S left      ^QP goto prev pos ^B reformat para\0\
^H del prev char ^E up        ^KP play macro    ^U undo\0\
^Y delete line   ^X down      ^KM togl rec mac  ^KU redo\0\
                   Press any key to continue ...";

/* code */

/* NT I/O specific code ********************************* */

#ifdef MSVC

/* MSVC specific variables */
COORD outxy ;                   /* output COORD for SetConsoleCursorPosition */
CONSOLE_SCREEN_BUFFER_INFO csbi;/* to get MS screen buffer info */
HANDLE stdIn,stdOut,stdErr;     /* handles to output window/buffer */
BOOL bSuccess;                  /* OS result flag */

/* MSVC specific protos */
void clreol();
void clrscr();
void gotoxy(int horz,int vert);

/* MSVC specific macros */

#define cprintf(x,y)    _cprintf(x,y)
#define cputs(s)        _cputs (s)

#define ttopen()     stdIn=GetStdHandle(STD_INPUT_HANDLE);\
    stdOut=GetStdHandle(STD_OUTPUT_HANDLE);\
    stdErr=GetStdHandle(STD_ERROR_HANDLE);\
    bSuccess = GetConsoleScreenBufferInfo(stdOut, &csbi);\
    if (!(bSuccess)) return(0);\
    screen_height=((csbi.srWindow.Bottom)-(csbi.srWindow.Top))-2;\
    screen_width=((csbi.srWindow.Right)-(csbi.srWindow.Left))+1

#define highvideo()   SetConsoleTextAttribute(stdOut,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN)
#define lowvideo()    SetConsoleTextAttribute(stdOut,0x0e)
#define ttclose()     SetConsoleTextAttribute(stdOut,0x0e)

#include "eeng.c"

void gotoxy(int horz,int vert)
{
    outxy.X=horz;
    outxy.Y=vert;
    SetConsoleCursorPosition(stdOut,outxy);
}

/* Microsof get a key */
int get_key()
{

    static  char k1[]="<=ABDHPKMRSGOIQ";    /* input func key second byte */
    static  char k2[]="rfbczEXSDngsdvw";    /* F2,F3,F7,F8,F10,up,dn,le,ri,Ins,Del,home,End,PgUp,PgDn*/
    int key1,key2;
    char *s;

    if(((key1=getch()) == 0xE0) || ((key1 == 0))) {
        key2=getch();
#ifdef DEBUG
    {   /* display up to 4 chars of func key input */
        char tbuf[80];
        show_top();
        sprintf(tbuf, "L=%d/%d, C=%d, char %2x_%2x-%2x+%2x",
            ytru+1, ytot, xtru, -1, -1, key1, key2);
        show_note(tbuf);
    }
#endif /* DEBUG */
        if((s=strchr(k1, key2)) == 0)
            return HLP;
        if((key1 = k2[s-k1]) > 'a')
            funckey=1;
        return key1&0x1F;
    }
    return key1;
}

void clreol()   /* clear from cursor to end of line */
{
  int i,ox=outxy.X,oy=outxy.Y;

  for(i=outxy.X; i<screen_width; ++i) putch(BLNK);
  gotoxy(ox,oy);
}

/* clrscr(): clear the screen by filling it with blanks, home cursor*/

void clrscr()
{
    COORD coordScreen = { 0, 0 }; /* here's where we'll home the cursor */
    BOOL bSuccess;
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
    DWORD dwConSize; /* number of character cells in the current buffer */

    /* get the number of character cells in the current buffer */
    bSuccess = GetConsoleScreenBufferInfo(stdOut, &csbi);
    if (!(bSuccess)) return;    /* error return on any failure */

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y; /* figure screen size */

    /* fill the screen with blanks from home cursor position */
    bSuccess = FillConsoleOutputCharacter(stdOut, (TCHAR) ' ',
        dwConSize, coordScreen, &cCharsWritten);
    if (!(bSuccess)) return;

    /* get the current text attribute */
    bSuccess = GetConsoleScreenBufferInfo(stdOut, &csbi);
    if (!(bSuccess)) return;

    /* now set the buffer's attributes accordingly */
    bSuccess = FillConsoleOutputAttribute(stdOut, csbi.wAttributes,
        dwConSize, coordScreen, &cCharsWritten);
    return;
}

#else /* not MSVC */

/******************** Start of termcap I/O *******************/
/*  uses termios.h for termcap I/O on quick and dirty Editor */

/* portable interface defines. */

#define getstate(p)     ((void) tcgetattr(0, (p)))
#define w_setstate(p)   ((void) tcsetattr(0, TCSADRAIN, (p)))

#define oflush()        (void) fflush(stdout)

#define cprintf(x,y)    fprintf(stdout,x,y)
#define cputs(s)        fputs(s,stdout)
#define putch(c)        putchar(c)
#define getch()         getchar()
#define ttclose()       sys_endv()
#define clrscr()        erase()

#define highvideo()     tputs(SO, 1, foutch)
#define lowvideo()      tputs(SE, 1, foutch)

/* Termcap-related declarations. */
extern  char    *tgetstr();
extern  char    *tgoto();

typedef struct termios  Termstate;

static Termstate    cooked_state, raw_state;

/* Local termios func declarations. */
void foutch(int c);
static void fail(char *str);
void ttopen();
void ttyopen();
void sys_endv();
void erase();
void gotoxy(int horz,int vert);
void clreol();

/* termios data declarations. */
unsigned int    COLS = 0;       /* screen dimensions; 0 at start */
unsigned int    LINES = 0;

/* termcap string, num and boolean defs. */
static  char    *CL;            /* erase line/display */
static  char    *CM;            /* cursor motion string */
static  char    *SO, *SE;       /* standout mode start/end */
//static  char    *colours[10];   /* colour caps c0 .. c9 */
//static  int     ncolours;       /* number of colour caps we have */

/* use MS style COORD to track cursor pos */
typedef struct {
    int X;      /* x coordinate */
    int Y;      /* y coordinate */
} COORD;        /* x-y structure */

COORD outxy;    /* cursor coordinates for screen positioning */

/* qde state info */
static enum {
    m_SYS = 0,
    m_QDE
}   termmode;

#include "eeng.c"

/* tputs char output callback func */
void foutch(int c)
{
    putchar(c);
}

/* fail() is called by ttyopen() on terminal open failure */

static void fail(char *str)
{
    /* Assume we are in raw mode already, so go back into cooked. */

    (void) fputs(str, stderr);  /* tell operator why failed */
    putc('\n', stderr);
    sys_endv();

    exit(2);                    /* tell OS we failed */
}

/* Initialise the terminal so that we can do single-character I/O */

void ttopen()
{
    /* Set up tty flags in raw and cooked structures.
     * Do this before any termcap-initialisation stuff
     * so that start sequences can be sent.*/

    getstate(&cooked_state);
    raw_state = cooked_state;

    cfmakeraw(&raw_state);

    /* Now set up the terminal interface. */
    ttyopen();

    /* Go into raw/cbreak mode, and say so. */
    w_setstate(&raw_state);

    termmode = m_QDE;
}

/* Look up term entry in termcap database, and set up all the strings. */

void ttyopen()
{
    char    tcbuf[4096];        /* buffer for termcap entry */
    char    *termtype;          /* terminal type */
    static  char strings[512];  /* space for storing strings */
    char    *strp = strings;    /* ptr to space left in strings */
    int iv;

    termtype = getenv("TERM");
    if (termtype == NULL) fail("Can't find TERM in environment.");

    switch (tgetent(tcbuf, termtype)) {
    case -1: fail("Can't open termcap.");    /*NOTREACHED*/
    case 0:  fail("Can't find entry for your terminal in termcap.");
    }

    /* Screen dimensions. */
    iv = tgetnum("co");
    /* check that x is there */
    if (iv <= 0) fail("`co' entry in termcap is invalid or missing.");

    COLS = (unsigned) iv;

    iv = tgetnum("li");
    /* check that y is there */
    if (iv <= 0) fail("`li' entry in termcap is invalid or missing.");

    LINES = (unsigned) iv;

    /* set output screen height and width */
    screen_height=LINES-3;
    screen_width=COLS;

    /* output control */
    CL = tgetstr("cl", &strp);  /* clear erase */
    CM = tgetstr("cm", &strp);  /* move goto */
    SO = tgetstr("so", &strp);  /* standout mode start */
    SE = tgetstr("se", &strp);  /* standout mode end */

    if (CM == NULL) {
        fail("qde can't work without cursor motion.");
    }

}

/* ReSet terminal into the mode it was in when we started. */

void sys_endv()
{
    if (termmode == m_SYS) return;

    (void) fflush(stdout);

    /* Restore terminal modes. */
	w_setstate(&cooked_state);
    termmode = m_SYS;
}

/* Erase display (may optionally home cursor). */

void erase()
{
    tputs(CL, (int) LINES, &foutch);
}

/* Goto the specified location. */

void gotoxy(int horz,int vert)
{
    outxy.X=horz;
    outxy.Y=vert;

    tputs(tgoto(CM, horz, vert), (int) LINES, &foutch);
}

/* Erase from cursor to end of line. */

void clreol()
{
  int i,ox=outxy.X,oy=outxy.Y;

  for(i=outxy.X; i<screen_width; ++i) putch(BLNK);
  gotoxy(ox,oy);
}

/* get a key */

int get_key()
{
    static  char k1[]="ABCD256"; /* up,dn,rt,le,ins,pgup,pgdn */
    static  char k2[]="EXDSnvw";
    int key1=0,key2=0,key3=0,key4=0;
    char    *s;

    if((key1 = getch()) == -1) {    /* -1 is ERR */
        key1 = getch();
    }
    if(key1 == 127) return 8; /* delete prev char */
    if(key1 == 27) {
        key2 = getch();
        key3 = getch();
        if ((key3 <= 0x40)||(key3 >= 0x5b)) key4 = getch();
        else key4 = 0x7e;
        if ((key4 != 0x7e)&(key3 != 0x5b)) { getch();}

        if((s=strchr(k1, key3)) != NULL && (key1 = k2[s-k1]) > 'a')
            funckey=1;
        key1=key1&0x1F;
        if (key1==0x1b) key1=0;
    }
#ifdef DEBUG
    {
        char tbuf[80];
        show_top();
        sprintf(tbuf, "L=%d/%d, C=%d, char %2x_%2x-%2x+%2x",
            ytru+1, ytot, xtru, key1, key2, key3, key4);
        show_note(tbuf);
    }
#endif /* DEBUG */
    return key1;
}

/********************** End of termcap input ***********************/

#endif /* not MSVC */

/* End of I/O specific code****************************** */

/* ctrl K keys */
void ctrlk_key(int key)
{
	switch(key | 0x60) {
		case 'b': block_mark(); break;		/* ^K^B set mark block on */
		case 'c': block_copy(0); break; 	/* ^K^C block copy to bbuffer */
		case 'd': flag[CHG]=0; break;		/* ^K^D say file not changed */
		case 'f': file_save(0,-1); break;	/* ^K^F new file */
		case 'h': show_help(0); break;		/* ^K^H show help menu */
		case 'k': block_mark(); break;		/* ^K^K set mark block on */
		case 'm': key_macros(1); break; 	/* ^K^M record macro */
		case 'n': chgdir(); break;			/* ^K^N get and change dir */
		case 'p': key_macros(0); break; 	/* ^K^P play macro */
		case 'q': sys_exit(0); break;		/* ^K^Q exit only if file saved */
		case 'r': block_read(); break;		/* ^K^R read file into cursor pos */
		case 's': do_save(); break; 		/* ^K^S save file and resume */
		case 't': tab_size(); break;		/* ^K^T get tab size */
		case 'u': redo(); break;			/* ^K^U redo */
		case 'v': block_paste(); break; 	/* ^K^V copy buffer to cursor */
		case 'w': block_write(); break; 	/* ^K^W write block to disk */
		case 'x': sys_exit(0); break;		/* ^K^X exit only if file saved */
		case 'y': block_copy(1); break; 	/* ^K^Y cut block to buffer */
//		case 'z': mterm(); break;			/* ^K^Z open an rxvt term */
	}
	doCtrlK = 0;
	show_top();
}

/* ctrl Q keys */
void ctrlq_key(int key)
{
	switch(key | 0x60) {
		case 'a': goto_replace(0); break;	/* ^Q^A replace */
		case 'c': goto_y(ytot); 			/* ^Q^C eof */
		case 'd': goto_x(strlen(line_start+1)+1); break;	/* ^Q^D eol */
		case 'f': goto_search(0); break;	/* ^Q^F find */
		case 'i': goto_line(); break;		/* ^Q^I goto line */
		case 'l': chg_case(0); break; 		/* ^Q^L lower case block */
		case 'm': window_size(); break; 	/* ^Q^M get right margin */
		case 'p': goto_last(); break;		/* ^Q^P goto last pos */
		case 'r': top();					/* ^Q^R bof */
		case 's': goto_x(1); break; 		/* ^Q^S bol */
		case 'u': chg_case(1); break; 		/* ^Q^L upper case block */
		case 'y': key_delword(1); break;	/* ^Q^Y delete to end of line */
	}
	doCtrlQ = 0;
	show_top();
}

/* Control keys */
void key_control(int key)
{
	switch(key|0x60) {
		case 'a': word_left(); break;			/* word left */
		case 'b': block_format(); break;		/* reformat block */
		case 'c': cursor_pagedown(); break; 	/* pgdn */
		case 'd': cursor_right(); break;		/* right */
		case 'e': cursor_up(); break;			/* up */
		case 'f': word_right(); break;			/* word right */
		case 'g': key_delete(); break;			/* delete cursor char */
		case 'h': key_backspace(); break;		/* destructive backspace */
		case 'i': key_tab(0); break;			/* insert tab char */
		case 'j': key_return(); break;		/* newline */
		case 'k': doCtrlK = key; break; 		/* ^K key */
		case 'l': goto_find(cur_pos, 0); break; /* find again */
		case 'm': 
		case 'n': key_return(); break;			/* newline at cursor */
		case 'o': show_mode(); break;			/* change modes */
		case 'p': literal = 1; break;			/* get inline literal */
		case 'q': doCtrlQ = key; break; 		/* ^Q key */
		case 'r': cursor_pageup(); break;		/* pgup */
		case 's': cursor_left(); break; 		/* left */
		case 't': key_delword(0); break;		/* delete word/to word */
		case 'u': undo(); break;				/* undo */
		case 'v': show_flag(OVR,!flag[OVR]);
				  break;						/* toggle Insert mode */
		case 'w': scroll_down(); break; 		/* scroll down */
		case 'x': cursor_down(); break; 		/* down */
		case 'y': block_line(); break;			/* delete line */
		case 'z': scroll_up(); break;			/* scroll up */
	}
	show_top();
}

/* main function */
void main_meta(int key)
{
    switch(key | 0x60) {
        case 'b': block_mark(); break;          /* F7 set mark block on */
        case 'c': block_mark(); break;          /* F8 set mark block on */
        case 'd': goto_x(strlen(line_start+1)+1); break;/* End eol */
        case 'f': file_save(0,-1); break;		/* F3 open new file */
        case 'g': key_backspace(); break;		/* Backspace delete prev char */
        case 'n': show_flag(OVR, !flag[OVR]); break;/* Ins toggle insert mode */
        case 'r': do_save(); break;             /* F2 save file and resume */
        case 's': goto_x(1); break;             /* Home bol */
        case 'v': cursor_pageup(); break;       /* PgUp */
        case 'w': cursor_pagedown(); break;     /* PgDn */
//		case 'z': key_shell(0); break;          /* F10 execute a shell */
    }
    funckey=0;
    show_top();
}

/* undraw, redraw status and display */
void scr_update()
{
	if(executive != MAIN) return;

	/* update text buffer display */
	if( flag[BLK] ) flag[SHW] = 1;
	if(y <= -1 || y >= screen_height) {
		if(y == -1) {
			y++; show_sdn(0);
		}
		else if(y == screen_height) {
			y--; show_sdn(0);
		}
		else {
			y = y < 0 ? 0 : screen_height-1;
			show_scr(0, screen_height-1);
		}
	}
	else if(flag[SHW] ) {
		show_scr(0, screen_height-1);
	}
	flag[SHW] = 0;

	show_pos();
	lowvideo();
	gotoxy(x-1,y+y2);
}

/* single char interpreter */
void main_exec(int key)
{
	cur_pos = get_cur();
	if(update_scr) {
		if(flag[REC]) rec_macro(key);
	}
	if(help_done){
		help_done = 0;
		flag[SHW] = 1;
	} else if(literal) {
		key_normal(key);
		literal = 0;
	} else {
		if(funckey ) main_meta(key); else
		if(doCtrlK) ctrlk_key(key); else
		if(doCtrlQ) ctrlq_key(key); else {
			if(key >= BLNK)  key_normal(key);
			else key_control(key);
		}
	}
	if(!doCtrlK && !doCtrlQ && old_pos != line_start) {
		last_pos = old_pos;
		old_pos = line_start;
	}
	cur_pos = get_cur();

	if(update_scr && exitf && executive == MAIN) scr_update();
}

int main(int argc,char *argv[])
{
	int skey;

/* edit engine init */

#ifndef STATICBUF
	/* alloc memory for the main buffer and block buffer */
	edbuf = (char *) malloc((size_t)(amax+tabsize+1));
	bb = (char *) malloc((size_t)(bmax+tabsize+1));
	unbuf = (void *) malloc((size_t)(umax+tabsize+1));

	if (!edbuf || !bb || !unbuf) {
		fprintf(stderr,"Memory allocation failed, aborting !!\n");
		exit(1);
	}
#endif /* STATICBUF */

    ttopen();
    phys_height = screen_height;
    y1 = YTOP;
	x = 1; y=0;

	/* get command line options */
	if(argc > 1){
		while(**++argv == '-') {
			--argc;
			switch(*++*argv) {
				case 't': tabsize = atoi(++*argv); break;
				case '+': ewin.jump = atoi(++*argv); break;
			}
		}
	}
	/* set path */
	if (cfdpath == NULL) {
		cfdpath = (char *) malloc(BUFSIZ);
		getcwd(cfdpath, BUFSIZ);
#ifdef MSVC
		if (strcmp(cfdpath, "\\") != 0)
			strcat(cfdpath, "\\");
#else
		if (strcmp(cfdpath, "/") != 0)
			strcat(cfdpath, "/");
#endif /* MSVC */
	}

	/* open the named file for editing */
	{
		char* p=strchr(*argv,':');
		if(p) {
			*p++ = '\0';
			ewin.jump = atoi(p);
		}
	}
	strcpy(ewin.name, (argc == 1) ? "": *argv);

	do_open();
	scr_update();

/* end of edit engine init */
	/* main event loop, dispatch function calls in response to events */
	while(exitf)  {
		skey = get_key();
		switch(executive) {
			case MAIN:
				main_exec(skey);
				break;
			case DIALOG:
				dialog(skey);
				break;
			case OPTIONS:
				options(skey);
				break;
		}
	}
    return(0);
}
