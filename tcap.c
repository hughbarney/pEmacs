/* termios video driver */

#define	termdef	1		/* don't define "term" external */

#include <stdlib.h> /* exit */
#include <stdio.h>		/* puts(3), snprintf(3) */
#include "estruct.h"
#include "edef.h"
#undef CTRL
#include <sys/ioctl.h>

extern int tgetent();
extern char *tgetstr();
extern char *tgoto();
extern void tputs();

extern char *getenv();
extern void ttopen();
extern int ttgetc();
extern void ttputc();
extern void ttflush();
extern void ttclose();

void getwinsize();
void tcapopen();
void tcapmove(int row, int col);
void tcapeeol();
void tcapeeop();
void tcaprev();
void tcapbeep();

#define	MARGIN 8
#define	SCRSIZ 64
#define BEL 0x07
#define TCAPSLEN 64

char tcapbuf[TCAPSLEN];		/* capabilities actually used */
char *CM, *CE, *CL, *SO, *SE;

TERM term = {
  0, 0, MARGIN, SCRSIZ, tcapopen, ttclose, ttgetc, ttputc,
  ttflush, tcapmove, tcapeeol, tcapeeop, tcapbeep, tcaprev
};

void getwinsize ()
{
  int cols, rows;
#ifndef FORCE_COLS
  struct winsize ws;

  ioctl (0, TIOCGWINSZ, &ws);
  cols = ws.ws_col;
  rows = ws.ws_row;
#else
  cols = FORCE_COLS;
  rows = FORCE_ROWS;
#endif
  if ((cols < 10) || (rows < 3))
    {
      puts ("Unbelievable screen size\n");
      exit (1);
    }
  term.t_ncol = cols;
  term.t_nrow = rows-1;
}

void tcapopen ()
{
  char tcbuf[1024], err_str[72];
  char *p, *tv_stype;

  if ((tv_stype = getenv ("TERM")) == NULL)
    {
      puts ("Environment variable TERM not defined\n");
      exit (1);
    }
  if ((tgetent (tcbuf, tv_stype)) != 1)
    {
      snprintf (err_str, 72, "Unknown terminal type %s\n", tv_stype);
      puts (err_str);
      exit (1);
    }
  p = tcapbuf;
  CL = tgetstr ("cl", &p);
  CM = tgetstr ("cm", &p);
  CE = tgetstr ("ce", &p);
  SE = tgetstr ("se", &p);
  SO = tgetstr ("so", &p);

  if (CE == NULL)
    eolexist = FALSE;
  if (SO != NULL && SE != NULL)
    revexist = TRUE;
  if (CL == NULL || CM == NULL)
    {
      puts ("Insufficient termcap! (needs cl & cm abilities)\n");
      exit (1);
    }
  if (p >= &tcapbuf[TCAPSLEN])	/* XXX */
    {
      puts ("Terminal description too big!\n");
      exit (1);
    }
  ttopen ();
}

void tcaprev (int state)
{
  if (revexist)
    tputs ((state ? SO : SE), 1, ttputc);
}

void tcapmove (int row, int col)
{
  tputs (tgoto (CM, col, row), 1, ttputc);
}

void tcapeeol ()
{
  tputs (CE, 1, ttputc);
}

void tcapeeop ()
{
  tputs (CL, 1, ttputc);
}

void tcapbeep ()
{
  ttputc (BEL);
}
