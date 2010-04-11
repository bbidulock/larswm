/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

void
unhide_last (ScreenInfo * s)
{
  int i;

  for (i = numhidden - 1; i >= 0; i--)
    {
      Client *c = hiddenc[i];

      if (c->screen == s)
	{
	  unhidec (c, 1);

	  if (!c->isnotile)
	    tile_all (s);

	  return;
	}
    }
}

void
initkeys (Client * c)
{
  if (c)
    {
      int y;

      /* only do this if there's a window to grab keys from */
      if (prefs.prev_window_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.prev_window_key),
		  prefs.prev_window_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.next_window_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.next_window_key),
		  prefs.next_window_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_northwest_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_northwest_key),
		  prefs.move_northwest_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_north_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_north_key),
		  prefs.move_north_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_northeast_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_northeast_key),
		  prefs.move_northeast_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_west_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_west_key),
		  prefs.move_west_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_center_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_center_key),
		  prefs.move_center_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_east_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_east_key),
		  prefs.move_east_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_southwest_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_southwest_key),
		  prefs.move_southwest_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_south_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_south_key),
		  prefs.move_south_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      if (prefs.move_southeast_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_southeast_key),
		  prefs.move_southeast_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_up_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_up_key),
		  prefs.move_up_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_down_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_down_key),
		  prefs.move_down_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_left_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_left_key),
		  prefs.move_left_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_right_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_right_key),
		  prefs.move_right_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_select_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_select_key),
		  prefs.move_select_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_aside_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_aside_key),
		  prefs.move_aside_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_aside_other_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_aside_other_key),
		  prefs.move_aside_other_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_sd_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_sd_key),
		  prefs.move_sd_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.grow_vert_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.grow_vert_key),
		  prefs.grow_vert_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.shrink_vert_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.shrink_vert_key),
		  prefs.shrink_vert_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.grow_horiz_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.grow_horiz_key),
		  prefs.grow_horiz_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.shrink_horiz_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.shrink_horiz_key),
		  prefs.shrink_horiz_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.grow_both_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.grow_both_key),
		  prefs.grow_both_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.shrink_both_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.shrink_both_key),
		  prefs.shrink_both_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.zoom_vert_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.zoom_vert_key),
		  prefs.zoom_vert_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.zoom_horiz_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.zoom_horiz_key),
		  prefs.zoom_horiz_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.zoom_full_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.zoom_full_key),
		  prefs.zoom_full_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.zoom_full2_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.zoom_full2_key),
		  prefs.zoom_full2_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.hide_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.hide_key), prefs.hide_mod,
		  c->parent, True, GrabModeAsync, GrabModeAsync);

      if (prefs.close_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.close_key),
		  prefs.close_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_next_desktop_key)
        XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_next_desktop_key),
		  prefs.move_next_desktop_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.move_prev_desktop_key)
        XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_prev_desktop_key),
		  prefs.move_prev_desktop_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      /* same as root window grab */
      if (num_screens > 1)
	{
	  if (prefs.prev_screen_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.prev_screen_key),
		      prefs.prev_screen_mod, c->parent, True, GrabModeAsync,
		      GrabModeAsync);
	  if (prefs.next_screen_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.next_screen_key),
		      prefs.next_screen_mod, c->parent, True, GrabModeAsync,
		      GrabModeAsync);
	}

      if (prefs.prev_desktop_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.prev_desktop_key),
		  prefs.prev_desktop_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.prev_desktop_drag_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.prev_desktop_drag_key),
		  prefs.prev_desktop_drag_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.next_desktop_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.next_desktop_key),
		  prefs.next_desktop_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.next_desktop_drag_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.next_desktop_drag_key),
		  prefs.next_desktop_drag_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.toggle_inc_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.toggle_inc_key),
		  prefs.toggle_inc_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.raise_notile_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.raise_notile_key),
		  prefs.raise_notile_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.unhide_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.unhide_key),
		  prefs.unhide_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.two_on_left_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.two_on_left_key),
		  prefs.two_on_left_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.two_on_left_shrink_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.two_on_left_shrink_key),
		  prefs.two_on_left_shrink_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.two_on_left_grow_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.two_on_left_grow_key),
		  prefs.two_on_left_grow_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);

      if (prefs.toggle_skip_focus_key)
	XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.toggle_skip_focus_key),
		  prefs.toggle_skip_focus_mod, c->parent, True, GrabModeAsync,
		  GrabModeAsync);
      
      for (y = 0; y < prefs.desktops; y++)
	if (prefs.goto_desktop_key[y])
	  XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.goto_desktop_key[y]),
		    prefs.goto_desktop_mod[y], c->parent, True, GrabModeAsync,
		    GrabModeAsync);

      for (y = 0; y < MAXAPPS; y++)
	if (prefs.application[y] && prefs.application_key[y])
	  XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.application_key[y]),
		    prefs.application_mod[y], c->parent, True, GrabModeAsync,
		    GrabModeAsync);

      for (y = 0; y < MAXSWITCHCLASSES; y++)
	if (prefs.switchclass[y] && prefs.switchclass_key[y])
	  XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.switchclass_key[y]),
		    prefs.switchclass_mod[y], c->parent, True, GrabModeAsync,
		    GrabModeAsync);

      for (y = 0; y < prefs.desktops; y++)
        if (prefs.move_desktop_key[y])
          XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_desktop_key[y]),
		    prefs.move_desktop_mod[y], c->parent, True, GrabModeAsync,
		    GrabModeAsync);
    }
  else
    {
      int i;

      XUngrabKeyboard (dpy, CurrentTime);

      for (i = 0; i < num_screens; i++)
	{
	  int y;

	  if (num_screens > 1)
	    {
	      if (prefs.prev_screen_key)
		XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.prev_screen_key),
			  prefs.prev_screen_mod, screens[i].root, True,
			  GrabModeAsync, GrabModeAsync);
	      if (prefs.next_screen_key)
		XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.next_screen_key),
			  prefs.next_screen_mod, screens[i].root, True,
			  GrabModeAsync, GrabModeAsync);
	    }

	  if (prefs.prev_desktop_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.prev_desktop_key),
		      prefs.prev_desktop_mod, screens[i].root, True,
		      GrabModeAsync, GrabModeAsync);
	  if (prefs.next_desktop_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.next_desktop_key),
		      prefs.next_desktop_mod, screens[i].root, True,
		      GrabModeAsync, GrabModeAsync);

	  if (prefs.toggle_inc_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.toggle_inc_key),
		      prefs.toggle_inc_mod, screens[i].root, True,
		      GrabModeAsync, GrabModeAsync);

	  if (prefs.raise_notile_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.raise_notile_key),
		      prefs.raise_notile_mod, screens[i].root, True,
		      GrabModeAsync, GrabModeAsync);

	  if (prefs.unhide_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.unhide_key),
		      prefs.unhide_mod, screens[i].root, True, GrabModeAsync,
		      GrabModeAsync);

	  if (prefs.toggle_skip_focus_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.toggle_skip_focus_key),
		      prefs.toggle_skip_focus_mod, screens[i].root, True, GrabModeAsync,
		      GrabModeAsync);

	  if (prefs.move_aside_key)
	    XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.move_aside_key),
		      prefs.move_aside_mod, screens[i].root, True,
		      GrabModeAsync, GrabModeAsync);

	  for (y = 0; y < prefs.desktops; y++)
	    if (prefs.goto_desktop_key[y])
	      XGrabKey (dpy,
			XKeysymToKeycode (dpy, prefs.goto_desktop_key[y]),
			prefs.goto_desktop_mod[y], screens[i].root, True,
			GrabModeAsync, GrabModeAsync);

	  for (y = 0; y < MAXAPPS; y++)
	    if (prefs.application[y] && prefs.application_key[y])
	      XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.application_key[y]),
			prefs.application_mod[y], screens[i].root, True,
			GrabModeAsync, GrabModeAsync);

	  for (y = 0; y < MAXSWITCHCLASSES; y++)
	    if (prefs.switchclass[y] && prefs.switchclass_key[y])
	      XGrabKey (dpy, XKeysymToKeycode (dpy, prefs.switchclass_key[y]),
			prefs.switchclass_mod[y], screens[i].root, True,
			GrabModeAsync, GrabModeAsync);

	}
    }
}

