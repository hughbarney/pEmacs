              pEmacs or Perfect Emacs
              =======================

pEmacs is a small footprint Emacs derived from Chris Baird's Ersatz
Emacs which was based on MicroEMACS 3.6 as released to mod.sources
and the Public Domain by Daniel Lawrence in 1986, and was itself
based on the work of Steve Wilhite and George Jones to MicroEMACS 2.0
(then also public domain) by Dave Conroy.  I have included a copy of
the Ersatz Emacs README at the end of this file.

pEmacs came about as a response to the invitation set by Chris Baird
in his original README file.

   'I encourage people to personalise this very simple editor to their
   own requirements... Feel free to do a code-fork and distribute
   your own perfect text editor.'

The design goals of pEmacs are threefold.

   1. To replace the early MicroEMACS style search with incremental search
   2. To add filename completion as per MicroEMACS 3.10 or Mg Emacs
   3. To achieve further reduction in the code footprint.

So far goals 1,3 have been achieved, 2 is yet to be done. I see
pEmacs as an attempt to find the perfect balance between minimalism
(ie the smallest functional emacs and code) whilst still retaining
the majority feeling of Emacs. 

   'A designer knows he has achieved perfection not when there is
   nothing left to add, but when there is nothing left to take away'
            - Antoine de Saint-Exupery 

So far I have tested pEmacs on Ubuntu Linux but hope to follow up
on Arch Linux and Raspberry Pi at some point soon.


hughbarney@gmail.com February 2013
-----------------------------------


pEmacs V1.0,  February 2013
===========================

   1) In an effort to reduce the code count and allow for a small
number of enhancements I have removed what I never use and can easily
live without.  (See features removed).  Esratz-Emacs is 5777 lines of
code.  pEmacs 1.0 is 5286, this leaves me around 300 lines to
implement filename completion.

   2) Replaced searching with incremental searching C-S, C-R. Once
you have got used to incremental searching you can no longer cope
with the two stage searching of MicroEMACS style editors.  Control-S
prompts for a search string and searches as you type it, hit
Control-S in the prompt and it will search again, C-R will reverse
search.  Searching also overlaps once you get to the end of the
buffer.  ESC will exit 

   3) Implemented M-g gotoline. A basic gotline function was present
in the Ersatz-Emacs code but it was not bound to a key and did not ask
the user for a line number. I find this fairly essential when editing
source code following a set of compile errors.

   4) Improved showcpos (^X=) so that it shows line number as well as
buffer position.

   5) Added --version to the command line arguments so you can see
what version of pEmacs you are running.  This assumes there will be
at least one more version that will implement filename completion.

   6) Other Minor Keystroke Changes
^XN Also does next buffer
^C  Added: copy-region (same as M-W)
M-R query search and replace (same as M-%)


Comaprisons with Other MicroEMACS Implementations
==================================================

Editor         Binary   BinSize     KLOC  Files
-----------------------------------------------
pEmacs         pe         44824     5.2K     14
Esatz-Emacs    ee         59050     5.7K     14
uEmacs/Pk      em        147546    17.5K     34
ue3.10         uemacs    171664    52.4K     16
Mg             mg        585313    16.5K     44
Zile           zile     1130457    11.7K     32



Removed From Ersatz-Emacs
=========================

In an effort to reduce the code count and allow for a small number of
enhancements I have removed what I never use and can easily live without.

Bind  Function                       Reason
-------------------------------------------
^X B  Switch to another buffer       ^XX is faster, dont have to type buffer name
M-^V  Scroll other window down       never use
M-^Z  Scroll other window up         never use
^X ^  Enlarge current window         never use
^X N  Rename file in buffer          ^X^W is just as good
M-Q   Format paragraph               rarely use 
^X F  Set fill column                rarely use 
M-U   UPPERCASE word                 luxury
M-C   Capitalize word                luxury
M-L   lowercase word                 luxury
^T    Transpose characters           rarely use
M-<-  Delete previous word           luxury
M-F   Forward word                   never use
M-B   Backward word                  never use
M-D   Delete next word               luxury
^O    Open Line                      never use, CR is just as good
^U    Universal Argument             never use
M-Z   Quick Exit                     rarely use - ^X^C is good enough


pEmacs Key Bindings
-------------------
C-A   Begining-of-line
C-B   backward-character
C-C   copy-region
C-D   delete-char
C-E   End-of-line
C-F   Forward Character
C-G   Abort
C-H   Backspace
C-I   Tab
C-J   Newline
C-K   Kill to eol
C-L   refresh display
C-M   Carrage Return
C-N   next line
C-P   previous line
C-Q   Quote Chararcter
C-R   Incremenal Search Reverse 
C-S   Incremenal Search Forward
C-V   Page Down
C-W   Wipe region (Cut)
C-X   CTRL-X command prefix
C-Y   Yank

M-G   gotoline
M-R   query-replace (alias to M-%)
M-V   or [Page Up] Scroll up
M-W   Copy region to kill buffer.
M-%   query-replace
M-<   Start of file
M->   End of file

^@ or M-<spacebar> Set mark at current position.

^X^C  Exit. Any unsaved files will require confirmation.
^X^F  Find file; read into a new buffer created from filename.
^X^R  Read file into current buffer, erasing its previous contents.
^X^I  Insert file into current buffer at cursor's location.
^X^S  Save current buffer to disk, using the buffer's filename as the name of
^X^W  Write current buffer to disk. Type in a new filename at the prompt to

^X^B  Show buffer directory in a window (^X 1 to remove).
^XK   Delete a non-displayed buffer.
^XX   Switch to next buffer in buffer list.
^X2   Split the current window in two	
^X1   Show only current window
^XO   Move cursor to next window	
^X(   Start recording a keyboard macro. Typing ^G or an error aborts.
^X)   Stop recording macro.
^XE   Execute macro.
^X=   Position report; displays line number, buffer size, etc.


