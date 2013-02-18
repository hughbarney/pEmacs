/*
 * This file contains the command processing functions for a number of random
 * commands. There is no functional grouping here, for sure.
 */

#include "estruct.h"
#include "edef.h"

extern void mlwrite ();
extern void lchange (int flag);
extern int lnewline ();
extern int linsert (int n, int c);
extern int backchar (int f, int n);
extern void kdelete ();
extern int ldelete (int f, int n);
extern int kremove (int k);

int showcpos (int f, int n);
int getccol (int bflg);
int quote (int f, int n);
int tab (int f, int n);
int newline (int f, int n);
int forwdel (int f, int n);
int backdel (int f, int n);
int killtext (int f, int n);
int yank (int f, int n);

/*
 * Display the current position of the cursor, in origin 1 X-Y coordinates,
 * the character that is under the cursor (in hex), and the fraction of the
 * text that is before the cursor. The displayed column is not the current
 * column, but the column that would be used on an infinite width display.
 * Normally this is bound to "C-X ="
 *
 * This version from uEmacs/PK based on MicroEMACS 3.09e
 */
/* ARGSUSED */
int showcpos(int f, int n)
{
	LINE *lp;	/* current line */
	long numchars;	/* # of chars in file */
	int numlines;	/* # of lines in file */
	long predchars;	/* # chars preceding point */
	int predlines;	/* # lines preceding point */
	int curchar;	/* character under cursor */
	int ratio;
	int col;
	int savepos;		/* temp save for current offset */
	int ecol;		/* column pos/end of current line */

	/* starting at the beginning of the buffer */
	lp = lforw(curbp->b_linep);

	/* start counting chars and lines */
	numchars = 0;
	numlines = 0;
	predchars = 0;
	predlines = 0;
	curchar = 0;
	while (lp != curbp->b_linep) {
		/* if we are on the current line, record it */
		if (lp == curwp->w_dotp) {
			predlines = numlines;
			predchars = numchars + curwp->w_doto;
			if ((curwp->w_doto) == llength(lp))
				curchar = '\n';
			else
				curchar = lgetc(lp, curwp->w_doto);
		}
		/* on to the next line */
		++numlines;
		numchars += llength(lp) + 1;
		lp = lforw(lp);
	}

	/* if at end of file, record it */
	if (curwp->w_dotp == curbp->b_linep) {
		predlines = numlines;
		predchars = numchars;
	}

	/* Get real column and end-of-line column. */
	col = getccol(FALSE);
	savepos = curwp->w_doto;
	curwp->w_doto = llength(curwp->w_dotp);
	ecol = getccol(FALSE);
	curwp->w_doto = savepos;

	ratio = 0;		/* Ratio before dot. */
	if (numchars != 0)
		ratio = (100L * predchars) / numchars;

	/* summarize and report the info */
	mlwrite("Line %d/%d Col %d/%d Char %d/%d (%d%%) char = 0x%x",
		predlines + 1, numlines + 1, col, ecol,
		predchars, numchars, ratio, curchar);
	return TRUE;
}


/*
 * Return current column.  Stop at first non-blank given TRUE argument.
 */
int getccol (int bflg)
{
  int c, i, col;

  col = 0;
  for (i = 0; i < curwp->w_doto; ++i)
    {
      c = lgetc (curwp->w_dotp, i);
      if (c != ' ' && c != '\t' && bflg)
	break;
      if (c == '\t')
	col |= 0x07;
      else if (c < 0x20 || c == 0x7F)
	++col;
      ++col;
    }
  return (col);
}

/*
 * Quote the next character, and insert it into the buffer. All the characters
 * are taken literally, with the exception of the newline, which always has
 * its line splitting meaning. The character is always read, even if it is
 * inserted 0 times, for regularity. Bound to "C-Q"
 */
