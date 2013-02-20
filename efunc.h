/* EFUNC.H: function declarations and names
 *
 * This file list all the C code functions used. To add functions, declare it
 * here in both the extern function list and the name binding table
 */

extern int ctrlg ();	       /* Abort out of things */
extern int quit ();	       /* Quit */
extern int ctlxlp ();	       /* Begin macro */
extern int ctlxrp ();	       /* End macro */
extern int ctlxe ();	       /* Execute macro */
extern int viewfile ();	       /* Get a file, read only */
extern int filefind ();	       /* Get a file, read write */
extern int filewrite ();       /* Write a file */
extern int filesave ();	       /* Save current file */
extern int getccol ();	       /* Get current column */
extern int gotobol ();	       /* Move to start of line */
extern int forwchar ();	       /* Move forward by characters */
extern int gotoeol ();	       /* Move to end of line */
extern int backchar ();	       /* Move backward by characters */
extern int forwline ();	       /* Move forward by lines */
extern int backline ();	       /* Move backward by lines */
extern int forwpage ();	       /* Move forward by pages */
extern int backpage ();	       /* Move backward by pages */
extern int gotobob ();	       /* Move to start of buffer */
extern int gotoeob ();	       /* Move to end of buffer */
extern int setmark ();	       /* Set mark */
extern int forwsearch ();      /* Search forward */
extern int backsearch ();      /* Search backwards */
extern int forwisearch ();      /* iSearch forward */
extern int backisearch ();      /* iSearch backwards */
extern int sreplace ();	       /* search and replace */
extern int qreplace ();	       /* search and replace w/query */
extern int showcpos ();	       /* Show the cursor position */
extern int nextwind ();	       /* Move to the next window */
extern int prevwind ();	       /* Move to the previous window */
extern int onlywind ();	       /* Make current window only one */
extern int splitwind ();       /* Split current window */
extern int listbuffers ();     /* Display list of buffers */
extern int killbuffer ();      /* Make a buffer go away */
extern int refresh ();	       /* Refresh the screen */
extern int tab ();	       /* Insert tab */
extern int newline ();	       /* Insert CR-LF */
extern int quote ();	       /* Insert literal */
extern int forwdel ();	       /* Forward delete */
extern int backdel ();	       /* Backward delete */
extern int killtext ();	       /* Kill forward */
extern int yank ();	       /* Yank back from killbuffer */
extern int killregion ();      /* Kill region */
extern int copyregion ();      /* Copy region to kill buffer */
extern int gotoline ();	       /* go to a numbered line */
extern int nextbuffer ();      /* switch to the next buffer */
extern int extendedcmd ();     /* parse ANSI/VT100 extended keys  */
extern int togglereadonly ();     /* flip read only flag */
