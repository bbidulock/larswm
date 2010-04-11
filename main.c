/* Copyright (c) 1994-1996 David Hogan, see README for licence details */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/cursorfont.h>

#include "dat.h"
#include "fns.h"

Display *dpy;
ScreenInfo *screens;
int initting;
XFontStruct *font;
char **myargv;
char *shell;
Bool shape;
int num_screens;

Atom exit_larswm;
Atom restart_larswm;
Atom bartext_larswm;

Atom wm_state;
Atom wm_change_state;
Atom wm_protocols;
Atom wm_delete;
Atom wm_colormaps;

void
usage (void)
{
  fprintf (stderr, "%s\n", VERSION);
  fprintf (stderr,
	   "usage: larswm [-display display] [-f file] [-defaults] [-v]\n");
  exit (1);
}

int
main (int argc, char **argv)
{
  int i;
  int shape_event, dummy;
  char rcname[512];
  char *display_string = "";
  char *rc_file;

  rc_file = getenv ("HOME");

  if (rc_file)
    {
      strncpy (rcname, rc_file, sizeof (rcname) - 11);
      strcat (rcname, "/.larswmrc");
      rc_file = rcname;
    }

  myargv = argv;
  font = 0;

  for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-f") == 0 && i + 1 < argc)
	{
	  rc_file = argv[++i];
	}
      else if (strcmp (argv[i], "-display") == 0 && i + 1 < argc)
	{
	  display_string = argv[++i];
	}
      else if (strcmp (argv[i], "-defaults") == 0)
	{
	  set_defaults ();
	  dump_prefs ();
	  exit (0);
	}
      else if (strcmp (argv[i], "-v") == 0)
	{
	  fprintf (stderr, "%s\n", VERSION);
	  exit (0);
	}
      else
	{
	  usage ();
	}
    }

  shell = (char *) getenv ("SHELL");

  if (shell == NULL)
    shell = DEFSHELL;

  dpy = XOpenDisplay (display_string);

  if (dpy == 0)
    fatal ("can not open display");

  initting = 1;

  XSetErrorHandler (handler);

  exit_larswm = XInternAtom (dpy, "LARSWM_EXIT", False);
  restart_larswm = XInternAtom (dpy, "LARSWM_RESTART", False);
  bartext_larswm = XInternAtom (dpy, "LARSWM_BARTEXT", False);

  wm_state = XInternAtom (dpy, "WM_STATE", False);
  wm_change_state = XInternAtom (dpy, "WM_CHANGE_STATE", False);
  wm_protocols = XInternAtom (dpy, "WM_PROTOCOLS", False);
  wm_delete = XInternAtom (dpy, "WM_DELETE_WINDOW", False);
  wm_colormaps = XInternAtom (dpy, "WM_COLORMAP_WINDOWS", False);

  XrmInitialize ();
  load_prefs (rc_file);

  if (prefs.fname != 0)
    {
      if ((font = XLoadQueryFont (dpy, prefs.fname)) == 0)
	fprintf (stderr, "larswm: warning: can't load font %s\n",
		 prefs.fname);
    }

  if (font == 0)
    {
      prefs.fname = "fixed";

      if ((font = XLoadQueryFont (dpy, prefs.fname)) == 0)
	fatal ("can not find fixed font");
    }

  shape = XShapeQueryExtension (dpy, &shape_event, &dummy);
  num_screens = ScreenCount (dpy);

  if (num_screens > MAXSCREENS)
    {
      fprintf (stderr, "larswm was compiled with support for %d screens.\n",
	       MAXSCREENS);
      fprintf (stderr,
	       "please recompile with MAXSCREENS set to at least %d screens.\n",
	       num_screens);
      exit (1);
    }

  screens = (ScreenInfo *) malloc (sizeof (ScreenInfo) * num_screens);

  for (i = 0; i < num_screens; i++)
    initscreen (&screens[i], i);

  XSync (dpy, False);
  initting = 0;

  nofocus ();

  for (i = 0; i < num_screens; i++)
    scanwins (&screens[i]);

  initkeys (0);

  XSync (dpy, False);
  mainloop (shape_event);

  exit (0);
}

