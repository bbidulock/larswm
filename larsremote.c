/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>


Display *dpy;

Atom exit_larswm;
Atom restart_larswm;
Atom bartext_larswm;

void
usage (void)
{
  fprintf (stderr,
	   "usage: larsremote [-display display] <message text | exit | restart>\n");
  exit (1);
}

void
sendcmessage (Window w, Atom a, long x, int isroot)
{
  XEvent ev;
  long mask;

  memset (&ev, 0, sizeof (ev));

  ev.xclient.type = ClientMessage;
  ev.xclient.window = w;
  ev.xclient.message_type = a;
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = x;
  ev.xclient.data.l[1] = CurrentTime;
  mask = 0L;

  if (isroot)
    mask = SubstructureRedirectMask;	/* magic! */

  XSendEvent (dpy, w, False, mask, &ev);
}

int
main (int argc, char **argv)
{
  int i, do_exit, do_restart, do_message;
  char *display_string = "";
  char *msgtext;

  do_exit = do_restart = do_message = 0;

  for (i = 1; i < argc; i++)
    {
      if ((strcmp (argv[i], "-display") == 0) && (i + 1 < argc))
	{
	  display_string = argv[++i];
	}
      else if (strcmp (argv[i], "exit") == 0)
	{
	  do_exit++;
	}
      else if (strcmp (argv[i], "restart") == 0)
	{
	  do_restart++;
	}
      else if (strcmp (argv[i], "message") == 0)
	{
	  msgtext = argv[++i];
	  do_message++;
	}
      else
	{
	  usage ();
	}
    }

  if ((do_exit + do_restart + do_message) != 1)
    usage ();

  dpy = XOpenDisplay (display_string);

  if (dpy == 0)
    {
      perror ("XOpenDisplay");
      exit (1);
    }

  exit_larswm = XInternAtom (dpy, "LARSWM_EXIT", False);
  restart_larswm = XInternAtom (dpy, "LARSWM_RESTART", False);
  bartext_larswm = XInternAtom (dpy, "LARSWM_BARTEXT", False);

  if (do_exit)
    {
      sendcmessage (DefaultRootWindow (dpy), exit_larswm, 0L, 1);
    }
  else if (do_restart)
    {
      sendcmessage (DefaultRootWindow (dpy), restart_larswm, 0L, 1);
    }
  else if (do_message)
    {
      XTextProperty pr;

      XStringListToTextProperty (&msgtext, 1, &pr);
      XSetTextProperty (dpy, DefaultRootWindow (dpy), &pr, bartext_larswm);
      XFree (pr.value);
    }

  XSync (dpy, False);
  exit (0);
}
