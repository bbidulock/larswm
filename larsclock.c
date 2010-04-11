/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */

/*
 * Minimal clock app for larswm. No fork()/exec() is done, it updates
 * the status bar by setting the LARSWM_BARTEXT atom every second.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#ifdef USE_LOCALE
#include <locale.h>
#endif

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#define CLOCK_LENGTH 100

int
main (int argc, char **argv)
{
  int i;
  int update = 0;
  time_t t;
  char *slist;
  char *format = "%c";
  char *display_string = "";
  Display *dpy;
  Atom bartext_larswm;
  XTextProperty pr;

#ifdef USE_LOCALE
  setlocale (LC_ALL, "");
#endif

  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-display") == 0 && i + 1 < argc)
	{
	  display_string = argv[++i];
	}
      else if (strcmp (argv[i], "-format") == 0 && i + 1 < argc)
	{
	  format = argv[++i];
	}
      else if (strcmp (argv[i], "-update") == 0 && i + 1 < argc)
	{
	  update = atoi(argv[++i]);
	}
      else
	{
	  fprintf (stderr, "usage: %s [-display display] [-format format] [-update seconds]\n",
		   argv[0]);
	  exit (1);
	}
    }

  dpy = XOpenDisplay (display_string);

  if (dpy == 0)
    {
      perror ("XOpenDisplay");
      exit (1);
    }

  if (update == 0)
    {
      update = 1;
    }
  
  bartext_larswm = XInternAtom (dpy, "LARSWM_BARTEXT", False);
  slist = malloc (CLOCK_LENGTH);

  while (1)
    {
      time (&t);
      strftime (slist, CLOCK_LENGTH, format, localtime (&t));

      XStringListToTextProperty (&slist, 1, &pr);
      XSetTextProperty (dpy, DefaultRootWindow (dpy), &pr, bartext_larswm);
      XFree (pr.value);
      XSync (dpy, False);

      sleep (update);
    }
}
