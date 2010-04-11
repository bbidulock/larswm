/* Copyright (c) 1994-1996 David Hogan, see README for licence details */
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

int ignore_badwindow;

void
fatal (char *s)
{
  fprintf (stderr, "larswm: ");
  perror (s);
  fprintf (stderr, "\n");
  exit (1);
}

int
handler (Display * d, XErrorEvent * e)
{
  char msg[80], req[80], number[80];

  if (initting && (e->request_code == X_ChangeWindowAttributes)
      && (e->error_code == BadAccess))
    {
      fprintf (stderr, "larswm: another wm already running?\n");
      exit (1);
    }

  if (ignore_badwindow
      && (e->error_code == BadWindow || e->error_code == BadColor))
    return 0;

  XGetErrorText (d, e->error_code, msg, sizeof (msg));
  sprintf (number, "%d", e->request_code);
  XGetErrorDatabaseText (d, "XRequest", number, "", req, sizeof (req));
  if (req[0] == '\0')
    sprintf (req, "<request-code-%d>", e->request_code);

  fprintf (stderr, "larswm: %s(0x%x): %s\n", req,
	   (unsigned int) e->resourceid, msg);

  if (initting)
    {
      fprintf (stderr, "larswm: failure during initialisation; aborting\n");
      exit (1);
    }
  return 0;
}
