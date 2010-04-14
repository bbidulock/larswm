/* Copyright (c) 1994-1996 David Hogan, see README for licence details */
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

int
manage (Client * c, int mapped)
{
  int dohide, docenter = 1, state;
  long msize;
  XClassHint class;
  XWMHints *hints;

  XSelectInput (dpy, c->window,
		ColormapChangeMask | PropertyChangeMask | EnterWindowMask |
		FocusChangeMask);

  c->istool = 0;
  c->issticky = 0;
  c->isnotile = 0;

  if (XGetClassHint (dpy, c->window, &class) != 0)
    {
      c->instance = class.res_name;
      c->class = class.res_class;
    }
  else
    {
      c->instance = 0;
      c->class = 0;
    }

  c->iconname = getprop (c->window, XA_WM_ICON_NAME);
  c->name = getprop (c->window, XA_WM_NAME);
  setlabel (c);

  hints = XGetWMHints (dpy, c->window);

  if (XGetWMNormalHints (dpy, c->window, &c->size, &msize) == 0
      || c->size.flags == 0)
    {
      c->size.flags = PSize;
    }

  if (is_tool (c))
    {
      c->istool = 1;
      c->isfloat = 1;
      c->issticky = 1;
      c->isnotile = 1;
    }

  if (is_float (c))
    {
      c->isfloat = 1;
      c->isnotile = 1;
    }

  if (is_sticky (c))
    {
      c->issticky = 1;
    }

  if (is_notile (c))
    {
      c->isnotile = 1;
    }

  getcmaps (c);
  getproto (c);
  gettrans (c);

  if (c->trans != None)
    {
      c->isnotile = 1;
    }

  if (!getstate (c->window, &state))
    {
      state = hints ? hints->initial_state : NormalState;
    }

  dohide = (state == IconicState);

  if ((c->size.flags & USPosition) || (c->size.flags & PPosition))
    {
      docenter = 0;
    }

  if (c->size.flags & PBaseSize)
    {
      c->min_dx = c->size.base_width;
      c->min_dy = c->size.base_height;
    }
  else if (c->size.flags & PMinSize)
    {
      c->min_dx = c->size.min_width;
      c->min_dy = c->size.min_height;
    }
  else
    {
      c->min_dx = c->min_dy = 1;
    }

  if (hints)
    {
      XFree (hints);
    }

  gravitate (c, 0);

  c->parent = XCreateSimpleWindow (dpy, c->screen->root,
				   c->x, c->y,
				   c->dx + (2 * BORDER), c->dy + (2 * BORDER),
				   0, c->screen->black, c->screen->white);

  XSelectInput (dpy, c->parent,
		ExposureMask | SubstructureRedirectMask |
		SubstructureNotifyMask);

  if (mapped)
    {
      c->reparenting = 1;
    }

  XSetWindowBorderWidth (dpy, c->window, 0);
  XReparentWindow (dpy, c->window, c->parent, BORDER, BORDER);

  c->isshaped = 0;

  if (shape)
    {
      XShapeSelectInput (dpy, c->window, ShapeNotifyMask);
      setshape (c);
    }

  XAddToSaveSet (dpy, c->window);

  if (dohide)
    {
      hide (c);
    }
  else
    {
      int dt = is_dt (c);

      if (dt < 0)
	{
	  dt = c->screen->desktop;
	}

      c->desktop = dt;

      if (c->desktop != c->screen->desktop)
	{
	  goto_desktop (c->screen, c->desktop);
	}

      XMapWindow (dpy, c->window);
      XMapRaised (dpy, c->parent);
      top (c);

      if ((c->trans != None) && current && (current->window == c->trans))
	{
	  active (c);
	}
      else if ((mouse_on_screen () == c->screen)
	       || (current && current->screen == c->screen))
	{
	  active (c);
	}
      else
	{
	  setactive (c, 0);
	}

      wmsetstate (c, NormalState);

      if (docenter)
	{
	  move_center (c);
	}
    }

  if (current && (current != c))
    {
      cmapfocus (current);
    }

  initkeys (c);
  c->init = 1;

  return 1;
}

