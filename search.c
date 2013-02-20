/* This file is in the public domain. */

/* Origin: $OpenBSD: search.c,v 1.38 2011/01/21 19:10:13 kjell Exp $	*/

/*
 * Search commands.
 *
 * The functions in this file implement the incremental search commands
 * and the query-replace command.
 *
 * The incremental search code is by Rich Ellison.
 * The incremental serch code originated from that used in Mg Emacs
 * https://github.com/Andyvanee/mg.git
 * clone of: http://homepage.boetes.org/software/mg/mg-20110905.tar.gz
 *
 * A small number of modifications were maded to retrofit it to the
 * original ErsatzEmacs code.
 *
 * The query replace code originated from the ErsatzEmacs code
 * https://github.com/stangelandcl/ersatz-emacs.git
 * which was based on MicroEmacs 3.06
 *
 */

#include <ctype.h>
#include <stdio.h>      /* sprintf() */
#include <string.h>		/* strncpy(3), strncat(3) */
#include "estruct.h"
#include "edef.h"
#include "search.h"

typedef int	RSIZE;		/* Type for file/region sizes	 */
typedef short	KCHAR;		/* Type for internal keystrokes	 */

struct srchcom {
	int		 s_code;
	LINE	*s_dotp;
	int		 s_doto;
	int		 s_dotline;
};

extern void mlwrite(char *,...);
extern int getkey(int);
extern int mlreplyt (char *, char *, int, char);
extern void update();
extern int forwchar (int f, int n);
extern int backchar (int f, int n);
extern int setmark(int f, int n);
extern int linsert (int n, int c);
extern int lnewline ();
extern int ldelete (int n, int kflag);
extern int ttgetc();
extern int ctrlg (int f, int n);
extern void tcapbeep ();
extern size_t strlcpy(char *, const char *, size_t);
extern void debug(char *,...);

int forwisearch(int f, int n);
int backisearch(int f, int n);
int qreplace(int f, int n);

int readpattern(char *);
int forwsrch(void);
int backsrch(void);
int forscan (char *patrn, int leavep);

static int	isearch(int);
static void	is_cpush(int);
static void	is_lpush(void);
static void	is_pop(void);
static int	is_peek(void);
static void	is_undo(int *, int *);
static int	is_find(int);
static void	is_prompt(int, int, int);
static void	is_dspl(char *, int);
static int	eq(int, int, int);


static struct srchcom	cmds[NSRCH];
static int	cip;

int		srch_lastdir = SRCH_NOPR;	/* Last search flags.	 */

/*
 * Use incremental searching, initially in the forward direction.
 * isearch ignores any explicit arguments.
 */
/* ARGSUSED */
int
forwisearch(int f, int n)
{
  return (isearch(SRCH_FORW));
}

/*
 * Use incremental searching, initially in the reverse direction.
 * isearch ignores any explicit arguments.
 */
/* ARGSUSED */
int
backisearch(int f, int n)
{
  return (isearch(SRCH_BACK));
}

/*
 * Incremental Search.
 *	dir is used as the initial direction to search.
 *	^S	switch direction to forward
 *	^R	switch direction to reverse
 *	<ESC><CTRL+M)	exit from Isearch
 *	<DEL>	undoes last character typed. (tricky job to do this correctly).
 *	other ^ exit search, don't set mark
 *	else	accumulate into search string
 */
