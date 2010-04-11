/* Copyright (c) 1994-1996 David Hogan, see README for licence details */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

Client *hiddenc[MAXHIDDEN];
int numhidden = 0;

void
button (XButtonEvent * e)
{
  Client *c;
  ScreenInfo *s;
  Window dw;

  s = getscreen (e->root);

  if (s == 0)
    return;

  if (e->window == s->barwin)
    {
      if (current && current->screen == s)
	cmapnofocus (s);

      switch (e->button)
	{
	case Button1:
	  if ((e->state & (ShiftMask | ControlMask | Mod1Mask)) ==
	      (ShiftMask | ControlMask | Mod1Mask))
	    {
	      if (current && current->isnotile && (current->screen == s))
		resize_opaque (current);
	    }
	  else if ((e->state & (ControlMask | ShiftMask)) ==
		   (ControlMask | ShiftMask))
	    grow (current, DIM_VERT | DIM_HORIZ, -1);
	  else if ((e->state & (ControlMask | Mod1Mask)) ==
		   (ControlMask | Mod1Mask))
	    move_aside (s, (Client *) 0);
	  else if ((e->state & (ShiftMask | Mod1Mask)) ==
		   (ShiftMask | Mod1Mask))
	    {
	      if (current && current->isnotile && (current->screen == s))
		move_opaque (current);
	    }
	  else if ((e->state & ControlMask) == ControlMask)
	    prev_menu ();
	  else if ((e->state & Mod1Mask) == Mod1Mask)
	    prev_hidden ();
	  else
	    prev_desktop (s);
	  break;
	case Button2:
	  if ((e->state & (ShiftMask | ControlMask | Mod1Mask)) ==
	      (ShiftMask | ControlMask | Mod1Mask))
	    {
	      if (current && (current->screen == s))
		XLowerWindow (dpy, current->parent);
	    }
	  else if ((e->state & (ControlMask | Mod1Mask)) ==
		   (ControlMask | Mod1Mask))
	    move_aside (s, current);
	  else if ((e->state & (ControlMask | ShiftMask)) ==
		   (ControlMask | ShiftMask))
	    move_select (current, 0);
	  else if ((e->state & ControlMask) == ControlMask)
	    do_menu (s);
	  else if ((e->state & Mod1Mask) == Mod1Mask)
	    show_hidden ();
	  else
	    toggle_notile (s);
	  break;
	case Button3:
	  if ((e->state & (ShiftMask | ControlMask | Mod1Mask)) ==
	      (ShiftMask | ControlMask | Mod1Mask))
	    zoom (current, (DIM_VERT | DIM_HORIZ));
	  else if ((e->state & (ControlMask | ShiftMask)) ==
		   (ControlMask | ShiftMask))
	    grow (current, DIM_VERT | DIM_HORIZ, 1);
	  else if ((e->state & (ControlMask | Mod1Mask)) ==
		   (ControlMask | Mod1Mask))
	    move_aside_clear (s, current);
	  else if ((e->state & ControlMask) == ControlMask)
	    next_menu ();
	  else if ((e->state & Mod1Mask) == Mod1Mask)
	    next_hidden ();
	  else
	    next_desktop (s);
	  break;
	case Button4:
	  if ((e->state & (ControlMask | ShiftMask | Mod1Mask)) ==
	      (ControlMask | ShiftMask | Mod1Mask))
	    move_up (current);
	  else if ((e->state & (ControlMask | Mod1Mask)) ==
		   (ControlMask | Mod1Mask))
	    move_aside (s, (Client *) 0);
	  else if ((e->state & (Mod1Mask | ShiftMask)) ==
		   (Mod1Mask | ShiftMask))
	    move_left (current);
	  else if ((e->state & (ControlMask | ShiftMask)) ==
		   (ControlMask | ShiftMask))
	    grow (current, DIM_VERT | DIM_HORIZ, -1);
	  else if ((e->state & ControlMask) == ControlMask)
	    prev_menu ();
	  else if ((e->state & Mod1Mask) == Mod1Mask)
	    prev_hidden ();
	  else if ((e->state & ShiftMask) == ShiftMask)
	    prev_window (s);
	  else
	    prev_desktop (s);
	  break;
	case Button5:
	  if ((e->state & (ControlMask | ShiftMask | Mod1Mask)) ==
	      (ControlMask | ShiftMask | Mod1Mask))
	    move_down (current);
	  else if ((e->state & (ControlMask | Mod1Mask)) ==
		   (ControlMask | Mod1Mask))
	    move_aside_clear (s, current);
	  else if ((e->state & (Mod1Mask | ShiftMask)) ==
		   (Mod1Mask | ShiftMask))
	    move_right (current);
	  else if ((e->state & (ControlMask | ShiftMask)) ==
		   (ControlMask | ShiftMask))
	    grow (current, DIM_VERT | DIM_HORIZ, 1);
	  else if ((e->state & ControlMask) == ControlMask)
	    next_menu ();
	  else if ((e->state & Mod1Mask) == Mod1Mask)
	    next_hidden ();
	  else if ((e->state & ShiftMask) == ShiftMask)
	    next_window (s);
	  else
	    next_desktop (s);
	  break;
	}

      if (current && current->screen == s)
	cmapfocus (current);

      return;
    }

  /* don't let clicks that "bleed" through clients look like they were on the root window. */
  if (!(c = getclient (e->window, 0)) && (e->subwindow != None)
      && (c = getclient (e->subwindow, 0)))
    XTranslateCoordinates (dpy, e->root, e->subwindow, e->x, e->y, &e->x,
			   &e->y, &dw);

  if (current && current->screen == s)
    cmapnofocus (s);

  switch (e->button)
    {
    case Button1:
      if (c && (e->x >= BORDER) && (e->x < (c->dx + BORDER))
	  && (e->y >= BORDER) && (e->y < (c->dy + BORDER)))
	{
	  int wasleft = c->isleft;
	  int wasaside = c->isaside;
	  int ctrl = (e->state & ControlMask) == ControlMask;
	  int skip = s->skip_focus[s->desktop];

	  if (ctrl && !c->isnotile)
	    skip = !skip;

	  if (c->isnotile != s->notile_raised[s->desktop])
	    {
	      if (c->isnotile || s->skip_focus[s->desktop])
		skip = 1;

	      if (!ctrl)
		toggle_notile (s);
	    }

	  if ((!skip && !c->isnotile) || (!ctrl && c->isnotile))
	    {
	      raise_tbar (c->screen);
	      XMapRaised (dpy, c->parent);
	      top (c);
	    }

	  active (c);

	  if (wasaside)
	    {
	      move_aside (c->screen, c);

	      if (ctrl)
		move_aside_other (c->screen);
	    }
	  else if (!skip && !c->isnotile)
	    tile_all (s);

	  if ((skip || wasleft || c->isnotile) && !ctrl && !wasaside &&
	      s->clickthru[s->desktop] && (c->istool || c->isfloat
					   || (c->isnotile ==
					       s->notile_raised[s->desktop])))
	    XAllowEvents (dpy, ReplayPointer, e->time);
	}
      else if (prefs.btnapplication[0])
	spawn (s, prefs.btnapplication[0]);
      break;

    case Button2:
      if (c && (e->x >= BORDER) && (e->x < (c->dx + BORDER))
	  && (e->y >= BORDER) && (e->y < (c->dy + BORDER)))
	{
	  if (s->clickthru[s->desktop] && !c->isaside &&
	      (c->istool || c->isfloat
	       || (c->isnotile == s->notile_raised[s->desktop])))
	    {
	      active (c);
	      XAllowEvents (dpy, ReplayPointer, e->time);
	    }
	}
      else if (prefs.btnapplication[1])
	spawn (s, prefs.btnapplication[1]);
      break;

    case Button3:
      if (c && (e->x >= BORDER) && (e->x < (c->dx + BORDER))
	  && (e->y >= BORDER) && (e->y < (c->dy + BORDER)))
	{
	  if (s->clickthru[s->desktop] && !c->isaside &&
	      (c->istool || c->isfloat
	       || (c->isnotile == s->notile_raised[s->desktop])))
	    {
	      active (c);
	      XAllowEvents (dpy, ReplayPointer, e->time);
	    }
	}
      else if (prefs.btnapplication[2])
	spawn (s, prefs.btnapplication[2]);
      break;

    case Button4:
      if (c && (e->x >= BORDER) && (e->x < (c->dx + BORDER))
	  && (e->y >= BORDER) && (e->y < (c->dy + BORDER)))
	{
	  if (s->clickthru[s->desktop] && !c->isaside &&
	      (c->istool || c->isfloat
	       || (c->isnotile == s->notile_raised[s->desktop])))
	    {
	      active (c);
	      XAllowEvents (dpy, ReplayPointer, e->time);
	    }
	}
      break;

    case Button5:
      if (c && (e->x >= BORDER) && (e->x < (c->dx + BORDER))
	  && (e->y >= BORDER) && (e->y < (c->dy + BORDER)))
	{
	  if (s->clickthru[s->desktop] && !c->isaside &&
	      (c->istool || c->isfloat
	       || (c->isnotile == s->notile_raised[s->desktop])))
	    {
	      active (c);
	      XAllowEvents (dpy, ReplayPointer, e->time);
	    }
	}
      break;
    }

  if (current && current->screen == s)
    cmapfocus (current);
}

