/*
 * This table is *roughly* in ASCII order, left to right across the
 * characters of the command. This expains the funny location of the
 * control-X commands.
 */

KEYTAB keytab[] = {
  {CTRL | '@', setmark},
  {CTRL | 'A', gotobol},
  {CTRL | 'B', backchar},
  {CTRL | 'C', copyregion},  /* non standard */
  {CTRL | 'D', forwdel},
  {CTRL | 'E', gotoeol},
  {CTRL | 'F', forwchar},
  {CTRL | 'G', ctrlg},
  {CTRL | 'H', backdel},
  {CTRL | 'I', tab},
  {CTRL | 'K', killtext},
  {CTRL | 'L', refresh},
  {CTRL | 'M', newline},
  {CTRL | 'N', forwline},
  {CTRL | 'P', backline},
  {CTRL | 'Q', quote},
  {CTRL | 'R', backisearch},
  {CTRL | 'S', forwisearch},
  {CTRL | 'V', forwpage},
  {CTRL | 'W', killregion},
  {CTRL | 'Y', yank},
  {CTLX | '(', ctlxlp},
  {CTLX | ')', ctlxrp},
  {CTLX | '1', onlywind},
  {CTLX | '2', splitwind},
  {CTLX | '=', showcpos},
  {CTLX | 'E', ctlxe},
  {CTLX | 'K', killbuffer},
  {CTLX | 'N', nextbuffer},
  {CTLX | 'O', nextwind},
  {CTLX | 'X', nextbuffer},
  {CTLX | CTRL | 'B', listbuffers},
  {CTLX | CTRL | 'C', quit},
  {CTLX | CTRL | 'F', filefind},
  {CTLX | CTRL | 'I', insfile},
  {CTLX | CTRL | 'R', fileread},
  {CTLX | CTRL | 'S', filesave},
  {CTLX | CTRL | 'W', filewrite},
  {META | ' ', setmark},
  {META | '%', qreplace},
  {META | '.', setmark},
  {META | '<', gotobob},
  {META | '>', gotoeob},
  {META | 'G', gotoline},		/* non-standard */
  {META | 'R', qreplace},
  {META | 'V', backpage},
  {META | 'W', copyregion},
  {0x7F, backdel},
  {META | '[', extendedcmd},
  {META | 'O', extendedcmd},
  {0, 0}
};