static int
isearch(int dir)
{
	LINE 	*clp;		/* Saved line pointer */
	int		 c;
	int		 cbo;		/* Saved offset */
	int		 success;
	int		 pptr;
	char	 opat[NPAT];

	for (cip = 0; cip < NSRCH; cip++)
		cmds[cip].s_code = SRCH_NOPR;

	(void)strlcpy(opat, pat, sizeof(opat));
	cip = 0;
	pptr = -1;
	clp = curwp->w_dotp;
	cbo = curwp->w_doto;
	is_lpush();
	is_cpush(SRCH_BEGIN);
	success = TRUE;
	is_prompt(dir, TRUE, success);

	for (;;) {
		update();
		c = ttgetc();

		switch (c) {
		case ESC:
			/*
			 * If new characters come in the next 300 msec,
			 * we can assume that they belong to a longer
			 * escaped sequence so we should ungetkey the
			 * ESC to avoid writing out garbage.
			 *
			 * BUG: The Mg code had these lines, however
			 * I was not able to get ttwait() to indicate that
			 * chars had arrived in the input buffer as it 
			 * always returned TRUE. So it made no difference.
			 *
			 * if (ttwait(300) == FALSE)
			 *   ungetkey(c);
			 *
			 * What will happen is that if the user hits
			 * up-arrow during an isearch then it terminates
			 * and [A gets output at the last search position.
			 * The same issue can also be observed in uEmacs/Pk
			 */

			/* FALLTHRU */
		case CTRL_M:
			srch_lastdir = dir;
			curwp->w_markp = clp;
			curwp->w_marko = cbo;
			setmark(0,0);
			return (TRUE);

		case CTRL_G:
			if (success != TRUE) {
				while (is_peek() == SRCH_ACCM)
					is_undo(&pptr, &dir);
				success = TRUE;
				is_prompt(dir, pptr < 0, success);
				break;
			}
			curwp->w_dotp = clp;
			curwp->w_doto = cbo;
			curwp->w_flag |= WFMOVE;
			srch_lastdir = dir;
			(void)strlcpy(pat, opat, NPAT);
			(void)ctrlg(FFRAND, 0);
			return (ABORT);
		case CTRL_S:
			if (dir == SRCH_BACK) {
				dir = SRCH_FORW;
				is_lpush();
				is_cpush(SRCH_FORW);
				success = TRUE;
			}
			if (success == FALSE && dir == SRCH_FORW) {
				/* wrap the search to beginning */
				curwp->w_dotp = bfirstlp(curbp);
				curwp->w_doto = 0;
				if (is_find(dir) != FALSE) {
					is_cpush(SRCH_MARK);
					success = TRUE;
				}
				mlwrite("Overwrapped I-search: %s", pat);
				break;
			}

			is_lpush();
			pptr = strlen(pat);
			(void)forwchar(FFRAND, 1);
			if (is_find(SRCH_FORW) != FALSE)
				is_cpush(SRCH_MARK);
			else {
				(void)backchar(FFRAND, 1);
				(*term.t_beep) ();
				success = FALSE;
				mlwrite("Failed I-search: %s", pat);
			}
			is_prompt(dir, pptr < 0, success);
			break;
		case CTRL_R:
			if (dir == SRCH_FORW) {
				dir = SRCH_BACK;
				is_lpush();
				is_cpush(SRCH_BACK);
				success = TRUE;
			}
			if (success == FALSE && dir == SRCH_BACK) {
				/* wrap the search to end */
				curwp->w_dotp = blastlp(curbp);
				curwp->w_doto = llength(curwp->w_dotp);
				if (is_find(dir) != FALSE) {
					is_cpush(SRCH_MARK);
					success = TRUE;
				}
				mlwrite("Overwrapped I-search: %s", pat);
				break;
			}
			is_lpush();
			pptr = strlen(pat);
			(void)backchar(FFRAND, 1);
			if (is_find(SRCH_BACK) != FALSE)
				is_cpush(SRCH_MARK);
			else {
				(void)forwchar(FFRAND, 1);
				(*term.t_beep) ();
				success = FALSE;
			}
			is_prompt(dir, pptr < 0, success);
			break;
		case CTRL_H:
		case BACKSPACE:
			is_undo(&pptr, &dir);
			if (is_peek() != SRCH_ACCM)
				success = TRUE;
			is_prompt(dir, pptr < 0, success);
			break;
		default:
			if (ISCTRL(c)) {
			  /* ungetkey(c);  not required as we allow any CTRL key to exit isearch */
				curwp->w_markp = clp;
				curwp->w_marko = cbo;
				setmark(0,0);
				curwp->w_flag |= WFMOVE;
				return (TRUE);
			}
			/* FALLTHRU */
		case CTRL_I:
		  //addchar:
			if (pptr == -1)
				pptr = 0;
			if (pptr == 0)
				success = TRUE;
			if (pptr == NPAT - 1)
				(*term.t_beep) ();
			else {
				pat[pptr++] = c;
				pat[pptr] = '\0';
			}
			is_lpush();
			if (success != FALSE) {
				if (is_find(dir) != FALSE)
					is_cpush(c);
				else {
					success = FALSE;
					(*term.t_beep) ();
					is_cpush(SRCH_ACCM);
				}
			} else
				is_cpush(SRCH_ACCM);
			is_prompt(dir, FALSE, success);
		}
	}
	/* NOTREACHED */
}

