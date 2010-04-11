/* Copyright (c) 2000-2004 Lars Bernhardsson, see README for license details */
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

/*
 * This file was added to David Hogan's 9wm source distribution.
 * It is written by Lars Bernhardsson, with inspiration from
 * such things as acme, wily, oberon and 100's of misplaced windows
 * over the years. :)
 */

/* only tile visible top level windows on the current desktop. */
#define DO_TILE(x,s) (((x)->screen == (s)) && (((x)->desktop == (s)->desktop) || (x)->issticky) && ((x)->trans == None) && (!(x)->isnotile) && (!(x)->istool) && (normal(x)))

/* windows to raise when Raise Untiled is selected. */
#define DO_RAISE_NOTILE(x,s) (((x)->screen == (s)) && (((x)->desktop == (s)->desktop) || (x)->issticky) && ((x)->isnotile) && (normal(x)))

/* global variables for this module */
static int track_x[64][2];
static int track_width[64][2];

#define MAX_RC 1024
static Client *rc[MAX_RC];
static int nrc;

/* which tracks to reserve tool space in. */
static int res_track = 0;


void
getmouse (int *x, int *y, ScreenInfo * s)
{
  Window dw1, dw2;
  int t1, t2;
  unsigned int t3;

  XQueryPointer (dpy, s->root, &dw1, &dw2, x, y, &t1, &t2, &t3);
}

void
setmouse (int x, int y, ScreenInfo * s)
{
  XWarpPointer (dpy, None, s->root, None, None, None, None, x, y);
}

void
rev_clients (void)
{
  Client *c;

  nrc = 0;

  for (c = clients; c && (nrc < MAX_RC); c = c->next)
    rc[nrc++] = c;
}

int
is_float (Client * c)
{
  int i;

  if (!c->class)
    return 0;

  for (i = 0; i < MAXFLOATCLASS; i++)
    {
      char *inst;

      if (!prefs.floatclass[i])
	continue;

      if (strcmp (c->class, prefs.floatclass[i]) == 0)
	return 1;

      if (!c->instance)
	return 0;

      if (strcmp (c->instance, prefs.floatclass[i]) == 0)
	return 1;

      if ((inst = strchr (prefs.floatclass[i], '~')))
	{
	  inst++;
	  if (strncmp (c->class, prefs.floatclass[i], strlen (c->class)) == 0)
	    if (strncmp (c->instance, inst, strlen (c->instance)) == 0)
	      return 1;
	}
    }

  return 0;
}

int
is_sticky (Client * c)
{
  int i;

  if (!c->class)
    return 0;

  for (i = 0; i < MAXSTICKYCLASS; i++)
    {
      char *inst;

      if (!prefs.stickyclass[i])
	continue;

      if (strcmp (c->class, prefs.stickyclass[i]) == 0)
	return 1;

      if (!c->instance)
	return 0;

      if (strcmp (c->instance, prefs.stickyclass[i]) == 0)
	return 1;

      if ((inst = strchr (prefs.stickyclass[i], '~')))
	{
	  inst++;
	  if (strncmp (c->class, prefs.stickyclass[i], strlen (c->class)) ==
	      0)
	    if (strncmp (c->instance, inst, strlen (c->instance)) == 0)
	      return 1;
	}
    }

  return 0;
}

int
is_tool (Client * c)
{
  int i;

  if (!c->class)
    return 0;

  for (i = 0; i < MAXTOOLCLASS; i++)
    {
      char *inst;

      if (!prefs.toolclass[i])
	continue;

      if (strcmp (c->class, prefs.toolclass[i]) == 0)
	return 1;

      if (!c->instance)
	return 0;

      if (strcmp (c->instance, prefs.toolclass[i]) == 0)
	return 1;

      if ((inst = strchr (prefs.toolclass[i], '~')))
	{
	  inst++;
	  if (strncmp (c->class, prefs.toolclass[i], strlen (c->class)) == 0)
	    if (strncmp (c->instance, inst, strlen (c->instance)) == 0)
	      return 1;
	}
    }

  return 0;
}

int
is_notile (Client * c)
{
  int i;

  if (!c->class)
    return 1;

  for (i = 0; i < MAXNOTILECLASS; i++)
    {
      char *inst;

      if (!prefs.notileclass[i])
	continue;

      if (strcmp (c->class, prefs.notileclass[i]) == 0)
	return 0;

      if (!c->instance)
	return 1;

      if (strcmp (c->instance, prefs.notileclass[i]) == 0)
	return 0;

      if ((inst = strchr (prefs.notileclass[i], '~')))
	{
	  inst++;
	  if (strncmp (c->class, prefs.notileclass[i], strlen (c->class)) ==
	      0)
	    if (strncmp (c->instance, inst, strlen (c->instance)) == 0)
	      return 0;
	}
    }

  return 1;
}

int
is_dt (Client * c)
{
  int i;

  if (!c->class)
    return -1;

  for (i = 0; i < MAXDTCLASS; i++)
    {
      char *inst;

      if (!prefs.dtclass[i])
	continue;

      if (strcmp (c->class, prefs.dtclass[i]) == 0)
	return prefs.dtnum[i];

      if (!c->instance)
	return -1;

      if (strcmp (c->instance, prefs.dtclass[i]) == 0)
	return prefs.dtnum[i];

      if ((inst = strchr (prefs.dtclass[i], '~')))
	{
	  inst++;
	  if (strncmp (c->class, prefs.dtclass[i], strlen (c->class)) == 0)
	    if (strncmp (c->instance, inst, strlen (c->instance)) == 0)
	      return prefs.dtnum[i];
	}
    }

  return -1;
}

