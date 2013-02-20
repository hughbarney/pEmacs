/*
 * This table is *roughly* in ASCII order, left to right across the
 * characters of the command. This expains the funny location of the
 * control-X commands.
 */

KEYTAB keytab[] = {
  {CTRL | '@', setmark, 0},
  {CTRL | 'A', gotobol, 0},
  {CTRL | 'B', backchar, 0},
  {CTRL | 'C', copyregion, 0},  /* non standard */
  {CTRL | 'D', forwdel, BFRO},
  {CTRL | 'E', gotoeol, 0},
  {CTRL | 'F', forwchar, 0},
  {CTRL | 'G', ctrlg, 0},
  {CTRL | 'H', backdel, BFRO},
  {CTRL | 'I', tab, BFRO},
  {CTRL | 'K', killtext, BFRO},
  {CTRL | 'L', refresh, 0},
  {CTRL | 'M', newline, BFRO},
  {CTRL | 'N', forwline, 0},
  {CTRL | 'P', backline, 0},
  {CTRL | 'Q', quote, BFRO},
  {CTRL | 'R', backisearch, 0},
  {CTRL | 'S', forwisearch, 0},
  {CTRL | 'V', forwpage, 0},
  {CTRL | 'W', killregion, BFRO},
  {CTRL | 'Y', yank, BFRO},
  {CTLX | '(', ctlxlp, 0},
  {CTLX | ')', ctlxrp, 0},
  {CTLX | '1', onlywind, 0},
  {CTLX | '2', splitwind, 0},
  {CTLX | '=', showcpos, 0},
  {CTLX | 'E', ctlxe, 0},
  {CTLX | 'K', killbuffer, 0},
  {CTLX | 'N', nextbuffer, 0},
  {CTLX | 'O', nextwind, 0},
  {CTLX | 'X', nextbuffer, 0},
  {CTLX | CTRL | 'B', listbuffers, 0},
  {CTLX | CTRL | 'C', quit, 0},
  {CTLX | CTRL | 'F', filefind, 0},
  {CTLX | CTRL | 'S', filesave, 0},
  {CTLX | CTRL | 'V', viewfile, 0},
  {CTLX | CTRL | 'W', filewrite, 0},
  {CTLX | CTRL | 'Q', togglereadonly, 0},
  {META | ' ', setmark, 0},
  {META | '%', qreplace, BFRO},
  {META | '.', setmark, 0},
  {META | '<', gotobob, 0},
  {META | '>', gotoeob, 0},
  {META | 'G', gotoline, 0},		/* non-standard */
  {META | 'R', qreplace, 0},
  {META | 'V', backpage, 0},
  {META | 'W', copyregion, 0},
  {0x7F, backdel, 0},
  {META | '[', extendedcmd, 0},
  {META | 'O', extendedcmd, 0},
  {0, 0, 0}
};
