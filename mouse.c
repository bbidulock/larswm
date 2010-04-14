/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */
/* this is my replacement for the old grab.c */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"


void
adjust_hints (Client * c)
{
  if (c->size.flags & PResizeInc)
    {
      c->dx =
	c->min_dx + (c->dx -
		     c->min_dx) / c->size.width_inc * c->size.width_inc;
      c->dy =
	c->min_dy + (c->dy -
		     c->min_dy) / c->size.height_inc * c->size.height_inc;
    }

  if (c->size.flags & PMinSize)
    {
      if (c->dx < c->size.min_width)
	c->dx = c->size.min_width;

      if (c->dy < c->size.min_height)
	c->dy = c->size.min_height;
    }

  if (c->dx < 1)
    c->dx = 1;

  if (c->dy < 1)
    c->dy = 1;

  if (c->size.flags & PMaxSize)
    {
      if (c->dx > c->size.max_width)
	c->dx = c->size.max_width;

      if (c->dy > c->size.max_height)
	c->dy = c->size.max_height;
    }
}

void
move_opaque (Client * c)
{
  XEvent report;
  Client *p;
  int old_x, old_y;
  int move_x, move_y;
  char geo[16];
  int len;

  if (!c)
    return;

  getmouse (&old_x, &old_y, c->screen);

  XMapRaised (dpy, c->parent);
  setmouse (c->x, c->y, c->screen);

  XChangeActivePointerGrab (dpy,
			    PointerMotionHintMask | ButtonMotionMask |
			    ButtonReleaseMask | OwnerGrabButtonMask,
			    c->screen->place, CurrentTime);

  while (1)
    {
      XNextEvent (dpy, &report);

      switch (report.type)
	{
#ifdef THREE_D
	case Expose:
	  p = getclient (report.xexpose.window, 0);

	  if (p)
	    {
	      draw_border (p, p == current ? 1 : 0);
	    }
	  break;
#endif

	case ButtonRelease:
	  if (report.xbutton.button == Button1)
	    {
	      getmouse (&move_x, &move_y, c->screen);

	      c->x = move_x;
	      c->y = move_y;

	      XMoveWindow (dpy, c->parent, c->x, c->y);
	      sendconfig (c);

	      setmouse (old_x, old_y, c->screen);
	      update_tbar ();
	      return;
	    }
	  break;

	case MotionNotify:
	  while (XCheckTypedEvent (dpy, MotionNotify, &report));

	  getmouse (&move_x, &move_y, c->screen);

	  XMoveWindow (dpy, c->parent, move_x, move_y);
	  sendconfig (c);

	  len = sprintf (geo, "+%d+%d", move_x, move_y);

	  XClearWindow (dpy, c->screen->barwin);
#ifdef THREE_D
	  XDrawLine (dpy, c->screen->barwin, c->screen->botwhitegc, 0,
		     prefs.bar_height - 1, BAR_WIDTH (c->screen) - 1,
		     prefs.bar_height - 1);
	  XDrawLine (dpy, c->screen->barwin, c->screen->botwhitegc,
		     BAR_WIDTH (c->screen) - 1, prefs.bar_height - 1,
		     BAR_WIDTH (c->screen) - 1, 0);
	  XDrawLine (dpy, c->screen->barwin, c->screen->topwhitegc, 0, 0,
		     BAR_WIDTH (c->screen) - 1, 0);
	  XDrawLine (dpy, c->screen->barwin, c->screen->topwhitegc, 0, 0, 0,
		     prefs.bar_height - 1);
#endif
	  XDrawString (dpy, c->screen->barwin, c->screen->gc, prefs.bar_height,
		       BAR_TEXT_Y, geo, len);
	  break;
	}
    }
}