void
toggle_isfloat (Client * c)
{
  if (c)
    c->isfloat = !c->isfloat;

  update_tbar ();
}

void
toggle_istool (Client * c)
{
  if (c)
    c->istool = !c->istool;

  update_tbar ();
}

void
toggle_issticky (Client * c)
{
  if (c)
    c->issticky = !c->issticky;

  update_tbar ();
}

void
toggle_inc (ScreenInfo * s)
{
  if (s)
    s->bigmr[s->desktop] = !s->bigmr[s->desktop];

  update_tbar ();
}

void
toggle_clickthru (ScreenInfo * s)
{
  if (s)
    s->clickthru[s->desktop] = !s->clickthru[s->desktop];

  update_tbar ();
}

void
toggle_tile_resize (ScreenInfo * s)
{
  if (s)
    s->tile_resize[s->desktop] = !s->tile_resize[s->desktop];

  update_tbar ();
}

void
toggle_show_bottom (ScreenInfo * s)
{
  if (s)
    s->tile_show_bottom[s->desktop] = !s->tile_show_bottom[s->desktop];

  tile_all(s);
  update_tbar ();
}

void
toggle_skip_focus (ScreenInfo * s)
{
  if (s)
    s->skip_focus[s->desktop] = !s->skip_focus[s->desktop];

  update_tbar ();
}

int
count_clients (ScreenInfo * s)
{
  Client *c;
  int i = 0;

  for (c = clients; c; c = c->next)
    if (DO_TILE (c, s))
      i++;

  return i;
}

void
find_tools (ScreenInfo * s)
{
  Client *c;
  int min_y;
  int min_x, max_x;
  int t;
  int found = 0;

  if (!s)
    abort ();

  t = 0;
  max_x = -1;
  min_x = DisplayWidth (dpy, s->num) + 1;
  min_y = DisplayHeight (dpy, s->num) + 1;

  for (c = clients; c; c = c->next)
    {
      if (c->istool && (c->screen == s) && normal (c) &&
	  (c->issticky || (c->desktop == s->desktop)) &&
	  s->tile_resize[s->desktop] && !c->isfloat)
	{
	  int x1 = c->x - BORDER;
	  int y1 = c->y - BORDER;
	  int x2 = c->x + c->dx + BORDER;

	  found = 1;

	  if (x1 < min_x)
	    min_x = x1;

	  if (x2 > max_x)
	    max_x = x2;

	  if (y1 < min_y)
	    min_y = y1;
	}
    }

  if (found)
    {
      s->res_height = min_y - TILE_PAD;

      if (min_x < track_width[s->num][0])
	t |= 1;
      if (max_x >= track_x[s->num][1])
	t |= 2;
      res_track = t;
    }
  else
    {
      s->res_height = s->tile_height;
      res_track = 0;
    }
}

void
calc_track_sizes (ScreenInfo * s)
{
  int screen_width;

  if (!s)
    abort ();

  screen_width = DisplayWidth (dpy, s->num);
  track_x[s->num][0] = 0;
  track_width[s->num][0] =
    (s->left_track_width[s->desktop] * screen_width) / 100;

  track_x[s->num][1] = track_width[s->num][0];
  track_width[s->num][1] = screen_width - track_width[s->num][0];
}

void
toggle_notile (ScreenInfo * s)
{
  if (s->notile_raised[s->desktop])
    raise_tile (s, 1);
  else
    raise_notile (s);
}

void
raise_notile (ScreenInfo * s)
{
  int i;

  rev_clients ();

  for (i = nrc - 1; i >= 0; i--)
    if (DO_RAISE_NOTILE (rc[i], s))
      XMapRaised (dpy, rc[i]->parent);

  s->notile_raised[s->desktop] = 1;
  raise_tbar (s);

  if (s->notilefocused[s->desktop]
      && DO_RAISE_NOTILE (s->notilefocused[s->desktop], s)
      && !s->notilefocused[s->desktop]->isaside)
    {
      active (s->notilefocused[s->desktop]);
    }
  else
    {
      revert_same (s);
    }

  if (current)
    XMapRaised (dpy, current->parent);

  update_tbar ();
}

void
raise_tile (ScreenInfo * s, int user)
{
  Client *c;

  for (c = clients; c; c = c->next)
    if (DO_TILE (c, s))
      XMapRaised (dpy, c->parent);

  s->notile_raised[s->desktop] = 0;
  raise_tbar (s);

  if (s->focused[s->desktop] && DO_TILE (s->focused[s->desktop], s))
    {
      if (user || (!current && (mouse_on_screen () == s))
	  || (current && (current->screen == s)))
	{
	  active (s->focused[s->desktop]);
	}
    }
  else
    {
      revert_same (s);
    }

  update_tbar ();
}