void
scanwins (ScreenInfo * s)
{
  unsigned int i, nwins, dt;
  Client *c;
  Window dw1, dw2, *wins;
  XWindowAttributes attr;

  XQueryTree (dpy, s->root, &dw1, &dw2, &wins, &nwins);
  for (i = 0; i < nwins; i++)
    {
      XGetWindowAttributes (dpy, wins[i], &attr);
      if (attr.override_redirect || wins[i] == s->barwin)
	continue;
      c = getclient (wins[i], 1);
      if (c != 0 && c->window == wins[i] && !c->init)
	{
	  c->x = attr.x;
	  c->y = attr.y;
	  c->dx = attr.width;
	  c->dy = attr.height;
	  c->border = attr.border_width;
	  c->screen = s;
	  c->parent = s->root;

	  if (attr.map_state == IsViewable)
	    manage (c, 1);

	  if ((dt = is_dt (c)) >= 0)
	    {
	      if (dt < prefs.desktops)
		c->desktop = dt;
	    }

	  if (c->desktop != c->screen->desktop)
	    move_hide (c);
	}
    }
  XFree ((void *) wins);	/* cast is to shut stoopid compiler up */
}

void
gettrans (Client * c)
{
  Window trans;

  trans = None;
  if (XGetTransientForHint (dpy, c->window, &trans) != 0)
    c->trans = trans;
  else
    c->trans = None;
}

void
withdraw (Client * c)
{
  XUnmapWindow (dpy, c->parent);
  gravitate (c, 1);
  XReparentWindow (dpy, c->window, c->screen->root, c->x, c->y);
  gravitate (c, 0);
  XRemoveFromSaveSet (dpy, c->window);
  wmsetstate (c, WithdrawnState);

  ignore_badwindow = 1;
  XSync (dpy, False);
  ignore_badwindow = 0;
}

/* This seemed like such a simple concept in theory..... */
void
gravitate (Client * c, int invert)
{
  int dx = 0;
  int dy = 0;
  int gravity = NorthWestGravity;

  if (!c)
    return;

  if (c->size.flags & PWinGravity)
    gravity = c->size.win_gravity;

  switch (gravity)
    {
    case NorthEastGravity:
    case EastGravity:
    case SouthEastGravity:
      dx = -BORDER;
      break;
    }

  switch (gravity)
    {
    case SouthWestGravity:
    case SouthGravity:
    case SouthEastGravity:
      dy = -prefs.bar_height - BORDER;
      break;
    }

  if (invert)
    {
      dx = -dx;
      dy = -dy;
    }

  c->x += dx;
  c->y += dy;
}

static void
installcmap (ScreenInfo * s, Colormap cmap)
{
  if (cmap == None)
    XInstallColormap (dpy, s->def_cmap);
  else
    XInstallColormap (dpy, cmap);
}

void
cmapfocus (Client * c)
{
  int i, found;
  Client *cc;

  if (c == 0)
    return;
  else if (c->ncmapwins != 0)
    {
      found = 0;
      for (i = c->ncmapwins - 1; i >= 0; i--)
	{
	  installcmap (c->screen, c->wmcmaps[i]);
	  if (c->cmapwins[i] == c->window)
	    found++;
	}
      if (!found)
	installcmap (c->screen, c->cmap);
    }
  else if (c->trans != None && (cc = getclient (c->trans, 0)) != 0
	   && cc->ncmapwins != 0)
    cmapfocus (cc);
  else
    installcmap (c->screen, c->cmap);
}

void
cmapnofocus (ScreenInfo * s)
{
  installcmap (s, None);
}

