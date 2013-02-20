/* ESTRUCT: Structure and preprocesser */

/* internal constants */
#define NFILEN	80		/* maximum # of bytes, file name */
#define NBUFN	16		/* maximum # of bytes, buffer name */
#define NLINE	512		/* maximum # of bytes, line */
#define NKBDM	256		/* maximum # of strokes, keyboard macro */
#define NPAT	80		/* maximum # of bytes, pattern */
#define HUGE	32700		/* Huge number for "impossible" row&col */

#define METACH	0x1B		/* M- prefix, Control-[, ESC */
#define BELL	0x07		/* a bell character */
#define TAB	0x09		/* a tab character */

#define CTRL	0x0100		/* Control flag, or'ed in */
#define META	0x0200		/* Meta flag, or'ed in */
#define CTLX	0x0400		/* ^X flag, or'ed in */

#define FALSE	0		/* False, no, bad, etc */
#define TRUE	1		/* True, yes, good, etc */
#define ABORT	2		/* Death, ^G, abort, etc */

#define FIOSUC	0		/* File I/O, success */
#define FIOFNF	1		/* File I/O, file not found */
#define FIOEOF	2		/* File I/O, end of file */
#define FIOERR	3		/* File I/O, error */
#define FIOLNG	4		/* line longer than allowed len */

#define	PTBEG	1		/* leave the point at the begining on search */
#define	PTEND	2		/* leave the point at the end on search */

#define SRCH_BEGIN	(0)	/* Search sub-codes.	 */
#define SRCH_FORW	(-1)
#define SRCH_BACK	(-2)
#define SRCH_NOPR	(-3)
#define SRCH_ACCM	(-4)
#define SRCH_MARK	(-5)

#define ESC    27
#define CTRL_G 7
#define CTRL_H 8
#define CTRL_I 9
#define CTRL_M 13
#define CTRL_S 19
#define CTRL_R 18
#define CTRL_Q 17
#define BACKSPACE 127

#define NSRCH	  128	/* Undoable search commands.	 */
#define FFRAND		8	/* Called by other function	 */
#define FFARG		7	/* any argument			 */

#define ISLETTER(c)	(('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c) || (128<=c && c<=167))
#define ISLOWER(c)	(('a' <= c && 'z' >= c))
#define ISUPPER(c)	(('A' <= c && 'Z' >= c))
#define ISDIGIT(c)       (((c) >= '0') && ((c) <= '9'))
#define ISCTRL(c)	     (c >= 0 && c <= 31)
#define TOUPPER(c)	((c)-0x20)
#define TOLOWER(c)	((c)+0x20)
#define	CHARMASK(c)	((unsigned char) (c))
#define CCHR(x)		((x) ^ 0x40)	/* CCHR('?') == DEL */

#define CFCPCN	0x0001		/* Last command was C-P, C-N */
#define CFKILL	0x0002		/* Last command was a kill */

/*
 * There is a window structure allocated for every active display window. The
 * windows are kept in a big list, in top to bottom screen order, with the
 * listhead at "wheadp". Each window contains its own values of dot and mark.
 * The flag field contains some bits that are set by commands to guide
 * redisplay; although this is a bit of a compromise in terms of decoupling,
 * the full blown redisplay is just too expensive to run for every input
 * character
 */
typedef struct WINDOW
{
  struct WINDOW *w_wndp;	/* Next window */
  struct BUFFER *w_bufp;	/* Buffer displayed in window */
  struct LINE *w_linep;		/* Top line in the window */
  struct LINE *w_dotp;		/* Line containing "." */
  long w_doto;			/* Byte offset for "." */
  struct LINE *w_markp;		/* Line containing "mark" */
  long w_marko;			/* Byte offset for "mark" */
  char w_toprow;		/* Origin 0 top row of window */
  char w_ntrows;		/* # of rows of text in window */
  char w_force;			/* If NZ, forcing row */
  char w_flag;			/* Flags */
} WINDOW;

#define WFFORCE 0x01	       /* Window needs forced reframe */
#define WFMOVE	0x02	       /* Movement from line to line */
#define WFEDIT	0x04	       /* Editing within a line */
#define WFHARD	0x08	       /* Better to a full display */
#define WFMODE	0x10	       /* Update mode line */