int quote (int f, int n)
{
  int s, c;

  c = (*term.t_getchar) ();
  if (n < 0)
    return (FALSE);
  if (n == 0)
    return (TRUE);
  if (c == '\n')
    {
      do
	{
	  s = lnewline ();
	}
      while (s == TRUE && --n);
      return (s);
    }
  return (linsert (n, c));
}

/*
 * Insert a tab into file.
 * Bound to "C-I"
 */
int tab (int f, int n)
{
  if (n < 0)
    return (FALSE);
  return (linsert (n, 9));
}

/*
 * Insert a newline. Bound to "C-M".
 */
int newline (int f, int n)
{
  int s;

  if (n < 0)
    return (FALSE);

  /* insert some lines */
  while (n--)
    {
      if ((s = lnewline ()) != TRUE)
	return (s);
    }
  return (TRUE);
}

/*
 * Delete forward. This is real easy, because the basic delete routine does
 * all of the work. Watches for negative arguments, and does the right thing.
 * If any argument is present, it kills rather than deletes, to prevent loss
 * of text if typed with a big argument. Normally bound to "C-D"
 */
int forwdel (int f, int n)
{
  if (n < 0)
    return (backdel (f, -n));
  if (f != FALSE)
    {			       /* Really a kill */
      if ((lastflag & CFKILL) == 0)
	kdelete ();
      thisflag |= CFKILL;
    }
  return (ldelete (n, f));
}

/*
 * Delete backwards. This is quite easy too, because it's all done with other
 * functions. Just move the cursor back, and delete forwards. Like delete
 * forward, this actually does a kill if presented with an argument. Bound to
 * both "RUBOUT" and "C-H"
 */
int backdel (int f, int n)
{
  int s;

  if (n < 0)
    return (forwdel (f, -n));
  if (f != FALSE)
    {			       /* Really a kill */
      if ((lastflag & CFKILL) == 0)
	kdelete ();
      thisflag |= CFKILL;
    }
  if ((s = backchar (f, n)) == TRUE)
    s = ldelete (n, f);
  return (s);
}

/*
 * Kill text. If called without an argument, it kills from dot to the end of
 * the line, unless it is at the end of the line, when it kills the newline.
 * If called with an argument of 0, it kills from the start of the line to
 * dot. If called with a positive argument, it kills from dot forward over
 * that number of newlines. If called with a negative argument it kills
 * backwards that number of newlines. Normally bound to "C-K"
 */
int killtext (int f, int n)
{
  LINE *nextp;
  int chunk;

  if ((lastflag & CFKILL) == 0)/* Clear kill buffer if last wasn't a kill */
    kdelete ();
  thisflag |= CFKILL;
  if (f == FALSE)
    {
      chunk = llength (curwp->w_dotp) - curwp->w_doto;
      if (chunk == 0)
	chunk = 1;
    }
  else if (n == 0)
    {
      chunk = curwp->w_doto;
      curwp->w_doto = 0;
    }
  else if (n > 0)
    {
      chunk = llength (curwp->w_dotp) - curwp->w_doto + 1;
      nextp = lforw (curwp->w_dotp);
      while (--n)
	{
	  if (nextp == curbp->b_linep)
	    return (FALSE);
	  chunk += llength (nextp) + 1;
	  nextp = lforw (nextp);
	}
    }
  else
    {
      mlwrite ("neg kill");
      return (FALSE);
    }
  return (ldelete (chunk, TRUE));
}

/*
 * Yank text back from the kill buffer. This is really easy. All of the work
 * is done by the standard insert routines. All you do is run the loop, and
 * check for errors. Bound to "C-Y"
 */
int yank (int f, int n)
{
  int c, i;

  if (n < 0)
    return (FALSE);
  while (n--)
    {
      i = 0;
      while ((c = kremove (i)) >= 0)
	{
	  if (c == '\n')
	    {
	      if (lnewline (FALSE, 1) == FALSE)
		return (FALSE);
	    }
	  else
	    {
	      if (linsert (1, c) == FALSE)
		return (FALSE);
	    }
	  ++i;
	}
    }
  return (TRUE);
}
