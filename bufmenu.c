/*
 * Buffer Menu as per Big Emacs
 * Author: Hugh Barney, hughbarney@gmail.com, March 2013
 *
 * Diplay the buffer list in a window and operate on it
 * using the followinf keys
 *
 * N,n, CTRL+N, down-arrow:   move to next line
 * P,p, CTRL+P, up-arrow:     move to prev line
 * s  save buffer at line
 * v  toggle read only flag
 * k  kill buffer at line
 * 1  select buffer at line
 * 2  select buffer in split window below original window
 * q,Q,x,X exit buffer menu
 *
 */
#include <stdlib.h>
#include <string.h>
#include "estruct.h"
#include "edef.h"

extern int listbuffers(int f, int n);
extern int onlywind(int f, int n);
extern int splitwind(int f, int n);
extern int forwline(int f, int n);
extern int backline(int f, int n);
extern int nextwind(int f, int n);

extern int swbuffer (BUFFER *bp);
extern void update();
extern int ttgetc();
extern void mlwrite(char *,...);
extern void mlerase(void);
extern int zotbuf(BUFFER *);
extern BUFFER* get_scratch(void);
extern int getctl(void);
extern int filesave (int f, int n);

BUFFER *get_buffer(int );
int buffermenu(int f, int n);
int valid_buf(BUFFER*);
int count_buffers(void);


int buffermenu(int f, int n)
{
	BUFFER *bp;
	BUFFER *org_bp = curbp;
	int c,k;
	int bufptr;
	int bufcount = 0;

	bufptr = 1;

start:
	listbuffers(f,n);
	swbuffer(blistp);
	onlywind(0,0);
	bufcount = count_buffers();

	if (bufptr > bufcount)
		bufptr = bufcount;

	if (bufcount > 0)
		forwline(0, bufptr + 1);
	else
		forwline(0, 2);

	for (;;)
	{
		mlwrite("Buffer Menu: 1,2,s,v,k,q ");
		update();
		c = ttgetc();

		/* if no buffers, only allow exit */
		if (bufcount == 0)
		{
			switch (c)
			{
			case 'q': case 'Q': case 'x': case 'X':
				break;
			default:
				(*term.t_beep) ();
				continue;
			}
		}

		/*
		 * pre process escape sequence to get up/down arrows
		 * convert to CTRL+N, CTRL+P 
		 */
		if (c == ESC)
		{
			k = getctl();
			if (k == '[')
			{
				k = getctl();
				switch(k)
				{
				case 'A': c = CTRL_P; break;
				case 'B': c = CTRL_N; break;
				default: 
					(*term.t_beep)();
					continue;
				}
			}
			else
			{
				k = getctl();
				(*term.t_beep) ();
				continue;
			}
		}  /* if ESC */

		switch (c)
		{
		case 'n':
		case 'N':
		case ' ':
		case CTRL_N:
			if (bufcount == bufptr)
			{
				(*term.t_beep) ();
				break;
			}
			forwline(0,1);
			bufptr++;
			break;

		case 'p':
		case 'P':
		case CTRL_P:
		case BACKSPACE:
			if (bufptr == 1)
			{
				(*term.t_beep) ();
				break;
			}
			backline(0,1);
			bufptr--;
			break;

		case '1':
		case CTRL_M:
			bp = get_buffer(bufptr);
			swbuffer(bp);
			onlywind(0,0);
			mlerase();
			return TRUE;

		case '2':
			bp = get_buffer(bufptr);
			swbuffer(bp);
			onlywind(0,0);
			/* need to check or is still valid */
			if (valid_buf(org_bp) == TRUE && bufcount > 1)
			{
				splitwind(0,0);
				swbuffer(org_bp);
				nextwind(0,0);
			}
			mlerase();
			return TRUE;

			/* save file */
		case 's':
		case 'S':
			bp = get_buffer(bufptr);
			if (bp != NULL)
			{
				curbp = bp;
				(void)filesave(0,0);
				curbp = blistp;
				goto start;
			}
			break;

			/* toggle read only */
		case 'v':
		case 'V':
		case '%':
			bp = get_buffer(bufptr);
			if (bp != NULL)  /* be defensive */
				bp->b_flag ^= BFRO;
			goto start;
			break;

			/* kill buffer */
		case 'd':
		case 'D':
		case 'k':
		case 'K':
			bp = get_buffer(bufptr);
			if (bp != NULL)
				zotbuf(bp);
			goto start;
			break;

			/* exit buffer menu */
		case 'q':
		case 'Q':
		case 'x':
		case 'X':
			if (bufcount == 0)
			{
				bp = get_scratch();
				swbuffer(bp);
				onlywind(0,0);
				mlerase();
				return TRUE;
			}

			if (valid_buf(org_bp) == TRUE)
				swbuffer(org_bp);
			else
			{
				bp = get_scratch();
				swbuffer(bp);
			}
			onlywind(0,0);
			mlerase();
			return TRUE;

			/* any other key */
		default:
			(*term.t_beep) ();
			break;
		}
	}

	mlerase();
	return TRUE;
}

/*
 * return a the nth buffer in the list that has been walked through
 * by calling makelist.	 Will only be correct if makelist has been recently
 * been called and the list of buffers displayed.  We walk the list in the
 * same way as makelist.
 */
BUFFER *get_buffer(int n)
{
	BUFFER *bp;
	int i = 0;

	bp = bheadp;
	while (bp != NULL)
	{
		if ((bp->b_flag & BFTEMP) != 0)
		{
			bp = bp->b_bufp;
			continue;
		}
	 
		if (++i == n)
			return bp;

		bp = bp->b_bufp;
	}

	/* we should never get here */
	mlwrite("[Fatal: could not find buffer]");
	exit(1);
	return NULL;
}


/*
 * return the count of non internal buffers. Will only be correct if
 * makelist has been recently been called and the list of buffers
 * displayed.  We walk the list in the same way as makelist.
 */
int count_buffers(void)
{
	BUFFER *bp;
	int count = 0;

	bp = bheadp;
	while (bp != NULL)
	{
		if ((bp->b_flag & BFTEMP) != 0)
		{
			bp = bp->b_bufp;
			continue;
		}
		count++;
		bp = bp->b_bufp;
	}

	return count;
}

/*
 * check that the buffer is still in use and in the 
 * list of known buffers
 */
int valid_buf(BUFFER* bp_try)
{
	BUFFER *bp;

	bp = bheadp;
	while (bp != NULL)
	{
		if (bp == bp_try)
			return TRUE;  /* we found it */
		bp = bp->b_bufp;
	}

	/* if we get here the buffer must have been killed */
	return (FALSE);
}