void
keyevent (XKeyEvent * e)
{
  Client *c;
  ScreenInfo *s;
  KeySym k;
  unsigned long m;

  if ((s = getscreen (e->root)) == 0)
    return;

  c = getclient (e->window, 0);

  k = XKeycodeToKeysym (dpy, e->keycode, 0);
  m = e->state;

  if (k == prefs.prev_screen_key && m == prefs.prev_screen_mod)
    prev_screen (s);
  else if (k == prefs.next_screen_key && m == prefs.next_screen_mod)
    next_screen (s);
  else if (k == prefs.prev_desktop_key && m == prefs.prev_desktop_mod)
    prev_desktop (s);
  else if (k == prefs.prev_desktop_drag_key
	   && m == prefs.prev_desktop_drag_mod)
    prev_desktop_drag (s);
  else if (k == prefs.next_desktop_key && m == prefs.next_desktop_mod)
    next_desktop (s);
  else if (k == prefs.next_desktop_drag_key
	   && m == prefs.next_desktop_drag_mod)
    next_desktop_drag (s);
  else if (k == prefs.prev_window_key && m == prefs.prev_window_mod)
    prev_window (s);
  else if (k == prefs.next_window_key && m == prefs.next_window_mod)
    next_window (s);
  else if (k == prefs.raise_notile_key && m == prefs.raise_notile_mod)
    toggle_notile (s);
  else if (k == prefs.move_northwest_key && m == prefs.move_northwest_mod)
    move_northwest (current);
  else if (k == prefs.move_north_key && m == prefs.move_north_mod)
    move_north (current);
  else if (k == prefs.move_northeast_key && m == prefs.move_northeast_mod)
    move_northeast (current);
  else if (k == prefs.move_west_key && m == prefs.move_west_mod)
    move_west (current);
  else if (k == prefs.move_center_key && m == prefs.move_center_mod)
    move_center (current);
  else if (k == prefs.move_east_key && m == prefs.move_east_mod)
    move_east (current);
  else if (k == prefs.move_southwest_key && m == prefs.move_southwest_mod)
    move_southwest (current);
  else if (k == prefs.move_south_key && m == prefs.move_south_mod)
    move_south (current);
  else if (k == prefs.move_southeast_key && m == prefs.move_southeast_mod)
    move_southeast (current);
  else if (k == prefs.toggle_inc_key && m == prefs.toggle_inc_mod)
    toggle_inc (s);
  else if (k == prefs.move_up_key && m == prefs.move_up_mod)
    move_up (current);
  else if (k == prefs.move_down_key && m == prefs.move_down_mod)
    move_down (current);
  else if (k == prefs.move_left_key && m == prefs.move_left_mod)
    move_left (current);
  else if (k == prefs.move_right_key && m == prefs.move_right_mod)
    move_right (current);
  else if (k == prefs.move_select_key && m == prefs.move_select_mod)
    move_select (current, 0);
  else if (k == prefs.move_aside_key && m == prefs.move_aside_mod)
    move_aside (s, current);
  else if (k == prefs.move_aside_other_key && m == prefs.move_aside_other_mod)
    move_aside_other (s);
  else if (k == prefs.move_sd_key && m == prefs.move_sd_mod)
    move_sd (current);
  else if (k == prefs.grow_vert_key && m == prefs.grow_vert_mod)
    grow (current, DIM_VERT, 1);
  else if (k == prefs.shrink_vert_key && m == prefs.shrink_vert_mod)
    grow (current, DIM_VERT, -1);
  else if (k == prefs.grow_horiz_key && m == prefs.grow_horiz_mod)
    grow (current, DIM_HORIZ, 1);
  else if (k == prefs.shrink_horiz_key && m == prefs.shrink_horiz_mod)
    grow (current, DIM_HORIZ, -1);
  else if (k == prefs.grow_both_key && m == prefs.grow_both_mod)
    grow (current, DIM_VERT | DIM_HORIZ, 1);
  else if (k == prefs.shrink_both_key && m == prefs.shrink_both_mod)
    grow (current, DIM_VERT | DIM_HORIZ, -1);
  else if (k == prefs.zoom_vert_key && m == prefs.zoom_vert_mod)
    zoom (current, DIM_VERT);
  else if (k == prefs.zoom_horiz_key && m == prefs.zoom_horiz_mod)
    zoom (current, DIM_HORIZ);
  else if (k == prefs.zoom_full_key && m == prefs.zoom_full_mod)
    zoom (current, (DIM_VERT | DIM_HORIZ));
  else if (k == prefs.zoom_full2_key && m == prefs.zoom_full2_mod)
    zoom (current, (DIM_VERT | DIM_HORIZ | DIM_OUTSIDE));
  else if (k == prefs.hide_key && m == prefs.hide_mod)
    hide (current);
  else if (k == prefs.unhide_key && m == prefs.unhide_mod)
    unhide_last (s);
  else if (k == prefs.close_key && m == prefs.close_mod)
    wmdelete (current, 0);
  else if (k == prefs.toggle_skip_focus_key && m == prefs.toggle_skip_focus_mod)
    toggle_skip_focus (s);
  else if (k == prefs.move_next_desktop_key && m == prefs.move_next_desktop_mod)
    move_other_desktop(current, -1);
  else if (k == prefs.move_prev_desktop_key && m == prefs.move_prev_desktop_mod)
    move_other_desktop(current, -2);
  else if (k == prefs.two_on_left_key && m == prefs.two_on_left_mod)
    two_on_left (s);
  else if (k == prefs.two_on_left_shrink_key && m == prefs.two_on_left_shrink_mod)
    two_on_left_grow (s, -VERT_GROW);
  else if (k == prefs.two_on_left_grow_key && m == prefs.two_on_left_grow_mod)
    two_on_left_grow (s, VERT_GROW);
  else
    {
      int i;

      for (i = 0; i < prefs.desktops; i++)
	{
	  if (k == prefs.goto_desktop_key[i]
	      && m == prefs.goto_desktop_mod[i])
	    {
	      goto_desktop (s, i);
	      return;
	    }
	}

      for (i = 0; i < MAXAPPS; i++)
	{
	  if (prefs.application[i] && (k == prefs.application_key[i])
	      && (m == prefs.application_mod[i]))
	    {
	      spawn (s, prefs.application[i]);
	      return;
	    }
	}

      for (i = 0; i < MAXSWITCHCLASSES; i++)
        {
	  if (prefs.switchclass[i] && (k == prefs.switchclass_key[i])
	      && (m == prefs.switchclass_mod[i]))
	    {
	      window_of_class (s, prefs.switchclass[i], prefs.switchclass_app[i]);
	      return;
	    }
	}

      for (i = 0; i < prefs.desktops; i++)
        {
          if (k == prefs.move_desktop_key[i] && m == prefs.move_desktop_mod[i])
	    {
              move_other_desktop (current, i);
	      return;
	    }
	}

      XAllowEvents (dpy, ReplayKeyboard, e->time);
    }
}
