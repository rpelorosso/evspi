/* 
   Taking control of hardare interrupt 9
   Will compile Borland, Microsoft, and Watcom C\C++ compilers.
   By Dhonn Lushine Ludhonn@aol.com

   This will help you contol keyboard input devices.
   Why use getch() or kbhit() or assembler int 16h,
   the computer will beep and freeze you progam momentarily.
   So just use the keyboard directly.
   It can also prevent keyboard warning beep (buffer full), and test for
   mulitple key presses.
   It will also speed up the code because it does not need to be called.
   The hardware will call it.

   Notes: Do not use int 16h, getch(), or any keyboard input function
	  while this keyboard interrupt is installed.

   Warnings: I am not reasponsible for any damages.
*/
					/* #define the keys
					   This was hard to find,
					   I had to press all the keys */
#define KEY_ESC			0x01
#define KEY_1			0x02
#define KEY_2			0x03
#define KEY_3			0x04
#define KEY_4			0x05
#define KEY_5			0x06
#define KEY_6			0x07
#define KEY_7			0x08
#define KEY_8			0x09
#define KEY_9			0x0a
#define KEY_0			0x0b
#define KEY_MINUS		0x0c
#define KEY_EQUALS		0x0d
#define KEY_BAKSP		0x0e
#define KEY_TAB			0x0f
#define KEY_Q			0x10
#define KEY_W			0x11
#define KEY_E			0x12
#define KEY_R			0x13
#define KEY_T			0x14
#define KEY_Y			0x15
#define KEY_U			0x16
#define KEY_I			0x17
#define KEY_O			0x18
#define KEY_P			0x19
#define KEY_LFT_BRACKET		0x1a
#define KEY_RGT_BRACKET		0x1b
#define KEY_ENTER		0x1c
#define KEY_CTRL		0x1d
#define KEY_A			0x1e
#define KEY_S			0x1f
#define KEY_D			0x20
#define KEY_F			0x21
#define KEY_G			0x22
#define KEY_H			0x23
#define KEY_J			0x24
#define KEY_K			0x25
#define KEY_L			0x26
#define KEY_SEMI		0x27
#define KEY_APOS		0x28
#define KEY_TILDE		0x29
#define KEY_LSHIFT		0x2a
#define KEY_BACK_SLASH		0x2b
#define KEY_Z			0x2c
#define KEY_X			0x2d
#define KEY_C			0x2e
#define KEY_V			0x2f
#define KEY_B			0x30
#define KEY_N			0x31
#define KEY_M			0x32
#define KEY_COMMA		0x33
#define KEY_PERIOD		0x34
#define KEY_FWD_SLASH		0x35
#define KEY_RSHIFT		0x36
#define KEY_PRT_SCRN		0x37
#define KEY_ALT			0x38
#define KEY_SPACE		0x39
#define KEY_CAPS_LOCK		0x3a
#define KEY_F1			0x3b
#define KEY_F2			0x3c
#define KEY_F3			0x3d
#define KEY_F4			0x3e
#define KEY_F5			0x3f
#define KEY_F6			0x40
#define KEY_F7			0x41
#define KEY_F8			0x42
#define KEY_F9			0x43
#define KEY_F10			0x44
#define KEY_F11			0x57
#define KEY_F12			0x58
#define KEY_NUM_LOCK		0x45
#define KEY_SCROLL_LOCK		0x46
#define KEY_HOME		0x47
#define KEY_UP			0x48
#define KEY_PGUP		0x49
#define KEY_KEYPAD_MINUS	0x4a
#define KEY_LEFT		0x4b
#define KEY_CENTER		0x4c
#define KEY_RIGHT		0x4d
#define KEY_KEYPAD_PLUS		0x4e
#define KEY_END			0x4f
#define KEY_DOWN		0x50
#define KEY_PGDWN		0x51
#define KEY_INS			0x52
#define KEY_DEL			0x53

#include <dos.h>			/* has the _dos_setvect and
					   _dos_getvet functions */

int rawkey,keys[128];			/* Holds the scan code
					   and out litte buffer */

/* little sti macro */
#ifdef __WATCOMC__
void enableints(void);
#pragma aux enableints="sti";
#else
#define enableints() _asm sti
#endif

void (interrupt far *oldint9)(void);  	/* Saves the old interrupt, you really need to. */

void interrupt far newint9(void)
{
 register char a;
 rawkey=inp(0x60);			/* Sorry I smashed the code */
 outp(0x61,(a=inp(0x61)|0x82));		/* you really dont need the info */
 outp(0x61,a&0x7f);			/* of what it does. Email me if you need it */

 outp(0x20,0x20);			/* Renable hardware interrupts */

 if(rawkey<128)  keys[rawkey]=1;	/* If the key it pressed make the buffer true. */
 else keys[rawkey-128]=0;		/* If the key is release make the buffer false.
					   Yes, the release scan codes are 128 apart
					   from the orginal scan codes. */
 enableints();				/* equal to sti */
}

void install_kb(void)			/* Install interrupt */
{
 oldint9=_dos_getvect(9);		/* Save the old int, we need that */
 _dos_setvect(9,newint9);		/* Set new interrupt code */
}

void uninstall_kb(void)			/* uninstall interrupt */
{
 _dos_setvect(9,oldint9);		/* Install the old interrupt code */
}

/* Now it's time to show you a real world example. */
/* See examp_i9.c */
