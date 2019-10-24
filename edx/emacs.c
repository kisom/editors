
/*
	edx (small emacs clone EDitor for X) (C) 2002, Jean-Pierre Demailly
	released into the public domain. This program comes with no warranties or
	binaries. Compile and use at your own risk.
*/

/* Help screen -- rewrite to suit your own tastes */

static char HELP_STR[] = "EDX (mini emacs clone EDitor for X) Ver. "VERSION" \0\
Usage: edx [-fn font] [-j line#] [-t tab#] [-w width] [-h height]\0\
           [-bg color] [-fg color] [-hibg color] [-hifg color] [-cur color]\0\
           [-rc rcfile] [file]\0\
MFOCTBNAW flags: Mod, Fill, Ovrwrit, Case, Tab, Blk mk, Ndent, repl All, Word)\0\

The Mod flag must be reset (with ^OM) to exit or load new file.\0\
F1 show help      F2  save          F3 open new file  F5 exec command\0\
F6 Chgdir         F10 rxvt          Alt-? private key bindings\0\
Shift-Del cut     Shift-Ins paste   ^Ins copy block to X clipboard\0\
^A  goto bol      ^E  goto eol      ^G  goto line     ^O? switch a flag\0\
^B  left          ^F  right         ^N  down          ^P  up\0\
^D  del char      ^H  del prev char ^J  insert line   ^K  del to eol\0\
^Q? literal char  ^S  find string   ^R  repeat find   ^T  transpose\0\
^V  page down     ^W  cut block     ^Y  paste block   ^_  undo\0\0\
E B word left     E F word right    E N scroll down   E P scroll up\0\
E < start of file E > end of file   E _ redo          E Q reformat paragr\0\
E . set mark      E W copy block    E V page up       E % find & replace\0\
E L lowcase block E U upcase block  E X switch marks  ^F1 Mrk&Opn Str\0\
^XC (^XQ) exit    ^XH show help     ^XS save          ^XF open new file\0\
^XI insert file   ^XV write block   ^XW save as       ^XK del word\0\
^XM togl rec mac  ^XP play macro    ^XT get tab size  ^XU delete line\0\
                  Press any key to continue ...";

/* include the editor engine */
#include "eeng.c"

/* All this stuff consists of the emacs-like bindings */

#ifdef GREEK
void ctrlc_key(int key)
{
	key_greek(key);
	doCtrlC = var_greek;
}
#endif /* GREEK */

/* ctrl X keys */
void ctrlx_key(int key)
{
	switch(key | 0x60) {
		case 'c': sys_exit(0); break;		/* ^XC exit only if file saved */
		case 'f': file_save(0,1); break;	/* ^XH show help menu */
		case 'h': show_help(0); break;		/* ^XH show help menu */
		case 'i': block_read(); break;		/* ^XI read file into cursor pos */
		case 'k': key_delword(0); break;	/* ^XK delete word */
		case 'm': key_macros(1); break; 	/* ^XM record macro */
		case 'p': key_macros(0); break; 	/* ^XP play macro */
		case 'q': sys_exit(2); break;		/* ^XQ exit even if file unsaved */
		case 's': do_save(); break; 		/* ^XS write block to disk */
		case 't': tab_size(); break;		/* ^XT get tab size */
		case 'v': block_write(); break; 	/* ^XV write block to disk */
		case 'w': file_save(0,-1); break;	/* ^XW save file as */
		case 'x': goto_last(); break;		/* ^XX goto last pos */
	}
	doCtrlX = 0;
	show_top();
}

/* Alt key bindings */
void key_alt(int key)
{
	char b[1024];
	if (key=='?') {
           show_help(1);
	   return;
	}
	if (key>=0x20) key = (key|0x60)-'a';
	if (key>=0 && key<26) if (binding[key]) {
		sprintf(b, binding[key], (cfdpath? cfdpath : "./"), ewin.name);
		SYSTEM(b);
	}
}

/* Shift, control and bare Function keys */
void key_func(int key)
{
	if(keve->state & ShiftMask)
		switch(key) {
		case XK_Insert	: do_paste(); return; 			/* Shift Ins paste */
		case XK_Delete	: do_select(1); return;			/* Shift Del cut */
		}
	if((keve->state & ShiftMask) && !flag[BLK] &&
		key >= XK_Home && key <= XK_End) block_mark();	/* enable marked block with Shift */
	if(keve->state & ControlMask) {
		switch(key) {
		case XK_F1		: word_marknopen(); break;		/* ^F1 mark cursor word & open */
		case XK_F3		: newedit(""); break;			/* ^F3 open new edx */
		case XK_Home	: top(); break; 				/* ^Home bof */
		case XK_End 	: goto_y(ytot); break;			/* ^End eof */
		case XK_Left	: word_left(); break;			/* ^Left word left */
		case XK_Right	: word_right(); break;			/* ^Right word right */
		case XK_Insert	: do_select(0); break;			/* ^Ins copy */
		}
	} else
		switch(key) {
		case XK_F1		: show_help(0); break;			/* F1 show help */
		case XK_F2		: do_save(); break; 			/* F2 save file and resume */
		case XK_F3		: file_save(0,1); break;		/* F3 open new file */
		case XK_F5		: run(); break; 				/* F5 get and run cmd */
		case XK_F6		: chgdir(); break; 				/* F6 get & change to dir */
		case XK_F7		: block_mark(); break;			/* F7 set mark block on */
		case XK_F8		: block_mark(); break;			/* F8 set mark block on */
		case XK_F10 	: key_alt('z'); break;				/* F10 open rxvt in cur.dir */
		case XK_F12 	: show_flag(OVR, !flag[OVR]); break;/* Ins toggle insert mode */
		case XK_Return	: key_return(); break;			/* Enter newline at cursor */
		case XK_Tab 	: key_tab(0); break;			/* Tab insert tab char at cursor */
		case XK_BackSpace: key_backspace(); break;		/* BS delete prev char */
		case XK_Insert	: show_flag(OVR, !flag[OVR]); break;/* Ins toggle insert mode */
		case XK_Delete	: key_delete(); break;			/* Del delete cursor char */
		case XK_Page_Up : cursor_pageup(); break;		/* PgUp */
		case XK_Page_Down: cursor_pagedown(); break;	/* PgDn */
		case XK_End 	: goto_x(strlen(line_start+1)+1); break;/* End eol */
		case XK_Home	: goto_x(1); break; 			/* Home bol */
		case XK_Up	: cursor_up(); break;			/* up */
		case XK_Down	: cursor_down(); break; 		/* down */
		case XK_Right	: cursor_right(); break;		/* right */
		case XK_Left	: cursor_left(); break; 		/* left */
	}
	if(!(keve->state & ShiftMask) && flag[BLK]) block_mark();	 // turn off marked block
}