void
tile_all (ScreenInfo * s)
{
  Client *c;

  unsigned int screen_height_left, screen_height_right, screen_y_left = 0;
  unsigned int track_width_left, track_width_right;

  unsigned short clients_left, clients_right;
  unsigned short current_client = 0;

  /** if tile_show_bottom, then place windows bottom at this level. */
  unsigned int current_bottom_y; 

  /** how much distance between consecutive tile lower/upper borders. */
  unsigned int vertical_tile_spacing;


  if (!s)
    abort ();


  clients_right = count_clients (s);

  if (clients_right == 0)
    return;

  if ( s->two_on_left[s->desktop] && (clients_right > 1))
    {
      clients_left = 2;
      clients_right = clients_right - 2;
    }
  else
    {
  clients_left = 1;
  --clients_right;
    }


  calc_track_sizes (s);
  find_tools (s);

  track_width_left = track_width[s->num][0];
  track_width_right = track_width[s->num][1];

  screen_height_left = (res_track & 1 ? s->res_height : s->tile_height);
	screen_height_left -= prefs.panelsize;
  screen_height_right = (res_track & 2 ? s->res_height : s->tile_height);
	screen_height_right -= prefs.panelsize;

  if (clients_right)
    {
      vertical_tile_spacing = screen_height_right / clients_right;

      /* first tiled window sits right above the bar */
      current_bottom_y = screen_height_right + vertical_tile_spacing - 2*BORDER;
    }

  for (c = clients; c; c = c->next)
    {
      /*
       * WARNING
       * The variables "screen_height_left", "odx" and "ody"
       * have more than one purpose.
       */
      if (DO_TILE (c, s))
	{
	  unsigned int x, y, width, height;
	  unsigned int odx, ody;

	  if (clients_left)
            {
              x = 0;
              width = track_width_left - TILE_PAD;
              if (clients_left == 2)
                {
                  y = prefs.panelsize;
                  height = screen_height_left * s->two_on_left[s->desktop] / 100;
                  screen_y_left = height;
                  height -= TILE_PAD;
                }
	      else
                {
                  y = screen_y_left + prefs.panelsize;
                  height = screen_height_left - screen_y_left;
                }
 
                /*
                 * Here the purpose of
                 * screen_height_left changes
                 */
                screen_height_left = screen_height_right;
            }
	  else
	    {
	      x = track_width_left;
	      y = screen_height_right - screen_height_left + prefs.panelsize;
	      height = vertical_tile_spacing - TILE_PAD;
	      width = track_width_right;

	      current_bottom_y -= vertical_tile_spacing;

	      if (++current_client < clients_right)
                screen_height_left -= vertical_tile_spacing;
	      else
		height = screen_height_left;

	    }

	  height -= 2 * BORDER;
	  width -= 2 * BORDER;

	  /*I am not sure all this checking is needed*/
	  c->isleft = (s->two_on_left[s->desktop] && clients_left == 2)
		  || (!s->two_on_left[s->desktop] && clients_left) ? 1 : 0;

	  odx = width;
	  ody = height;

	  /*
	   * resize in given increments if the program
	   * wishes this..
	   */
	  if (c->size.flags & PResizeInc)
	    {
	      width = c->min_dx +
		(((signed int) (width -
				c->min_dx) / c->size.width_inc) *
		 c->size.width_inc);

	      height = c->min_dy +
		(((signed int) (height -
				c->min_dy) / c->size.height_inc) *
		 c->size.height_inc);
	    }

	  /* don't exceed this size */
	  if (c->size.flags & PMaxSize)
	    {
	      if (width > c->size.max_width)
		width = c->size.max_width;

	      if (height > c->size.max_height)
		height = c->size.max_height;
	    }

	  /*
	   * adjust track width if left window doesn't use
	   * all of it or height left if a right window
	   * doesn't use it all..
	   */

	   if (clients_left)
	    {
		   /*track_width_right += odx - width;
		     track_width_left -= odx - width;*/
	      if (odx != width)
	      {
		 track_width_right +=
			 odx - width;
		 track_width_left -=
			 odx - width;
	    }
	      if ((clients_left == 2) && ody != height)
		      screen_y_left -= ody - height;

	    }
	   else
	      if (ody != height)
		      screen_height_left +=
			      ody - height;


	  /*
	   * The second meaning begins
	   */
	  odx = c->dx;
	  ody = c->dy;

	  /*
	   * if resize is true OR we're at the upper left corner
	   * set size to maximum
	   */
	  if (s->tile_resize[s->desktop] || clients_left)
	    {
	      c->dx = width;
	      c->dy = height;
	    }

          c->x = x;
          c->y = ((clients_left == 0)
                  && s->tile_show_bottom[s->desktop] 
                  && !s->tile_resize[s->desktop]) ?
            (current_bottom_y - c->dy) : y;

	  XMoveResizeWindow (dpy, c->parent, c->x, c->y,
			     c->dx + (2 * BORDER), c->dy + (2 * BORDER));
	  XResizeWindow (dpy, c->window, c->dx, c->dy);
	  sendconfig (c);

	  /* give focus to window if appropriate */
	  if (c->isleft
	      && (mouse_on_screen () == s) && !s->notile_raised[s->desktop])
	    {
	      if (!current
		  || !s->skip_focus[s->desktop]
		  || !s->tile_resize[s->desktop])
		active (c);
	    }

	  if (clients_left)
	    --clients_left;
	}
    }

  if (current && !current->isnotile && !s->skip_focus[s->desktop])
    {
      top (current);
      XMapRaised (dpy, current->parent);
    }

  if (!s->notile_raised[s->desktop])
    {
      raise_tile (s, 0);
    }
}

void
two_on_left (ScreenInfo *s)
{
  if (s)
    {
      s->two_on_left[s->desktop] = (s->two_on_left[s->desktop] ? 0 : 50);
      tile_all (s);
    }
}

void
two_on_left_grow (ScreenInfo *s, int dy)
{
  if (s && s->two_on_left[s->desktop])
    {
      int w = s->two_on_left[s->desktop] + dy;
      if ((w > 0) && (w < 100))
        {
	  s->two_on_left[s->desktop] = w;
	  tile_all (s);
	}
    }
}