void
getcmaps (Client * c)
{
  int n, i;
  Window *cw;
  XWindowAttributes attr;

  if (!c->init)
    {
      XGetWindowAttributes (dpy, c->window, &attr);
      c->cmap = attr.colormap;
    }

  n =
    _getprop (c->window, wm_colormaps, XA_WINDOW, 100L,
	      (unsigned char **) &cw);
  if (c->ncmapwins != 0)
    {
      XFree ((char *) c->cmapwins);
      free ((char *) c->wmcmaps);
    }
  if (n <= 0)
    {
      c->ncmapwins = 0;
      return;
    }

  c->ncmapwins = n;
  c->cmapwins = cw;

  c->wmcmaps = (Colormap *) malloc (n * sizeof (Colormap));
  for (i = 0; i < n; i++)
    {
      if (cw[i] == c->window)
	c->wmcmaps[i] = c->cmap;
      else
	{
	  XSelectInput (dpy, cw[i], ColormapChangeMask);
	  XGetWindowAttributes (dpy, cw[i], &attr);
	  c->wmcmaps[i] = attr.colormap;
	}
    }
}

void
setlabel (Client * c)
{
  char *label;

  if (c->name != 0)
    label = c->name;
  else if (c->iconname != 0)
    label = c->iconname;
  else if (c->instance != 0)
    label = c->instance;
  else if (c->class != 0)
    label = c->class;
  else
    label = "???";

  c->label = label;

  if (normal (c))
    draw_border (c, c == current ? 1 : 0);

  if (c == current)
    update_tbar ();
}

void
setshape (Client * c)
{
  int n, order;
  XRectangle *rect;

  /* don't try to add a border if the window is non-rectangular */
  rect = XShapeGetRectangles (dpy, c->window, ShapeBounding, &n, &order);

  if (n > 1)
    {
      XShapeCombineShape (dpy, c->parent, ShapeBounding, BORDER, BORDER,
			  c->window, ShapeBounding, ShapeSet);
      c->isshaped = 1;
    }
  else
    {
      c->isshaped = 0;
    }

  XFree ((void *) rect);
}

int
_getprop (Window w, Atom a, Atom type, long len, unsigned char **p)
{
  Atom real_type;
  int format;
  unsigned long n, extra;
  int status;

  status =
    XGetWindowProperty (dpy, w, a, 0L, len, False, type, &real_type, &format,
			&n, &extra, p);
  if (status != Success || *p == 0)
    return -1;
  if (n == 0)
    XFree ((void *) *p);
  /* could check real_type, format, extra here... */
  return n;
}

char *
getprop (Window w, Atom a)
{
  unsigned char *p;

  if (_getprop (w, a, XA_STRING, 100L, &p) <= 0)
    return 0;
  return (char *) p;
}

int
get1prop (Window w, Atom a, Atom type)
{
  char **p, *x;

  if (_getprop (w, a, type, 1L, (unsigned char **) &p) <= 0)
    return 0;
  x = *p;
  XFree ((void *) p);
  return (int) x;
}

void
wmsetstate (Client * c, int state)
{
  long data[2];

  data[0] = (long) state;
  data[1] = (long) None;

  c->state = state;
  XChangeProperty (dpy, c->window, wm_state, wm_state, 32,
		   PropModeReplace, (unsigned char *) data, 2);
}

int
getstate (Window w, int *state)
{
  long *p = 0;

  if (_getprop (w, wm_state, wm_state, 2L, (unsigned char **) &p) <= 0)
    return 0;

  *state = (int) *p;
  XFree ((char *) p);
  return 1;
}

void
getproto (Client * c)
{
  Atom *p;
  int i;
  long n;
  Window w;

  w = c->window;
  c->proto = 0;
  if ((n =
       _getprop (w, wm_protocols, XA_ATOM, 20L, (unsigned char **) &p)) <= 0)
    return;

  for (i = 0; i < n; i++)
    if (p[i] == wm_delete)
      c->proto |= Pdelete;

  XFree ((char *) p);
}
