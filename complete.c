#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "estruct.h"
#include "edef.h"
#include "efunc.h"

extern void mlwrite (char *,...);
extern int ttgetc();
extern void ttputc();
extern void ttflush();
extern int ttcol;

int getfilename(char *, char *, int);
void outstring(char *);

/*
 * basic filename completion, based on code in uemacs/PK
 */
int getfilename(char *prompt, char *buf, int nbuf)
{
	int cpos = 0;	/* current character position in string */
	int c;
	int ocpos, nskip = 0, didtry = 0;
	int eolchar = '\n';
	int result = 0;

	static char tmp[] = "/tmp/meXXXXXX";
	FILE *tmpf = NULL;

	/* prompt the user for the input string */
	mlwrite(prompt);

	for (;;) {
		if (!didtry)
			nskip = -1;
		didtry = 0;

		/* get a character from the user */
		c = ttgetc();

		/* If it is a <ret>, change it to a <NL> */
		if (c == CTRL_M)
			c = '\n';

		/* if they hit the line terminate, wrap it up */
		if (c == eolchar) {
			buf[cpos++] = 0;

			/* clear the message line */
			mlwrite("");
			ttflush();

			/* if we default the buffer, return FALSE */
			if (buf[0] == 0)
				return FALSE;

			return TRUE;
		}

		if (c == CTRL_G) {
			/* Abort the input? */
			ctrlg(FALSE, 0);
			ttflush();
			return ABORT;
		} else if ((c == 0x7F || c == 0x08)) {
			/* rubout/erase */
			if (cpos != 0) {
				outstring("\b \b");
				--ttcol;
				if (buf[--cpos] < 0x20) {
					outstring("\b \b");
					--ttcol;
				}
				if (buf[cpos] == '\n') {
					outstring("\b\b  \b\b");
					ttcol -= 2;
				}
				ttflush();
			}
		} else if (c == 0x15) {
			/* C-U, kill */
			while (cpos != 0) {
				outstring("\b \b");
				--ttcol;

				if (buf[--cpos] < 0x20) {
					outstring("\b \b");
					--ttcol;
				}
				if (buf[cpos] == '\n') {
					outstring("\b\b  \b\b");
					ttcol -= 2;
				}
			}
			ttflush();
		} else if ((c == 0x09 || c == ' ')) {
			/* TAB, complete file name */
			char ffbuf[255];
			int n, iswild = 0;

			didtry = 1;
			ocpos = cpos;
			while (cpos != 0) {
				outstring("\b \b");
				--ttcol;

				if (buf[--cpos] < 0x20) {
					outstring("\b \b");
					--ttcol;
				}
				if (buf[cpos] == '\n') {
					outstring("\b\b  \b\b");
					ttcol -= 2;
				}
				if (buf[cpos] == '*' || buf[cpos] == '?')
					iswild = 1;
			}
			ttflush();
			if (nskip < 0) {
				buf[ocpos] = 0;
				if (tmpf != NULL)
					fclose(tmpf);
				strcpy(tmp, "/tmp/meXXXXXX");
				strcpy(ffbuf, "echo ");
				strcat(ffbuf, buf);
				if (!iswild)
					strcat(ffbuf, "*");
				strcat(ffbuf, " >");
				result = mkstemp(tmp);
				if (result == -1)
				{
				  printf("Failed to create temp file\n");
				  exit(1);
				}
				strcat(ffbuf, tmp);
				strcat(ffbuf, " 2>&1");
				result = system(ffbuf);
				tmpf = fopen(tmp, "r");
				nskip = 0;
			}
			c = ' ';
			for (n = nskip; n > 0; n--)
				while ((c = getc(tmpf)) != EOF
				       && c != ' ');
			nskip++;

			if (c != ' ') {
				(*term.t_beep) ();
				nskip = 0;
			}
			while ((c = getc(tmpf)) != EOF && c != '\n'
			       && c != ' ' && c != '*')
			{
				if (cpos < nbuf - 1)
					buf[cpos++] = c;
			}
			if (c == '*')
				(*term.t_beep) ();

			for (n = 0; n < cpos; n++) {
				c = buf[n];
				if ((c < ' ') && (c != '\n')) {
					outstring("^");
					++ttcol;
					c ^= 0x40;
				}

				if (c != '\n') {
					ttputc(c);
				} else {	/* put out <NL> for <ret> */
					outstring("<NL>");
					ttcol += 3;
				}
				++ttcol;
			}
			ttflush();
			rewind(tmpf);
			unlink(tmp);
		} else {
			if (cpos < nbuf - 1) {
				/* if a control char */
				if ((c < ' ') && (c != '\n')) {
				  (*term.t_beep) ();
				} else {
				  buf[cpos++] = c;
				  ttputc(c);
				  ++ttcol;
				  ttflush();
				}
			}
		}
	}
}

/*
 * output a string of characters
 * char *s;		string to output
 */
void outstring(char *s)
{
  while (*s)
	ttputc(*s++);
}