void
goto_desktop (ScreenInfo * s, int n)
{
  if (s)
    {
      s->desktop = n;
      s->desktop %= prefs.desktops;
      update_desktop (s);
    }
}

void
next_desktop (ScreenInfo * s)
{
  if (s)
    {
      s->desktop++;
      s->desktop %= prefs.desktops;
      update_desktop (s);
    }
}

void
next_desktop_drag (ScreenInfo * s)
{
  Client *t;

  if (s)
    {
      if (current && (current->screen == s))
	{
	  t = current;

	  t->desktop++;
	  t->desktop %= prefs.desktops;

	  next_desktop (s);

	  active (t);

	  if (t->isnotile)
	    raise_notile (s);
	  else
	    raise_tile (s, 0);

	  update_tbar ();
	}
    }
}

void
prev_desktop (ScreenInfo * s)
{
  if (s)
    {
      s->desktop += (prefs.desktops - 1);
      s->desktop %= prefs.desktops;
      update_desktop (s);
    }
}

void
prev_desktop_drag (ScreenInfo * s)
{
  Client *t;

  if (s)
    {
      if (current && (current->screen == s))
	{
	  t = current;

	  t->desktop += (prefs.desktops - 1);
	  t->desktop %= prefs.desktops;

	  prev_desktop (s);

	  active (t);

	  if (t->isnotile)
	    raise_notile (s);
	  else
	    raise_tile (s, 0);

	  update_tbar ();
	}
    }
}

void
move_hide (Client * c)
{
  if (c)
    XMoveWindow (dpy, c->parent, DisplayWidth (dpy, c->screen->num), c->y);
}

void
move_show (Client * c)
{
  if (c)
    XMoveWindow (dpy, c->parent, c->x, c->y);
}

void
update_desktop (ScreenInfo * s)
{
  Client *c;

  for (c = clients; c; c = c->next)
    {
      if ((c->screen != s) || (c->parent == c->screen->root) || c->issticky
	  || (c->state != NormalState))
	continue;

      if (c->desktop == s->desktop)
	move_show (c);
      else
	move_hide (c);
    }

  if (!s->notile_raised[s->desktop] && s->focused[s->desktop])
    active (s->focused[s->desktop]);
  else if (s->notile_raised[s->desktop] && s->notilefocused[s->desktop])
    active (s->notilefocused[s->desktop]);
  else
    {
      revert_same (s);

      if (!current)
	revert_window (s);
    }

  tile_all (s);
  update_tbar ();
}