static void
is_cpush(int cmd)
{
	if (++cip >= NSRCH)
		cip = 0;
	cmds[cip].s_code = cmd;
}

static void
is_lpush(void)
{
	int	ctp;

	ctp = cip + 1;
	if (ctp >= NSRCH)
		ctp = 0;
	cmds[ctp].s_code = SRCH_NOPR;
	cmds[ctp].s_doto = curwp->w_doto;
	cmds[ctp].s_dotp = curwp->w_dotp;
}

static void
is_pop(void)
{
	if (cmds[cip].s_code != SRCH_NOPR) {
		curwp->w_doto = cmds[cip].s_doto;
		curwp->w_dotp = cmds[cip].s_dotp;
		curwp->w_flag |= WFMOVE;
		cmds[cip].s_code = SRCH_NOPR;
	}
	if (--cip <= 0)
		cip = NSRCH - 1;
}

static int
is_peek(void)
{
	return (cmds[cip].s_code);
}

/* this used to always return TRUE (the return value was checked) */
static void
is_undo(int *pptr, int *dir)
{
	int	redo = FALSE;

	switch (cmds[cip].s_code) {
	case SRCH_BEGIN:
	case SRCH_NOPR:
		*pptr = -1;
		break;
	case SRCH_MARK:
		break;
	case SRCH_FORW:
		*dir = SRCH_BACK;
		redo = TRUE;
		break;
	case SRCH_BACK:
		*dir = SRCH_FORW;
		redo = TRUE;
		break;
	case SRCH_ACCM:
	default:
		*pptr -= 1;
		if (*pptr < 0)
			*pptr = 0;
		pat[*pptr] = '\0';
		break;
	}
	is_pop();
	if (redo)
		is_undo(pptr, dir);
}

static int
is_find(int dir)
{
  int	 plen, odoto;
	LINE	*odotp;

	odoto = curwp->w_doto;
	odotp = curwp->w_dotp;
	plen = strlen(pat);
	if (plen != 0) {
		if (dir == SRCH_FORW) {
			(void)backchar(FFARG | FFRAND, plen);
			if (forwsrch() == FALSE) {
				curwp->w_doto = odoto;
				curwp->w_dotp = odotp;
				return (FALSE);
			}
			return (TRUE);
		}
		if (dir == SRCH_BACK) {
			(void)forwchar(FFARG | FFRAND, plen);
			if (backsrch() == FALSE) {
				curwp->w_doto = odoto;
				curwp->w_dotp = odotp;
				return (FALSE);
			}
			return (TRUE);
		}
		mlwrite("bad call to is_find");
		return (FALSE);
	}
	return (FALSE);
}

/*
 * If called with "dir" not one of SRCH_FORW or SRCH_BACK, this routine used
 * to print an error message.  It also used to return TRUE or FALSE, depending
 * on if it liked the "dir".  However, none of the callers looked at the
 * status, so I just made the checking vanish.
 */
static void
is_prompt(int dir, int flag, int success)
{
	if (dir == SRCH_FORW) {
		if (success != FALSE)
			is_dspl("I-search", flag);
		else
			is_dspl("Failing I-search", flag);
	} else if (dir == SRCH_BACK) {
		if (success != FALSE)
			is_dspl("I-search backward", flag);
		else
			is_dspl("Failing I-search backward", flag);
	} else
		mlwrite("Broken call to is_prompt");
}