void
spawn (ScreenInfo * s, char *p)
{
  /*
   * ugly dance to avoid leaving zombies.  Could use SIGCHLD,
   * but it's not very portable.
   */
  if (fork () == 0)
    {
      if (fork () == 0)
	{
	  close (ConnectionNumber (dpy));

	  if (s->display[0] != '\0')
	    {
	      putenv (s->display);
	    }

	  if (p)
	    {
	      execl (shell, shell, "-c", p, (char *) NULL);
	      fprintf (stderr, "larswm: execl %s", shell);
	      perror (" failed");
	    }
	  else
	    {
	      execlp ("xterm", "xterm", (char *) NULL);
	      fatal ("execlp xterm");
	    }
	}

      exit (0);
    }

  wait ((int *) 0);
}

int
is_switchclass (Client * c, char *sc)
{
  int i;
  char *inst;

  if (!sc)
    return 0;
  
  if (!c->class)
    return 0;

  if (strcmp (c->class, sc) == 0)
    return 1;
  
  if (!c->instance)
    return 0;
  if (strcmp (c->instance, sc) == 0)
    return 1;

  if ((inst = strchr (sc, '~')))
    {
      inst++;
      if (strncmp (c->class, sc, strlen (c->class)) == 0)
	if (strncmp (c->instance, inst, strlen (c->instance)) == 0)
          return 1;
    }
    
  return 0;
}