void
resize_opaque (Client * c)
{
  XEvent report;
  Client *p;
  int old_x, old_y;
  int move_x, move_y;
  char geo[32];
  int len, sx, sy, rx = 0, ry = 0, mx = 0, my = 0;

  if (!c)
    return;

  getmouse (&old_x, &old_y, c->screen);

  if (c->size.flags & PMinSize)
    {
      mx = c->size.min_width;
      my = c->size.min_height;
    }

  if (c->size.flags & PResizeInc)
    {
      rx = c->size.width_inc;
      ry = c->size.height_inc;
    }

  XMapRaised (dpy, c->parent);
  setmouse (c->x + c->dx + 1 + BORDER, c->y + c->dy + 1 + BORDER, c->screen);

  XChangeActivePointerGrab (dpy,
			    PointerMotionHintMask | ButtonMotionMask |
			    ButtonReleaseMask | OwnerGrabButtonMask,
			    c->screen->sweep, CurrentTime);

  while (1)
    {
      XNextEvent (dpy, &report);

      switch (report.type)
	{
#ifdef THREE_D
	case Expose:
	  p = getclient (report.xexpose.window, 0);

	  if (p)
	    {
	      draw_border (p, p == current ? 1 : 0);
	    }
	  break;
#endif

	case ButtonRelease:
	  if (report.xbutton.button == Button1)
	    {
	      getmouse (&move_x, &move_y, c->screen);

	      c->dx = move_x - BORDER - c->x - 1;
	      c->dy = move_y - BORDER - c->y - 1;

	      adjust_hints (c);

	      XResizeWindow (dpy, c->window, c->dx, c->dy);
	      XResizeWindow (dpy, c->parent, c->dx + (2 * BORDER),
			     c->dy + (2 * BORDER));
	      sendconfig (c);

	      if (shape)
		setshape (c);

	      setmouse (old_x, old_y, c->screen);
	      update_tbar ();
	      return;
	    }
	  break;

	case MotionNotify:
	  while (XCheckTypedEvent (dpy, MotionNotify, &report));

	  getmouse (&move_x, &move_y, c->screen);

	  c->dx = move_x - BORDER - c->x - 1;
	  c->dy = move_y - BORDER - c->y - 1;

	  adjust_hints (c);

	  sx = c->dx;
	  sy = c->dy;

	  XResizeWindow (dpy, c->window, c->dx, c->dy);
	  XResizeWindow (dpy, c->parent, c->dx + (2 * BORDER),
			 c->dy + (2 * BORDER));
	  sendconfig (c);

	  if (shape)
	    setshape (c);

	  if (rx)
	    {
	      if (mx)
		sx -= mx;

	      sx /= rx;
	      sx++;
	    }

	  if (ry)
	    {
	      if (my)
		sy -= my;

	      sy /= ry;
	      sy++;
	    }

	  if (rx || ry)
	    len = sprintf (geo, "%dx%d (%dx%d)", sx, sy, c->dx, c->dy);
	  else
	    len = sprintf (geo, "%dx%d", c->dx, c->dy);


	  XClearWindow (dpy, c->screen->barwin);
#ifdef THREE_D
	  XDrawLine (dpy, c->screen->barwin, c->screen->botwhitegc, 0,
		     prefs.bar_height - 1, BAR_WIDTH (c->screen) - 1,
		     prefs.bar_height - 1);
	  XDrawLine (dpy, c->screen->barwin, c->screen->botwhitegc,
		     BAR_WIDTH (c->screen) - 1, prefs.bar_height - 1,
		     BAR_WIDTH (c->screen) - 1, 0);
	  XDrawLine (dpy, c->screen->barwin, c->screen->topwhitegc, 0, 0,
		     BAR_WIDTH (c->screen) - 1, 0);
	  XDrawLine (dpy, c->screen->barwin, c->screen->topwhitegc, 0, 0, 0,
		     prefs.bar_height - 1);
#endif
	  XDrawString (dpy, c->screen->barwin, c->screen->gc, prefs.bar_height,
		       BAR_TEXT_Y, geo, len);
	  break;
	}
    }
}