/*
 * Prompt writing routine for the incremental search.  The "prompt" is just
 * a string. The "flag" determines whether pat should be printed.
 */
static void
is_dspl(char *prompt, int flag)
{
  if (flag != FALSE) {
	mlwrite("%s: ", prompt);
  } else {
	mlwrite("%s: %s", prompt, pat);
  }
}

/*
 * qreplace: search for a string and replace it with another string. query
 * might be enabled.
 */
int qreplace (int f, int n)
{
  LINE *origline;		/* original "." position */
  char tmpc;			/* temporary character */
  char c;			/* input char for query */
  char tpat[NPAT];		/* temporary to hold search pattern */
  int i;			/* loop index */
  int s;			/* success flag on pattern inputs */
  int slength, rlength;		/* length of search and replace strings */
  int numsub;			/* number of substitutions */
  int nummatch;			/* number of found matches */
  int nlflag;			/* last char of search string a <NL>? */
  int nlrepl;			/* was a replace done on the last line? */
  int origoff;			/* and offset (for . query option) */
  int ask = TRUE;

  /* check for negative repititions */
  if (f && n < 0)
    return (FALSE);

  /* ask the user for the text of a pattern */
  if ((s = readpattern ("Query replace ")) != TRUE)
    return (s);
  strncpy (&tpat[0], &pat[0], NPAT); /* salt it away */

  /* ask for the replacement string */
  strncpy (&pat[0], &rpat[0], NPAT); /* set up default string */
  if ((s = readpattern ("with ")) == ABORT)
    return (s);

  /* move everything to the right place and length them */
  strncpy (&rpat[0], &pat[0], NPAT);
  strncpy (&pat[0], &tpat[0], NPAT);
  slength = strlen (&pat[0]);
  rlength = strlen (&rpat[0]);

  /* set up flags so we can make sure not to do a recursive replace on the
   * last line */
  nlflag = (pat[slength - 1] == '\n');
  nlrepl = FALSE;

  /* build query replace question string */
  sprintf(tpat, "Replace '%s' with '%s' ? ", pat, rpat);

  /* save original . position */
  origline = curwp->w_dotp;
  origoff = curwp->w_doto;

  /* scan through the file */
  numsub = 0;
  nummatch = 0;
  while ((f == FALSE || n > nummatch) &&
	 (nlflag == FALSE || nlrepl == FALSE))
    {
      /* search for the pattern */
      if (forscan (&pat[0], PTBEG) != TRUE)
	break;			/* all done */
      ++nummatch;		/* increment # of matches */

      /* check if we are on the last line */
      nlrepl = (lforw (curwp->w_dotp) == curwp->w_bufp->b_linep);

	  if (ask == TRUE) {

		/* get the query */
		mlwrite (tpat);
	  qprompt:
		update ();		/* show the proposed place to change */
		c = (*term.t_getchar) (); /* and input */
		mlwrite ("");		/* and clear it */

		/* and respond appropriately */
		switch (c)
		  {
		  case 'y':		/* yes, substitute */
		  case ' ':
			break;
			
		  case 'n':		/* no, onword */
			forwchar (FALSE, 1);
			continue;
			
		  case '!':		/* yes/stop asking */
			ask = FALSE;
			break;
			
		  case '.':		/* abort! and return */
			/* restore old position */
			curwp->w_dotp = origline;
			curwp->w_doto = origoff;
			curwp->w_flag |= WFMOVE;

		  case BELL:		/* abort! and stay */
			mlwrite ("Aborted!");
			return (FALSE);

		  case 0x0d:		/* controlled exit */
		  case 'q':
			return (TRUE);

		  default:		/* bitch and beep */
			(*term.t_beep) ();

		  case '?':		/* help me */
			mlwrite ("(Y)es, (N)o, (!)Do the rest, (^G)Abort, (?)Help: ");
			goto qprompt;
		  }
	  }
      /* delete the sucker */
      if (ldelete (slength, FALSE) != TRUE)
	{
	  /* error while deleting */
	  mlwrite ("ERROR while deleteing");
	  return (FALSE);
	}
      /* and insert its replacement */
      for (i = 0; i < rlength; i++)
	{
	  tmpc = rpat[i];
	  s = (tmpc == '\n' ? lnewline () : linsert (1, tmpc));
	  if (s != TRUE)
	    {
	      /* error while inserting */
	      mlwrite ("Out of memory while inserting");
	      return (FALSE);
	    }
	}

      numsub++;			/* increment # of substitutions */
    }

  /* and report the results */
  mlwrite ("%d substitutions", numsub);
  return (TRUE);
}