void
initscreen (ScreenInfo * s, int i)
{
  char *ds, *colon, *dot1;
  unsigned long mask;
  XGCValues gv;
  int t;

  s->num = i;
  s->root = RootWindow (dpy, i);
  s->def_cmap = DefaultColormap (dpy, i);
  s->min_cmaps = MinCmapsOfScreen (ScreenOfDisplay (dpy, i));
  s->tile_height = s->res_height = DisplayHeight (dpy, s->num);

  ds = DisplayString (dpy);

  strcpy (s->display, "DISPLAY=");
  strcat (s->display, ds);

  colon = rindex (ds, ':');

  if (colon && num_screens > 1)
    {
      colon = s->display + 8 + (colon - ds);
      dot1 = index (colon, '.');

      if (!dot1)
	dot1 = colon + strlen (colon);

      sprintf (dot1, ".%d", i);
    }

  /* hey, this is experimental software, it doesn't have to look pretty just yet. :) */
  if (prefs.fgstr)
    {
      XColor color;

      if (XAllocNamedColor
	  (dpy, DefaultColormap (dpy, s->num), prefs.fgstr, &color,
	   &color) == 0)
	{
	  fprintf (stderr,
		   "XAllocNamedColor - allocation of fg color failed.\n");
	  s->black = BlackPixel (dpy, i);
	}
      else
	{
	  s->black = color.pixel;
	}
    }
  else
    {
      s->black = BlackPixel (dpy, i);
    }

  if (prefs.bgstr)
    {
      XColor color;

      if (XAllocNamedColor
	  (dpy, DefaultColormap (dpy, s->num), prefs.bgstr, &color,
	   &color) == 0)
	{
	  fprintf (stderr,
		   "XAllocNamedColor - allocation of bg color failed.\n");
	  s->white = WhitePixel (dpy, i);
	}
      else
	{
	  s->white = color.pixel;
	}
    }
  else
    {
      s->white = WhitePixel (dpy, i);
    }

#ifdef THREE_D
  {
    XColor topcolor;
    XColor botcolor;
    XColor get_c;

    get_c.pixel = s->white;

    if ((get_c.pixel == WhitePixel (dpy, s->num)) ||
	(get_c.pixel == BlackPixel (dpy, s->num)))
      {
        topcolor.red = L_TOP (L_OFF_R);
        topcolor.green = L_TOP (L_OFF_G);
        topcolor.blue = L_TOP (L_OFF_B);
	
        botcolor.red = L_BOT (L_OFF_R);
        botcolor.green = L_BOT (L_OFF_G);
        botcolor.blue = L_BOT (L_OFF_B);
      }
    else
      {
	XQueryColor (dpy, s->def_cmap, &get_c);

	topcolor.red = L_TOP (get_c.red);
	topcolor.green = L_TOP (get_c.green);
	topcolor.blue = L_TOP (get_c.blue);

	botcolor.red = L_BOT (get_c.red);
	botcolor.green = L_BOT (get_c.green);
	botcolor.blue = L_BOT (get_c.blue);
      }

    XAllocColor (dpy, s->def_cmap, &topcolor);
    XAllocColor (dpy, s->def_cmap, &botcolor);

    s->topwhite = topcolor.pixel;
    s->botwhite = botcolor.pixel;

    gv.foreground = s->topwhite;
    gv.line_width = 0;
    mask = GCForeground | GCLineWidth;
    s->topwhitegc = XCreateGC (dpy, s->root, mask, &gv);

    gv.foreground = s->botwhite;
    gv.line_width = 0;
    mask = GCForeground | GCLineWidth;
    s->botwhitegc = XCreateGC (dpy, s->root, mask, &gv);

    get_c.pixel = s->black;

    if ((get_c.pixel == WhitePixel (dpy, s->num)) ||
	(get_c.pixel == BlackPixel (dpy, s->num)))
      {
        topcolor.red = L_TOP (L_ON_R);
        topcolor.green = L_TOP (L_ON_G);
        topcolor.blue = L_TOP (L_ON_B);

	botcolor.red = L_BOT (L_ON_R);
        botcolor.green = L_BOT (L_ON_G);
        botcolor.blue = L_BOT (L_ON_B);
      }
    else
      {
	XQueryColor (dpy, s->def_cmap, &get_c);
	
	topcolor.red = L_TOP (get_c.red);
	topcolor.green = L_TOP (get_c.green);
	topcolor.blue = L_TOP (get_c.blue);
	
	botcolor.red = L_BOT (get_c.red);
	botcolor.green = L_BOT (get_c.green);
	botcolor.blue = L_BOT (get_c.blue);
      }
    
    XAllocColor (dpy, s->def_cmap, &topcolor);
    XAllocColor (dpy, s->def_cmap, &botcolor);

    s->topblack = topcolor.pixel;
    s->botblack = botcolor.pixel;

    gv.foreground = s->topblack;
    gv.line_width = 0;
    mask = GCForeground | GCLineWidth;
    s->topblackgc = XCreateGC (dpy, s->root, mask, &gv);

    gv.foreground = s->botblack;
    gv.line_width = 0;
    mask = GCForeground | GCLineWidth;
    s->botblackgc = XCreateGC (dpy, s->root, mask, &gv);
  }
#endif

  gv.foreground = s->black ^ s->white;
  gv.background = s->white;
  gv.function = GXxor;
  gv.line_width = 0;
  gv.subwindow_mode = IncludeInferiors;
  mask = GCForeground | GCBackground | GCFunction | GCLineWidth
    | GCSubwindowMode;

  if (font != 0)
    {
      gv.font = font->fid;
      mask |= GCFont;
    }

  s->gc = XCreateGC (dpy, s->root, mask, &gv);
  s->place = XCreateFontCursor (dpy, XC_top_left_corner);
  s->sweep = XCreateFontCursor (dpy, XC_bottom_right_corner);

  XSelectInput (dpy, s->root,
		SubstructureRedirectMask | SubstructureNotifyMask |
		ColormapChangeMask | PropertyChangeMask | ButtonPressMask |
		ButtonReleaseMask);

  XSync (dpy, False);

  /* lars stuff */
  s->desktop = 0;
  for (t = 0; t < prefs.desktops; t++)
    {
      s->clickthru[t] = prefs.clickthru[s->num][t];
      s->tile_resize[t] = prefs.tile_resize[s->num][t];
      s->tile_show_bottom[t] = prefs.tile_show_bottom[s->num][t];
      s->skip_focus[t] = prefs.skip_focus[s->num][t];
      s->left_track_width[t] = prefs.left_track_width[s->num][t];
      s->bigmr[t] = 1;
      s->focused[t] = 0;
      s->notilefocused[t] = 0;
      s->notile_raised[t] = 0;
      s->two_on_left[t] = 0;
    }

  s->barwin =
    XCreateSimpleWindow (dpy, s->root, BAR_X (s), BAR_Y (s), BAR_WIDTH (s),
			 BAR_HEIGHT, 0, s->black, s->white);
  XSelectInput (dpy, s->barwin,
		ExposureMask | ButtonPressMask | ButtonReleaseMask);
  raise_tbar (s);
  s->tile_height = BAR_Y (s);
  s->res_height = BAR_Y (s);
}