void
revert_window (ScreenInfo * s)
{
  Client *c;

  if (mouse_on_screen () != s)
    {
      if (current && (current->screen == s))
	nofocus ();

      return;
    }

  if (prev_current)
    {
      c = prev_current;

      if (!c->istool && !c->isaside
	  && (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
	{
	  if (c->isnotile)
	    raise_notile (s);
	  else
	    raise_tile (s, 0);

	  active (c);
	  return;
	}
    }

  for (c = clients; c; c = c->next)
    {
      if (!c->istool && !c->isaside
	  && (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
	{
	  if (c->isnotile)
	    raise_notile (s);
	  else
	    raise_tile (s, 0);

	  active (c);
	  return;
	}
    }

  nofocus ();
}

void
revert_same (ScreenInfo * s)
{
  Client *c;

  if (mouse_on_screen () != s)
    {
      if (current && (current->screen == s))
	nofocus ();

      return;
    }

  if (prev_current)
    {
      c = prev_current;

      if (!c->istool && !c->isaside
	  && (c->isnotile == s->notile_raised[s->desktop]) && (DO_TILE (c, s)
							       ||
							       DO_RAISE_NOTILE
							       (c, s)))
	{
	  active (c);
	  return;
	}
    }

  for (c = clients; c; c = c->next)
    {
      if (!c->istool && !c->isaside
	  && (c->isnotile == s->notile_raised[s->desktop]) && (DO_TILE (c, s)
							       ||
							       DO_RAISE_NOTILE
							       (c, s)))
	{
	  active (c);
	  return;
	}
    }

  nofocus ();
}

void
prev_window (ScreenInfo * s)
{
  Client *c;

  if (!current || !prev_current || (prev_current == current)
      || (prev_current->desktop != current->desktop)
      || (prev_current->isnotile != current->isnotile)
      || (!s->notile_raised[s->desktop] && !s->skip_focus[s->desktop]))
    {
      for (c = clients; c; c = c->next)
	{
	  if (!c->istool && !c->isaside &&
	      (c != current) &&
	      (c->isnotile == s->notile_raised[s->desktop]) &&
	      (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
	    {
	      active (c);

	      if (c->isnotile || (!c->isnotile && !s->skip_focus[s->desktop]))
		{
		  raise_tbar (s);
		  top (c);
		  XMapRaised (dpy, c->parent);
		  tile_all (s);
		}

	      return;
	    }
	}
      return;
    }

  c = prev_current;

  if (!c->istool && !c->isaside &&
      (c != current) &&
      (c->isnotile == s->notile_raised[s->desktop]) &&
      (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
    {
      active (c);

      if (c->isnotile || (!c->isnotile && !s->skip_focus[s->desktop]))
	{
	  raise_tbar (s);
	  top (c);
	  XMapRaised (dpy, c->parent);
	  tile_all (s);
	}
    }
}

void
prev_window_focus (ScreenInfo * s)
{
  if (s)
    {
      s->skip_focus[s->desktop] = !s->skip_focus[s->desktop];
      prev_window (s);
      s->skip_focus[s->desktop] = !s->skip_focus[s->desktop];
    }
}

void
next_window (ScreenInfo * s)
{
  Client *c;

  if (s->notile_raised[s->desktop] ||
      (!s->notile_raised[s->desktop] && !s->skip_focus[s->desktop]))
    {
      Client *last = 0;

      for (c = clients; c; c = c->next)
	{
	  if (!c->istool && !c->isaside &&
	      (c != current) &&
	      (c->isnotile == s->notile_raised[s->desktop]) &&
	      (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
	    {
	      last = c;
	    }
	}

      if (last)
	{
	  raise_tbar (s);
	  top (last);
	  XMapRaised (dpy, last->parent);

	  if (last->isnotile)
	    active (last);
	  else
	    tile_all (s);
	}

      return;
    }

  if (current &&
      current->next &&
      !s->notile_raised[s->desktop] && s->skip_focus[s->desktop])
    {
      for (c = current->next; c; c = c->next)
	{
	  if (!c->istool && !c->isaside &&
	      (c != current) &&
	      (c->isnotile == s->notile_raised[s->desktop]) &&
	      (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
	    {
	      active (c);

	      if (c->isnotile || (!c->isnotile && !s->skip_focus[s->desktop]))
		{
		  raise_tbar (s);
		  XMapRaised (dpy, c->parent);
		  tile_all (s);
		}

	      return;
	    }
	}
    }

  for (c = clients; c; c = c->next)
    {
      if (!c->istool && !c->isaside &&
	  (c != current) &&
	  (c->isnotile == s->notile_raised[s->desktop]) &&
	  (DO_TILE (c, s) || DO_RAISE_NOTILE (c, s)))
	{
	  active (c);

	  if (c->isnotile || (!c->isnotile && !s->skip_focus[s->desktop]))
	    {
	      raise_tbar (s);
	      top (c);
	      XMapRaised (dpy, c->parent);
	      tile_all (s);
	    }

	  return;
	}
    }
}

void
next_window_focus (ScreenInfo * s)
{
  if (s)
    {
      s->skip_focus[s->desktop] = !s->skip_focus[s->desktop];
      next_window (s);
      s->skip_focus[s->desktop] = !s->skip_focus[s->desktop];
    }
}

void
prev_screen (ScreenInfo * s)
{
  ScreenInfo *ns;
  int cur;

  if (!s)
    abort ();

  cur = s->num;

  if (--cur < 0)
    cur = num_screens - 1;

  raise_tbar (s);

  ns = &screens[cur];

  setmouse (DisplayWidth (dpy, cur) / 2, DisplayHeight (dpy, cur) - 2, ns);

  if (!ns->notile_raised[ns->desktop] && ns->focused[ns->desktop])
    active (ns->focused[ns->desktop]);
  else if (ns->notile_raised[ns->desktop] && ns->notilefocused[ns->desktop])
    active (ns->notilefocused[ns->desktop]);
  else
    {
      revert_same (ns);

      if (!current)
	revert_window (ns);
    }
}

void
next_screen (ScreenInfo * s)
{
  ScreenInfo *ns;
  int cur;

  if (!s)
    abort ();

  cur = s->num;

  if (++cur >= num_screens)
    cur = 0;

  raise_tbar (s);

  ns = &screens[cur];

  setmouse (DisplayWidth (dpy, cur) / 2, DisplayHeight (dpy, cur) - 2, ns);

  if (!ns->notile_raised[ns->desktop] && ns->focused[ns->desktop])
    active (ns->focused[ns->desktop]);
  else if (ns->notile_raised[ns->desktop] && ns->notilefocused[ns->desktop])
    active (ns->notilefocused[ns->desktop]);
  else
    {
      revert_same (ns);

      if (!current)
	revert_window (ns);
    }
}

ScreenInfo *
mouse_on_screen (void)
{
  int i;

  if (num_screens == 1)
    return &screens[0];

  for (i = 0; i < num_screens; i++)
    {
      Window root, child;
      int rx, ry, wx, wy;
      unsigned int mask;

      if (XQueryPointer
	  (dpy, screens[i].root, &root, &child, &rx, &ry, &wx, &wy,
	   &mask) == True)
	return &screens[i];
    }

  return 0;
}

void
shrink_left (ScreenInfo * s)
{
  if (s)
    {
      s->left_track_width[s->desktop] -= 5;

      if (s->left_track_width[s->desktop] < 5)
	s->left_track_width[s->desktop] = 5;

      tile_all (s);
    }
}

void
grow_left (ScreenInfo * s)
{
  if (s)
    {
      s->left_track_width[s->desktop] += 5;

      if (s->left_track_width[s->desktop] > 95)
	s->left_track_width[s->desktop] = 95;

      tile_all (s);
    }
}

void
max_left (ScreenInfo * s)
{
  if (s)
    {
      s->left_track_width[s->desktop] = 95;

      tile_all (s);
    }
}

void
restore_left (ScreenInfo * s)
{
  if (s)
    {
      s->left_track_width[s->desktop] =
	prefs.left_track_width[s->num][s->desktop];

      tile_all (s);
    }
}

void
move_northwest (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = 0;
      c->y = prefs.panelsize;
      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_north (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = (DisplayWidth (dpy, c->screen->num) / 2) - (c->dx / 2) - BORDER;
      c->y = prefs.panelsize;

      if (c->x < 0)
	c->x = 0;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_northeast (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = (DisplayWidth (dpy, c->screen->num)) - (c->dx) - (2 * BORDER);
      c->y = prefs.panelsize;
      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_west (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = 0;
      c->y = (c->screen->tile_height / 2) - (c->dy / 2) - BORDER;

      if (c->y < prefs.panelsize)
	c->y = prefs.panelsize;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_center (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = (DisplayWidth (dpy, c->screen->num) / 2) - (c->dx / 2) - BORDER;
      c->y = (c->screen->tile_height / 2) - (c->dy / 2) - BORDER;

      if (c->x < 0)
	c->x = 0;

      if (c->y < prefs.panelsize)
	c->y = prefs.panelsize;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_east (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = DisplayWidth (dpy, c->screen->num) - c->dx - (2 * BORDER);
      c->y = (c->screen->tile_height / 2) - (c->dy / 2) - BORDER;

      if (c->y < prefs.panelsize)
	c->y = prefs.panelsize;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_southwest (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = 0;
      c->y = c->screen->tile_height - c->dy - (2 * BORDER);
      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_south (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = (DisplayWidth (dpy, c->screen->num) / 2) - (c->dx / 2) - BORDER;
      c->y = c->screen->tile_height - c->dy - (2 * BORDER);

      if (c->x < 0)
	c->x = 0;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_southeast (Client * c)
{
  if (c && c->isnotile)
    {
      c->x = DisplayWidth (dpy, c->screen->num) - c->dx - (2 * BORDER);
      c->y = c->screen->tile_height - c->dy - (2 * BORDER);
      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_up (Client * c)
{
  if (c && c->isnotile)
    {
      int min_y = prefs.panelsize + MIN_VISIBLE - c->dy;
      if (c->screen->bigmr[c->screen->desktop])
	c->y -= (c->screen->tile_height / 20);
      else
	c->y--;

      if (c->y < min_y)
	c->y = min_y;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_down (Client * c)
{
  if (c && c->isnotile)
    {
      int max_y = c->screen->tile_height - MIN_VISIBLE;

      if (c->screen->bigmr[c->screen->desktop])
	c->y += (c->screen->tile_height / 20);
      else
	c->y++;

      if (c->y > max_y)
	c->y = max_y;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_left (Client * c)
{
  if (c && c->isnotile)
    {
      int min_x = MIN_VISIBLE - c->dx;
      if (c->screen->bigmr[c->screen->desktop])
	c->x -= (DisplayWidth (dpy, c->screen->num) / 20);
      else
	c->x--;

      if (c->x < min_x)
	c->x = min_x;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
move_right (Client * c)
{
  if (c && c->isnotile)
    {
      int max_x = DisplayWidth (dpy, c->screen->num) - MIN_VISIBLE;

      if (c->screen->bigmr[c->screen->desktop])
	c->x += (DisplayWidth (dpy, c->screen->num) / 20);
      else
	c->x++;

      if (c->x > max_x)
	c->x = max_x;

      XMoveWindow (dpy, c->parent, c->x, c->y);
      sendconfig (c);
    }
}

void
zoom (Client * c, int dimension)
{
  if (c)
    {
      if (!c->isnotile)
	{
	  if (c->screen->notile_raised[c->screen->desktop])
	    toggle_notile (c->screen);

	  if (c->isleft)
	    {
	      Client *p;

	      for (p = clients->next; p; p = p->next)
		{
		  if (!p->istool &&
		      (p != current) &&
		      (p->isnotile ==
		       p->screen->notile_raised[p->screen->desktop])
		      && (DO_TILE (p, c->screen)
			  || DO_RAISE_NOTILE (p, c->screen)))
		    {
		      active (p);
		      break;
		    }
		}

	      top (current);
	      tile_all (c->screen);
	    }
	  else
	    {
	      top (c);
	      tile_all (c->screen);
	    }
	}
      else if (c->iszoom == ZOOM_SELECT)
	{
	  move_select (c, 0);
	}
      else if (c->iszoom == ZOOM_NORMAL)
	{
	  c->x = c->zx;
	  c->y = c->zy;
	  c->dx = c->zdx;
	  c->dy = c->zdy;

	  XMoveResizeWindow (dpy, c->parent, c->x, c->y, c->dx + (2 * BORDER),
			     c->dy + (2 * BORDER));
	  XResizeWindow (dpy, c->window, c->dx, c->dy);
	  sendconfig (c);

	  c->iszoom = ZOOM_UNZOOMED;
	}
      else
	{
	  int height;
	  int width;

	  if (dimension & DIM_OUTSIDE)
	    {
	      width = DisplayWidth (dpy, c->screen->num);
	      height = DisplayHeight (dpy, c->screen->num);
	    }
	  else
	    {
	      height = c->screen->tile_height - (2 * BORDER);
	      width = DisplayWidth (dpy, c->screen->num) - (2 * BORDER);
	    }

	  if (c->size.flags & PResizeInc)
	    {
	      width =
		c->min_dx + (width -
			     c->min_dx) / c->size.width_inc *
		c->size.width_inc;
	      height =
		c->min_dy + (height -
			     c->min_dy) / c->size.height_inc *
		c->size.height_inc;
	    }

	  if (c->size.flags & PMaxSize)
	    {
	      if (width > c->size.max_width)
		width = c->size.max_width;

	      if (height > c->size.max_height)
		height = c->size.max_height;
	    }

	  c->zx = c->x;
	  c->zy = c->y;
	  c->zdx = c->dx;
	  c->zdy = c->dy;

	  if (dimension & DIM_HORIZ)
	    {
	      c->dx = width;

	      if (dimension & DIM_OUTSIDE)
		c->x = -BORDER;
	      else
		c->x =
		  (DisplayWidth (dpy, c->screen->num) / 2) - (c->dx / 2) -
		  BORDER;
	    }

	  if (dimension & DIM_VERT)
	    {
	      c->dy = height;

	      if (dimension & DIM_OUTSIDE)
		c->y = -BORDER;
	      else
		c->y = (c->screen->tile_height / 2) - (c->dy / 2) - BORDER;
	    }

	  XMoveResizeWindow (dpy, c->parent, c->x, c->y, c->dx + (2 * BORDER),
			     c->dy + (2 * BORDER));
	  XResizeWindow (dpy, c->window, c->dx, c->dy);
	  sendconfig (c);

	  c->iszoom = ZOOM_NORMAL;
	}

      update_tbar ();
    }
}

void
grow (Client * c, int dimension, int m)
{
  int h, w, dh, dw, x, y;
  int odx, ody, want_w, want_h;
  int ddx, ddy;

  if (!c || !dimension)
    return;

  if (!c->isnotile)
    {
      if ((dimension == DIM_HORIZ) || (dimension == (DIM_HORIZ | DIM_VERT)))
	{
	  if (m > 0)
	    grow_left (c->screen);
	  else
	    shrink_left (c->screen);
	}
      else if (dimension == DIM_VERT)
	{
	  if (m > 0)
	    max_left (c->screen);
	  else
	    restore_left (c->screen);
	}
      return;
    }

  x = c->x;
  y = c->y;

  w = odx = c->dx;
  h = ody = c->dy;

  dw = DisplayWidth (dpy, c->screen->num) - (2 * BORDER);
  dh = c->screen->tile_height - (2 * BORDER) - prefs.panelsize;

  if (c->screen->bigmr[c->screen->desktop])
    {
      ddx = (dw / 20);
      ddy = (dh / 20);
    }
  else if (c->size.flags & PResizeInc)
    {
      ddx = c->size.width_inc;
      ddy = c->size.height_inc;
    }
  else
    {
      ddx = 2;
      ddy = 2;
    }

  if (dimension & DIM_HORIZ)
    {
      x -= (ddx / 2) * m;
      w += ddx * m;

      if ((x + w) > dw)
	x = dw - w;

      if (x < 0)
	x = 0;

      if ((x + w) > dw)
	w = dw;

      if (w < 1)
	w = 1;
    }

  if (dimension & DIM_VERT)
    {
      y -= (ddy / 2) * m;
      h += ddy * m;

      if ((y + h) > dh)
	y = dh - h;

      if (y < prefs.panelsize)
	y = prefs.panelsize;

      if ((y + h) > dh)
	h = dh;

      if (h < 1)
	h = 1;
    }

  want_w = w;
  want_h = h;

  if (c->size.flags & PResizeInc)
    {
      w = c->min_dx + (w - c->min_dx) / c->size.width_inc * c->size.width_inc;
      h =
	c->min_dy + (h - c->min_dy) / c->size.height_inc * c->size.height_inc;
    }

  if (c->size.flags & PMinSize)
    {
      if (w < c->size.min_width)
	w = c->size.min_width;

      if (h < c->size.min_height)
	h = c->size.min_height;
    }

  if (c->size.flags & PMaxSize)
    {
      if (w > c->size.max_width)
	w = c->size.max_width;

      if (h > c->size.max_height)
	h = c->size.max_height;
    }

  x += ((want_w - w) / 2);
  y += ((want_h - h) / 2);

  if ((w == odx) && (h == ody))
    return;

  if (w != odx)
    {
      c->x = x;
      c->dx = w;
    }

  if (h != ody)
    {
      c->y = y;
      c->dy = h;
    }

  XMoveResizeWindow (dpy, c->parent, c->x, c->y, c->dx + (2 * BORDER),
		     c->dy + (2 * BORDER));
  XResizeWindow (dpy, c->window, c->dx, c->dy);
  sendconfig (c);
}

void
move_select (Client * c, int untiled)
{
  int h, w, dh, dw, x, y;
  int odx, ody;

  if (!c || !c->isnotile || c->isaside)
    return;

  odx = c->dx;
  ody = c->dy;

  if (c->iszoom == ZOOM_NORMAL)
    {
      zoom (c, DIM_HORIZ | DIM_VERT);

      return;
    }
  else if (c->iszoom == ZOOM_SELECT)
    {
      c->x = c->zx;
      c->y = c->zy;
      c->dx = c->zdx;
      c->dy = c->zdy;

      XMoveResizeWindow (dpy, c->parent, c->x, c->y, c->dx + (2 * BORDER),
			 c->dy + (2 * BORDER));
      XResizeWindow (dpy, c->window, c->dx, c->dy);
      sendconfig (c);

      XMapRaised (dpy, c->parent);
      top (c);
      active (c);

      if (!c->screen->notile_raised[c->screen->desktop])
	raise_notile (c->screen);

      c->iszoom = ZOOM_UNZOOMED;

      update_tbar ();
      return;
    }

  x = c->x;
  y = c->y;

  w = odx;
  h = ody;

  dh = c->screen->tile_height - (2 * BORDER);
  dw = DisplayWidth (dpy, c->screen->num) - (2 * BORDER);

  if (untiled)			/* window that just stopped being tiled, restore original size */
    {
      if ((c->size.flags & PSize)
	  && ((c->size.height != ody) || (c->size.width != odx)))
	{
	  h = c->size.height;
	  w = c->size.width;
	}
      else			/* emacs changes size hints when you resize it... odd. */
	{
	  h = dh / 2;
	  w = dw / 2;
	}
    }
  else
    {
      h = dh;
      w = (17 * h) / 22;	/* US Letter ratio */
    }

  if (w > dw)			/* for screens with non-standard ratios */
    w = dw;

  if (h > dh)			/* for when hints says taller than screen */
    h = dh;

  if (c->size.flags & PResizeInc)
    {
      w = c->min_dx + (w - c->min_dx) / c->size.width_inc * c->size.width_inc;
      h =
	c->min_dy + (h - c->min_dy) / c->size.height_inc * c->size.height_inc;
    }

  if (c->size.flags & PMinSize)
    {
      if (w < c->size.min_width)
	w = c->size.min_width;

      if (h < c->size.min_height)
	h = c->size.min_height;
    }

  if (c->size.flags & PMaxSize)
    {
      if (w > c->size.max_width)
	w = c->size.max_width;

      if (h > c->size.max_height)
	h = c->size.max_height;
    }

  x = (DisplayWidth (dpy, c->screen->num) / 2) - (w / 2) - BORDER;
  y = (c->screen->tile_height / 2) - (h / 2) - BORDER;

  if (c->x < 0)
    c->x = 0;

  if (c->y < prefs.panelsize)
    c->y = prefs.panelsize;

  c->zx = c->x;
  c->zy = c->y;
  c->zdx = c->dx;
  c->zdy = c->dy;

  if (w != odx)
    {
      c->x = x;
      c->dx = w;
    }

  if (h != ody)
    {
      c->y = y;
      c->dy = h;
    }

  XMoveResizeWindow (dpy, c->parent, c->x, c->y, c->dx + (2 * BORDER),
		     c->dy + (2 * BORDER));
  XResizeWindow (dpy, c->window, c->dx, c->dy);
  sendconfig (c);

  XMapRaised (dpy, c->parent);
  top (c);
  active (c);

  if (!c->screen->notile_raised[c->screen->desktop])
    raise_notile (c->screen);

  if (untiled)			/* if just stopped being tiled, do not count this as zoomed */
    c->iszoom = ZOOM_UNZOOMED;
  else
    c->iszoom = ZOOM_SELECT;

  update_tbar ();
}

int
count_aside (ScreenInfo * s)
{
  int i = 0;
  Client *c;

  if (!s)
    return 0;

  for (c = clients; c; c = c->next)
    {
      if (c->isaside && DO_RAISE_NOTILE (c, s))
	{
	  i++;
	}
    }

  return i;
}

Client *
find_aside (ScreenInfo * s)
{
  Client *c;

  if (!s)
    return (Client *) 0;

  for (c = clients; c; c = c->next)
    {
      if (c->isaside && DO_RAISE_NOTILE (c, s))
	{
	  return c;
	}
    }

  return (Client *) 0;
}

void
arrange_aside (ScreenInfo * s)
{
  int wnum = 0;
  int wcount;
  int ystep;
  Client *c;

  if (!s)
    return;

  wcount = count_aside (s);

  if (wcount < 1)
    {
      return;
    }

  ystep = s->tile_height / wcount;

  for (c = clients; c; c = c->next)
    {
      if (c->isaside && DO_RAISE_NOTILE (c, s))
	{
	  c->y = ystep * wnum++;
	  XMapRaised (dpy, c->parent);
	  XMoveWindow (dpy, c->parent, c->x, c->y);
	}
    }
}

void
move_aside (ScreenInfo * s, Client * c)
{
  if (!s)
    return;

  if (!c || !c->isnotile || c->istool)
    {
      Client *t = find_aside (s);

      if (t)
	{
	  raise_notile (s);
	  XMapRaised (dpy, s->barwin);
	  move_aside (s, t);
	}

      return;
    }

  if (c->isaside)
    {
      c->x = c->ax;
      c->y = c->ay;
      c->isaside = 0;

      if (s->notile_raised[s->desktop] != c->isnotile)
	toggle_notile (s);

      arrange_aside (s);
      active (c);
      raise_tbar (s);
      XMapRaised (dpy, c->parent);
      XMoveWindow (dpy, c->parent, c->x, c->y);
    }
  else
    {
      c->ax = c->x;
      c->ay = c->y;
      c->x = (95 * DisplayWidth (dpy, s->num)) / 100;
      c->isaside = 1;
      arrange_aside (s);
      revert_window (s);
      XMapRaised (dpy, s->barwin);

      if (current)
	XMapRaised (dpy, current->parent);
    }
}

void
move_aside_other (ScreenInfo * s)
{
  Client *c;

  if (!s || !current || !current->isnotile)
    return;

  for (c = clients; c; c = c->next)
    {
      if ((c != current) && !c->isaside && !c->istool
	  && DO_RAISE_NOTILE (c, s))
	{
	  c->ax = c->x;
	  c->ay = c->y;
	  c->x = (95 * DisplayWidth (dpy, s->num)) / 100;
	  c->isaside = 1;
	}
    }

  arrange_aside (s);
  XMapRaised (dpy, s->barwin);
  XMapRaised (dpy, current->parent);
}

void
move_aside_clear (ScreenInfo * s, Client * c)
{
  if (s && c && (c->screen == s) && c->isnotile && normal (c) && !c->isaside)
    move_aside (s, c);
}

void
move_sd (Client * c)
{
  if (!c)
    return;

  c->iszoom = ZOOM_UNZOOMED;

  if (c->isnotile)
    {
      c->isnotile = 0;
      raise_tile (c->screen, 0);
      top (c);
      active (c);
      tile_all (c->screen);
    }
  else
    {
      c->isnotile = 1;
      tile_all (c->screen);
      raise_notile (c->screen);
      move_select (c, 1);
    }
}