/*
 * This routine does the real work of a forward search.  The pattern is sitting
 * in the external variable "pat".  If found, dot is updated, the window system
 * is notified of the change, and TRUE is returned.  If the string isn't found,
 * FALSE is returned.
 */
int
forwsrch(void)
{
	LINE	*clp, *tlp;
	int	 cbo, tbo, c, i, xcase = 0;
	char	*pp;
	int	 nline;

	clp = curwp->w_dotp;
	cbo = curwp->w_doto;
	for (i = 0; pat[i]; i++)
		if (ISUPPER(CHARMASK(pat[i])))
			xcase = 1;
	for (;;) {
		if (cbo == llength(clp)) {
			if ((clp = lforw(clp)) == curbp->b_linep)
				break;
			nline++;
			cbo = 0;
			c = CCHR('J');
		} else
			c = lgetc(clp, cbo++);
		if (eq(c, pat[0], xcase) != FALSE) {
			tlp = clp;
			tbo = cbo;
			pp = &pat[1];
			while (*pp != 0) {
				if (tbo == llength(tlp)) {
					tlp = lforw(tlp);
					if (tlp == curbp->b_linep)
						goto fail;
					tbo = 0;
					c = CCHR('J');
					if (eq(c, *pp++, xcase) == FALSE)
						goto fail;
					nline++;
				} else {
					c = lgetc(tlp, tbo++);
					if (eq(c, *pp++, xcase) == FALSE)
						goto fail;
				}
			}
			curwp->w_dotp = tlp;
			curwp->w_doto = tbo;
			curwp->w_flag |= WFMOVE;
			return (TRUE);
		}
fail:		;
	}
	return (FALSE);
}

/*
 * This routine does the real work of a backward search.  The pattern is
 * sitting in the external variable "pat".  If found, dot is updated, the
 * window system is notified of the change, and TRUE is returned.  If the
 * string isn't found, FALSE is returned.
 */
int
backsrch(void)
{
	LINE	*clp, *tlp;
	int	 cbo, tbo, c, i, xcase = 0;
	char	*epp, *pp;
	int	 nline;

	for (epp = &pat[0]; epp[1] != 0; ++epp);
	clp = curwp->w_dotp;
	cbo = curwp->w_doto;
	for (i = 0; pat[i]; i++)
		if (ISUPPER(CHARMASK(pat[i])))
			xcase = 1;
	for (;;) {
		if (cbo == 0) {
			clp = lback(clp);
			if (clp == curbp->b_linep)
				return (FALSE);
			nline--;
			cbo = llength(clp) + 1;
		}
		if (--cbo == llength(clp))
			c = CCHR('J');
		else
			c = lgetc(clp, cbo);
		if (eq(c, *epp, xcase) != FALSE) {
			tlp = clp;
			tbo = cbo;
			pp = epp;
			while (pp != &pat[0]) {
				if (tbo == 0) {
					tlp = lback(tlp);
					if (tlp == curbp->b_linep)
						goto fail;
					nline--;
					tbo = llength(tlp) + 1;
				}
				if (--tbo == llength(tlp))
					c = CCHR('J');
				else
					c = lgetc(tlp, tbo);
				if (eq(c, *--pp, xcase) == FALSE)
					goto fail;
			}
			curwp->w_dotp = tlp;
			curwp->w_doto = tbo;
			curwp->w_flag |= WFMOVE;
			return (TRUE);
		}
fail:		;
	}
	/* NOTREACHED */
}

