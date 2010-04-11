/* Copyright (c) 1994-1996 David Hogan, see README for licence details */
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

Client *clients;
Client *current;
Client *prev_current;

void
setactive (Client * c, int on)
{
  if (!c || (c->parent == c->screen->root))
    {
      nofocus ();
      return;
    }

  if (on)
    {
      XUngrabButton (dpy, AnyButton, AnyModifier, c->parent);
      XSetInputFocus (dpy, c->window, RevertToPointerRoot, CurrentTime);
      cmapfocus (c);
    }
  else
    {
      XUngrabButton (dpy, AnyButton, AnyModifier, c->parent);
      XGrabButton (dpy, AnyButton, AnyModifier, c->parent, False,
		   ButtonPressMask | ButtonReleaseMask, GrabModeSync,
		   GrabModeSync, None, None);
    }

  draw_border (c, on);
}

void
draw_border (Client * c, int on)
{
#ifdef THREE_D
  GC topshadegc;
  GC botshadegc;
#endif

  if (!c)
    return;

  if (c->parent == c->screen->root)
    return;

  XSetWindowBackground (dpy, c->parent,
			on ? c->screen->black : c->screen->white);
  XClearWindow (dpy, c->parent);

#ifdef THREE_D
  botshadegc = on ? c->screen->botblackgc : c->screen->botwhitegc;
  topshadegc = on ? c->screen->topblackgc : c->screen->topwhitegc;

  XDrawLine (dpy, c->parent, botshadegc,
	     0, (2 * BORDER) + c->dy - 1, (2 * BORDER) + c->dx - 1,
	     (2 * BORDER) + c->dy - 1);
  XDrawLine (dpy, c->parent, botshadegc, (2 * BORDER) + c->dx - 1,
	     (2 * BORDER) + c->dy - 1, (2 * BORDER) + c->dx - 1, 0);
  XDrawLine (dpy, c->parent, topshadegc, 0, 0, (2 * BORDER) + c->dx - 1, 0);
  XDrawLine (dpy, c->parent, topshadegc, 0, 0, 0, (2 * BORDER) + c->dy - 1);
#endif
}

void
active (Client * c)
{
  int i;

  if (!c)
    return;

  if (c == current)
    return;

  if (current)
    {
      setactive (current, 0);

      if (current->screen != c->screen)
	cmapnofocus (current->screen);
    }

  setactive (c, 1);

  if (current)
    prev_current = current;

  current = c;

  for (i = 0; i < prefs.desktops; i++)
    {
      if (current->screen->notilefocused[i] == current)
	current->screen->notilefocused[i] = 0;

      if (current->screen->focused[i] == current)
	current->screen->focused[i] = 0;
    }

  if (!current->istool)
    {
      if (current->isnotile)
	current->screen->notilefocused[current->screen->desktop] = current;
      else
	current->screen->focused[current->screen->desktop] = current;
    }

  update_tbar ();
}

void
nofocus (void)
{
  static Window w = 0;
  int mask;
  XSetWindowAttributes attr;

  if (current)
    {
      prev_current = current;
      setactive (current, 0);
      cmapnofocus (current->screen);
    }

  current = 0;

  if (w == 0)
    {
      mask = CWOverrideRedirect;
      attr.override_redirect = 1;
      w = XCreateWindow (dpy, screens[0].root, 0, 0, 1, 1, 0,
			 CopyFromParent, InputOnly, CopyFromParent, mask,
			 &attr);
      XMapWindow (dpy, w);
    }

  XSetInputFocus (dpy, w, RevertToPointerRoot, CurrentTime);

  update_tbar ();
}

void
top (Client * c)
{
  Client **l, *cc;

  l = &clients;
  for (cc = *l; cc; cc = *l)
    {
      if (cc == c)
	{
	  *l = c->next;
	  c->next = clients;
	  clients = c;
	  return;
	}
      l = &cc->next;
    }
}

Client *
getclient (Window w, int create)
{
  Client *c;

  if (w == 0 || getscreen (w) || getbarscreen (w))
    return 0;

  for (c = clients; c; c = c->next)
    if (c->window == w || c->parent == w)
      return c;

  if (!create)
    return 0;

  c = (Client *) malloc (sizeof (Client));
  memset (c, 0, sizeof (Client));
  c->window = w;
  /* c->parent will be set by the caller */
  c->parent = None;
  c->reparenting = 0;
  c->state = WithdrawnState;
  c->hidden = 0;
  c->iszoom = ZOOM_UNZOOMED;
  c->isaside = 0;
  c->isleft = 0;
  c->init = 0;
  c->cmap = None;
  c->label = c->class = 0;
  c->ncmapwins = 0;
  c->cmapwins = 0;
  c->wmcmaps = 0;
  c->desktop = 0;
  c->next = clients;
  clients = c;
  return c;
}

void
rmclient (Client * c)
{
  int i;
  Client *cc;

  if (prev_current == c)
    prev_current = 0;

  for (i = 0; i < num_screens; i++)
    {
      int d;

      for (d = 0; d < prefs.desktops; d++)
	{
	  if (screens[i].focused[d] == c)
	    screens[i].focused[d] = 0;

	  if (screens[i].notilefocused[d] == c)
	    screens[i].notilefocused[d] = 0;
	}
    }

  if (c == clients)
    {
      clients = c->next;
    }

  for (cc = clients; cc && cc->next; cc = cc->next)
    {
      if (cc->next == c)
	{
	  cc->next = cc->next->next;
	}
    }

  if (hidden (c))
    {
      unhidec (c, 0);
    }

  if (c->parent != c->screen->root)
    {
      XDestroyWindow (dpy, c->parent);
    }

  c->parent = c->window = None;

  if (current == c)
    {
      current = 0;
      revert_window (c->screen);
    }

  if (c->ncmapwins != 0)
    {
      XFree ((char *) c->cmapwins);
      free ((char *) c->wmcmaps);
    }

  if (c->iconname != 0)
    XFree ((char *) c->iconname);

  if (c->name != 0)
    XFree ((char *) c->name);

  if (c->instance != 0)
    XFree ((char *) c->instance);

  if (c->class != 0)
    XFree ((char *) c->class);

  memset (c, 0, sizeof (Client));

  free (c);
}