/* Control keys */
void key_control(int key)
{
	switch(key|0x60) {
		case 'a': goto_x(1); break; 		/* bol */
		case 'b': cursor_left(); break; 	/* left */
#ifdef GREEK
		case 'c': doCtrlC = 1; break; 		/* ^C switch */
#endif /* GREEK */
		case 'd': key_delete(); break;		/* delete cursor char */
		case 'e': goto_x(strlen(line_start+1)+1); break;	/* goto eol */
		case 'f': cursor_right(); break;	/* right */
		case 'h': key_backspace(); break;	/* destructive backspace */
		case 'j': key_return(); break;		/* newline at cursor */
		case 'k': key_delword(1); break;	/* delete to eol */
		case 'n': cursor_down(); break; 	/* down */
		case 'o': show_mode(); break; 		/* change modes */
		case 'p': cursor_up(); break;		/* up */
		case 'q': literal = 1; break;		/* literal */
		case 'r': goto_find(cur_pos, 0); break; /* find again */
		case 's': goto_search(0); break;	/* ^S find */
		case 't': transpose(); break;		/* ^T transpose */
		case 'u': block_line(); break;		/* ^U delete line */
		case 'v': cursor_pagedown(); break; 	/* ^V pgdn */
		case 'x': doCtrlX = 1; break;		/* ^X switch */
		case 'y': block_paste(); break; 	/* ^Y copy buffer at cursor */
		case 'w': reset_mark();
                          block_copy(1); break; 	/* ^W cut block at buffer */
		case 0x7f: undo(0); break;		/* ^_ undo */
							/* user typed Ctrl-_ */
	}
	show_top();
}

/* Escape keys */
void key_escape(int key)
{
	switch(tolower(key)) {
		case ' ':
		case '.': block_mark(); break;		/* Esc-BLANK set mark block on */
		case '_': redo(); break;		/* Esc-_ redo */
		case '%': goto_replace(0); break;	/* Esc-% replace */

		case '<': top(); break;			/* Esc-< bof */
		case '>': goto_ptr(file_end); break; 	/* Esc-> eof */
		case 'b': word_left(); break;		/* Esc-B word left */
		case 'f': word_right(); break;		/* Esc-F word right */
		case 'g': keve->state & ControlMask ?
					goto_line() :	/* goto line# */
					key_return(); break;	/* newline */
		case 'l': chg_case(0); break; 		/* Esc-L lower case block */
		case 'm': window_size(); break; 	/* Esc-M get right margin */
                case 'n': scroll_up(); break;		/* Esc-N scroll down (!) */
		case 'p': scroll_down(); break;		/* Esc-P scroll up (!) */
		case 'q': block_format(); break;	/* Esc-Q reformat block */
		case 'u': chg_case(1); break; 		/* Esc-U upper case block */
		case 'v': cursor_pageup(); break;	/* Esc-V pgup */

		case 'w': reset_mark(); block_copy(0); 
                          reset_mark(); break; 		/* Esc-W copy block to buffer */
		case 'x': twist_pos(); break;		/* Esc-X switch between block marks */
	}
	doEscap = 0;
	show_top();
}

/* undraw, redraw status and display */
void scr_update()
{
	if(executive == MAIN){

#ifndef TERMINAL
		undraw_cursor();
#endif /* TERMINAL */
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
		gotoxy(x-1,y+y2);
#ifndef TERMINAL
		draw_cursor();
		show_vbar();
#else
		lowvideo();
#endif /* TERMINAL */
	}
}

/* single char interpreter */
void main_exec(int key)
{
	cur_pos = get_cur();
	if (update_scr) {
#ifndef TERMINAL
		undraw_cursor();
#endif /* TERMINAL */
		if(flag[REC]) rec_macro(key);
	}
	if (help_done){
		help_done = 0;
		flag[SHW] = 1;
	} else if (literal) {
		key_normal(keve->state & ControlMask ? key & 0x1f : key);
		literal = 0;
	} else {
		if (key==XK_Escape) doEscap = 1; else
		if (key & 0xFF00) key_func(key); else
		if (keve->state & Mod1Mask) key_alt(key);
		else {
			if (doEscap) key_escape(key); else
#ifdef GREEK
			if (doCtrlC) ctrlc_key(key); else
#endif /* GREEK */
			if (doCtrlX) ctrlx_key(key); else 
			{
				if (keve->state & ControlMask) key &= 0x1f;
				if (key >= BLNK)  {
					flag[BLK] = 0;
					key_normal(key);
				}
				else key_control(key);
			}
		}
	}
	if(!doCtrlX && old_pos != line_start) {
		last_pos = old_pos;
		old_pos = line_start;
	}
	cur_pos = get_cur();

	if(update_scr) scr_update();
}