/*
 * Compare two characters.  The "bc" comes from the buffer.  It has its case
 * folded out. The "pc" is from the pattern.
 */
static int
eq(int bc, int pc, int xcase)
{
	bc = CHARMASK(bc);
	pc = CHARMASK(pc);
	if (bc == pc)
		return (TRUE);
	if (xcase)
		return (FALSE);
	if (ISUPPER(bc))
		return (TOLOWER(bc) == pc);
	if (ISUPPER(pc))
		return (bc == TOLOWER(pc));
	return (FALSE);
}

/*
 * Read a pattern.  Stash it in the external variable "pat".  The "pat" is not
 * updated if the user types in an empty line.  If the user typed an empty
 * line, and there is no old pattern, it is an error.  Display the old pattern,
 * in the style of Jeff Lomicka.  There is some do-it-yourself control
 * expansion.
 */
int
readpattern(char *prompt)
{
	char	tpat[NPAT];
	char 	nprompt[NPAT + 20]; // XXX
	int	retval;
	int r;

	if (pat[0] == '\0')
	  r = mlreplyt(prompt, tpat, NPAT, '\n'); /* Read pattern */
	else {
	  sprintf(nprompt, "%s: (default %s) ", prompt, pat);
	  r = mlreplyt(nprompt, tpat, NPAT, '\n'); /* Read pattern */
	}

	if (r == ABORT) {
		retval = ABORT;
		/* specified */
	} else if (r == TRUE) {
		(void)strlcpy(pat, tpat, NPAT);
		retval = TRUE;
		/*  cr but old one */
	} else if (r == FALSE && pat[0] != '\0') {
		retval = TRUE;
	} else
		retval = FALSE;
	return (retval);
}

/* search forward for a <patrn>
 */
int forscan (char *patrn, int leavep)
{
  LINE *curline;		/* current line during scan */
  LINE *lastline;		/* last line position during scan */
  LINE *matchline;		/* current line during matching */
  char *patptr;			/* pointer into pattern */
  int curoff;			/* position within current line */
  int lastoff;			/* position within last line */
  int c;			/* character at current position */
  int matchoff;			/* position in matching line */

  /* setup local scan pointers to global "." */
  curline = curwp->w_dotp;
  curoff = curwp->w_doto;

  /* scan each character until we hit the head link record */
  while (curline != curbp->b_linep)
    {
      /* save the current position in case we need to restore it on a match */
      lastline = curline;
      lastoff = curoff;

      /* get the current character resolving EOLs */
      if (curoff == llength (curline))
	{			/* if at EOL */
	  curline = lforw (curline); /* skip to next line */
	  curoff = 0;
	  c = '\n';		/* and return a <NL> */
	}
      else
	c = lgetc (curline, curoff++); /* get the char */

      /* test it against first char in pattern */
      if (eq (c, patrn[0], 0) != FALSE) /* if we find it. */
	{
	  /* setup match pointers */
	  matchline = curline;
	  matchoff = curoff;
	  patptr = &patrn[0];

	  /* scan through patrn for a match */
	  while (*++patptr != 0)
	    {
	      /* advance all the pointers */
	      if (matchoff == llength (matchline))
		{
		  /* advance past EOL */
		  matchline = lforw (matchline);
		  matchoff = 0;
		  c = '\n';
		}
	      else
		c = lgetc (matchline, matchoff++);

	      /* and test it against the pattern */
	      if (eq (*patptr, c, 0) == FALSE)
		goto fail;
	    }

	  /* A SUCCESSFULL MATCH!!! */
	  /* reset the global "." pointers */
	  if (leavep == PTEND)
	    {			/* at end of string */
	      curwp->w_dotp = matchline;
	      curwp->w_doto = matchoff;
	    }
	  else
	    {			/* at begining of string */
	      curwp->w_dotp = lastline;
	      curwp->w_doto = lastoff;
	    }
	  curwp->w_flag |= WFMOVE; /* flag that we have moved */
	  return (TRUE);
	}
    fail:;			/* continue to search */
    }
  /* we could not find a match */
  return (FALSE);
}