ScreenInfo *
getscreen (Window w)
{
  int i;

  for (i = 0; i < num_screens; i++)
    if (screens[i].root == w)
      return &screens[i];

  return 0;
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

void
sendconfig (Client * c)
{
  XConfigureEvent ce;

  ce.type = ConfigureNotify;
  ce.event = c->window;
  ce.window = c->window;
  ce.x = c->x;
  ce.y = c->y;
  ce.width = c->dx;
  ce.height = c->dy;
  ce.border_width = c->border;
  ce.above = None;
  ce.override_redirect = 0;

  XSendEvent (dpy, c->window, False, StructureNotifyMask, (XEvent *) & ce);
}

void
cleanup (void)
{
  Client *c, *cc[2], *next;
  XWindowChanges wc;
  int i;

  /* order of un-reparenting determines final stacking order... */
  cc[0] = cc[1] = 0;

  for (c = clients; c; c = next)
    {
      next = c->next;
      i = normal (c);
      c->next = cc[i];
      cc[i] = c;
    }

  for (i = 0; i < 2; i++)
    {
      for (c = cc[i]; c; c = c->next)
	{
	  if (!withdrawn (c))
	    {
	      gravitate (c, 1);
	      XReparentWindow (dpy, c->window, c->screen->root, c->x, c->y);
	    }

	  wc.border_width = c->border;

	  XConfigureWindow (dpy, c->window, CWBorderWidth, &wc);
	}
    }

  XSetInputFocus (dpy, PointerRoot, RevertToPointerRoot, CurrentTime);

  for (i = 0; i < num_screens; i++)
    cmapnofocus (&screens[i]);

  XCloseDisplay (dpy);
}