pEmacs 1.0 Desk Card (Feb 2013)
===============================
M-  means to use the <ESC> key prior to using another key
^A  means to use the control key at the same time as the 'A' key

------------------------------------------------------------------------------
			  MOVING THE CURSOR

^F   Forward character   	
^B   Backward character  	
^N   Next line				M-P    Front of paragraph
^P   Previous line			M-N    End of paragraph
^A   Front of line			M-< or [HOME] Start of file
^E   End of line			M-> or [END] End of file
^V or [Page Down] Scroll down
M-V or [Page Up] Scroll up		Arrow keys are active

------------------------------------------------------------------------------
			 DELETING & INSERTING

<-   Delete previous character	
^D   Delete next character		
^K   Delete to end of line		

------------------------------------------------------------------------------
		       FORMATTING & TRANSPOSING

^Q   Quote next key, so that control codes may be entered into text.

------------------------------------------------------------------------------
			      SEARCHING

^S   Incremental Search forward, Esc to exit, ^G to abort, ^S search next
^R   As above, but reverse search from cursor position.

------------------------------------------------------------------------------
			      REPLACING

M-%  Replace with query. Answer with:
	Y   replace & continue		N   no replacement & continue
	!   replace the rest		?   Get a list of options
	.   exit and return to entry point
	^G,'q' or <return> exit and remain at current location

------------------------------------------------------------------------------
			  COPYING AND MOVING

^@ or M-<spacebar> Set mark at current position.
^W   Delete region.
M-W  Copy region to kill buffer.
^Y   Yank back kill buffer at cursor.

A region is defined as the area between this mark and the current cursor
position. The kill buffer is the text which has been most recently deleted or
copied.

Generally, the procedure for copying or moving text is:
1) Mark out region using M-<spacebar> at the beginning and move the cursor to
   the end.
2) Delete it (with ^W) or copy it (with M-W) into the kill buffer.
3) Move the cursor to the desired location and yank it back (with ^Y).

------------------------------------------------------------------------------
			   MULTIPLE BUFFERS

A buffer contains a COPY of a document being edited, and must be saved for
changes to be kept. Many buffers may be activated at once.

^X^B  Show buffer directory in a window (^X 1 to remove).
^X K  Delete a non-displayed buffer.
^X X  Switch to next buffer in buffer list.

------------------------------------------------------------------------------
			  READING FROM DISK

^X^F  Find file; read into a new buffer created from filename.
      (This is the usual way to edit a new file.)
^X^R  Read file into current buffer, erasing its previous contents.
      No new buffer will be created.
^X^I  Insert file into current buffer at cursor's location.

------------------------------------------------------------------------------
			    SAVING TO DISK

^X^S  Save current buffer to disk, using the buffer's filename as the name of
      the disk file. Any disk file of that name will be overwritten.
^X^W  Write current buffer to disk. Type in a new filename at the prompt to
      write to; it will also become the current buffer's filename.

------------------------------------------------------------------------------
			   MULTIPLE WINDOWS

Many windows may be visible at once on the screen. Windows may show different
parts of the same buffer, or each may display a different one.

^X 2  Split the current window in two	^X 1   Show only current window
^X O  Move cursor to next window	

------------------------------------------------------------------------------
			       EXITING

^X^C  Exit. Any unsaved files will require confirmation.

------------------------------------------------------------------------------
				MACROS

^X (  Start recording a keyboard macro. Typing ^G or an error aborts.
^X )  Stop recording macro.
^X E  Execute macro.
------------------------------------------------------------------------------
			     SPECIAL KEYS

^G   Cancel current command.
^L   Redraws the screen completely.
^X = Position report; displays line number, buffer size, etc.

------------------------------------------------------------------------------


Ersatz-Emacs Original README
============================
This shar file contains the source to a microemacs-derived text editor
that I have been personally hacking on for over a decade.

Originally this was MicroEMACS 3.6 as released to mod.sources and the
Public Domain by Daniel Lawrence in 1986, and was itself based on the
work of Steve Wilhite and George Jones to MicroEMACS 2.0 (then also
public domain) by Dave Conroy. I would like to reiterate Lawrence's
thanks to them for writing such nice, well structured and documented
code.

"Ersatz-Emacs", as I call it today, is the above text editor throughly
cleansed of routines and features that I personally never use. It is
also an editor MINIX-creator Andy Tanenbaum could describe as "fitting
inside a student's brain" (namely, mine).

This source code should compile cleanly on any "modern" UN*X system
with a termcap/curses library. This release has been tested with
NetBSD and various Linux systems, although in the past when it was
still mostly MicroEMACS, proto-Ersatz-Emacs was an editor of choice on
SunOS, Solaris, Xenix, Minix/i386, and AIX. Supporting these and
similar systems should not be difficult.

I encourage people to personalise this very simple editor to their own
requirements. Please send any useful bug reports and fixes back to me,
but I'm not really interested in incorporating new features unless it
simplifies the program further. Feel free to do a code-fork and
distribute your own perfect text editor.

The title "Ersatz" comes from the category Richard Stallman uses in
MIT AI Memo 519a to describe those editors that are a surface-deep
imitation (key bindings) of "real" ITS Emacs. If you are familiar with
any Emacs-variant editor, you should have few problems with Ersatz.

All source code of this program is in the Public Domain. I am a rabid
Stallmanite weenie, but it would be improper to publish this under a
different licence than it was given to me with.

-- 
Chris Baird,, <cjb@brushtail.apana.org.au>