void
window_of_class (ScreenInfo *s, char *sc, char *p)
{
  Client *c, *last = 0;
  for (c = clients; c; c = c->next)
    {
      if (is_switchclass(c, sc))
        last = c;
    }
  if (last)
    {
      goto_desktop (s, last->desktop);
      raise_tbar (s);
      
      if (last->isnotile) {
	if (!s->notile_raised[s->desktop])
	  raise_notile (last->screen);
	top (last);
	XMapRaised (dpy, last->parent);
        active (last);
      } else {
	if (s->notile_raised[s->desktop])
	  raise_tile (last->screen, 0);
	top (last);
	XMapRaised (dpy, last->parent);
        tile_all (s);
      }
    } else {
        if (p)
          spawn (s, p);
    }
}

void
wmdelete (Client * c, int shift)
{
  if (c == 0)
    return;
  if ((c->proto & Pdelete) && !shift)
    sendcmessage (c->window, wm_protocols, wm_delete, 0);
  else
    XKillClient (dpy, c->window);	/* let event clean up */
}

void
hide (Client * c)
{
  if (c == 0 || numhidden == MAXHIDDEN)
    return;

  if (hidden (c))
    return;

  XUnmapWindow (dpy, c->parent);
  XUnmapWindow (dpy, c->window);
  wmsetstate (c, IconicState);

  if (c == current)
    {
      revert_window (c->screen);
    }

  hiddenc[numhidden] = c;
  numhidden++;
  c->hidden = 1;
  bar_hidden = numhidden;

  update_tbar ();
}

void
unhide (int n, int map)
{
  Client *c;
  int i;

  if (n >= numhidden)
    return;

  c = hiddenc[n];

  if (!hidden (c))
    return;

  if (map)
    {
      c->desktop = c->screen->desktop;
      raise_tbar (c->screen);
      XMapWindow (dpy, c->window);
      XMapRaised (dpy, c->parent);
      wmsetstate (c, NormalState);
      top (c);

      active (c);
    }

  numhidden--;

  for (i = n; i < numhidden; i++)
    hiddenc[i] = hiddenc[i + 1];

  c->hidden = 0;
  bar_hidden = numhidden;

  if (map && (c->isnotile != c->screen->notile_raised[c->screen->desktop]))
    toggle_notile (c->screen);

  update_tbar ();
}

void
unhidec (Client * c, int map)
{
  int i;

  if (!c->hidden)
    return;

  for (i = 0; i < numhidden; i++)
    if (c == hiddenc[i])
      {
	unhide (i, map);
	return;
      }
}

void
renamec (Client * c, char *name)
{
  if (name == 0)
    name = "???";

  c->label = name;
}