/*
 * Text is kept in buffers. A buffer header, described below, exists for every
 * buffer in the system. The buffers are kept in a big list, so that commands
 * that search for a buffer by name can find the buffer header. There is a
 * safe store for the dot and mark in the header, but this is only valid if
 * the buffer is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with a pointer to
 * the header line in "b_linep". Buffers may be "Inactive" which means the
 * files accosiated with them have not been read in yet. These get read in at
 * "use buffer" time
 */
typedef struct BUFFER
{
  struct BUFFER *b_bufp;	/* Link to next BUFFER */
  struct LINE *b_dotp;		/* Link to "." LINE structure */
  long b_doto;			/* Offset of "." in above LINE */
  struct LINE *b_markp;		/* The same as the above two, */
  long b_marko;			/* but for the "mark" */
  struct LINE *b_linep;		/* Link to the header LINE */
  char b_active;		/* window activated flag */
  char b_nwnd;			/* Count of windows on buffer */
  char b_flag;			/* Flags */
  char b_fname[NFILEN];		/* File name */
  char b_bname[NBUFN];		/* Buffer name */
} BUFFER;

#define BFTEMP	0x01		/* Internal temporary buffer */
#define BFCHG	0x02		/* Changed since last write */
#define BFRO	0x04		/* Read Only Buffer */

#define bfirstlp(buf)	(lforw((buf)->b_linep))
#define blastlp(buf)	(lback((buf)->b_linep))

/*
 * The starting position of a region, and the size of the region in
 * characters, is kept in a region structure.  Used by the region commands
 */
typedef struct
{
  struct LINE *r_linep;		/* Origin LINE address */
  long r_offset;		/* Origin LINE offset */
  long r_size;			/* Length in characters */
} REGION;

/*
 * All text is kept in circularly linked lists of "LINE" structures. These
 * begin at the header line (which is the blank line beyond the end of the
 * buffer). This line is pointed to by the "BUFFER". Each line contains a the
 * number of bytes in the line (the "used" size), the size of the text array,
 * and the text. The end of line is not stored as a byte; it's implied. Future
 * additions will include update hints, and a list of marks into the line
 */
typedef struct LINE
{
  struct LINE *l_fp;		/* Link to the next line */
  struct LINE *l_bp;		/* Link to the previous line */
  int l_size;			/* Allocated size */
  int l_used;			/* Used size */
  char l_text[1];		/* A bunch of characters */
} LINE;

#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define lgetc(lp, n)	((lp)->l_text[(n)]&0xFF)
#define lputc(lp, n, c) ((lp)->l_text[(n)]=(c))
#define llength(lp)	((lp)->l_used)
#define lused(lp)	((lp)->l_used)
#define lsize(lp)	((lp)->l_size)
#define ltext(lp)	((lp)->l_text)

/*
 * The editor communicates with the display using a high level interface. A
 * "TERM" structure holds useful variables, and indirect pointers to routines
 * that do useful operations. The low level get and put routines are here too.
 * This lets a terminal, in addition to having non standard commands, have
 * funny get and put character code too. The calls might get changed to
 * "termp->t_field" style in the future, to make it possible to run more than
 * one terminal type
 */
typedef struct
{
  int t_nrow;			/* Number of rows */
  int t_ncol;			/* Number of columns */
  int t_margin;			/* min margin for extended lines */
  int t_scrsiz;			/* size of scroll region " */
  void (*t_open) ();		/* Open terminal at the start */
  void (*t_close) ();		/* Close terminal at end */
  int (*t_getchar) ();		/* Get character from keyboard */
  void (*t_putchar) ();		/* Put character to display */
  void (*t_flush) ();		/* Flush output buffers */
  void (*t_move) ();		/* Move the cursor, origin 0 */
  void (*t_eeol) ();		/* Erase to end of line */
  void (*t_eeop) ();		/* Erase to end of page */
  void (*t_beep) ();		/* Beep */
  void (*t_rev) ();		/* set reverse video state */
} TERM;

/* structure for the table of initial key bindings */

typedef struct
{
  short k_code;			/* Key code */
  int (*k_fp) ();		/* Routine to handle it */
  int k_flag;           /* data about the capabilities */
} KEYTAB;
