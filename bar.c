/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"

#define MSGSIZE 100

static char cmd_output[MSGSIZE + 1];
static char *cmd_output_ptr = 0;
int bar_hidden = 0;

#define MENU_BASE -10
#define MENU_ENTRIES 6

static char *menu[] = {
  "Menu [1/6]  Toggle clickthru    (Desktop)",
  "Menu [2/6]  Toggle tile_resize  (Desktop)",
  "Menu [3/6]  Toggle skip_focus   (Desktop)",
  "Menu [4/6]  Toggle floatclass    (Window)",
  "Menu [5/6]  Toggle toolclass     (Window)",
  "Menu [6/6]  Toggle stickyclass   (Window)",
};

/* Make sure all menu entries really are this length! */
#define MLEN 41

void
do_menu (ScreenInfo * s)
{
  if ((bar_hidden >= 0) && (numhidden > 0) && (bar_hidden < numhidden))
    {
      bar_hidden = numhidden;
      update_tbar ();
    }
  else
    switch (-bar_hidden + MENU_BASE)
      {
      case 0:
	toggle_clickthru (s);
	break;
      case 1:
	toggle_tile_resize (s);
	break;
      case 2:
	toggle_skip_focus (s);
	break;
      case 3:
	toggle_isfloat (current);
	break;
      case 4:
	toggle_istool (current);
	break;
      case 5:
	toggle_issticky (current);
	break;
      }
}

void
prev_menu (void)
{
  if ((bar_hidden >= 0) && (numhidden > 0) && (bar_hidden < numhidden))
    {
      bar_hidden = numhidden;
    }
  else
    {
      bar_hidden++;

      if ((bar_hidden > MENU_BASE) && (bar_hidden < numhidden))
	bar_hidden = numhidden;
      else if (bar_hidden > MENU_BASE)
	bar_hidden = MENU_BASE - (MENU_ENTRIES - 1);
    }

  update_tbar ();
}

void
next_menu (void)
{
  if ((bar_hidden >= 0) && (numhidden > 0) && (bar_hidden < numhidden))
    {
      bar_hidden = numhidden;
    }
  else
    {
      bar_hidden--;

      if (bar_hidden > MENU_BASE)
	bar_hidden = MENU_BASE;
      else if (bar_hidden < (MENU_BASE - (MENU_ENTRIES - 1)))
	bar_hidden = numhidden;
    }

  update_tbar ();
}

void
show_hidden (void)
{
  if (bar_hidden < 0)
    {
      bar_hidden = numhidden;
      update_tbar ();
    }
  else if ((bar_hidden >= 0) && (numhidden > 0) && (bar_hidden < numhidden))
    {
      Client *c = hiddenc[bar_hidden];

      unhide (bar_hidden, 1);

      if (!c->isnotile)
	tile_all (c->screen);
    }
  else if (current)
    {
      hide (current);
    }
}

void
prev_hidden (void)
{
  bar_hidden--;

  if (bar_hidden < 0)
    bar_hidden = numhidden;

  update_tbar ();
}

void
next_hidden (void)
{
  bar_hidden++;

  if (bar_hidden < 0)
    bar_hidden = numhidden;
  else if (bar_hidden > numhidden)
    bar_hidden = 0;

  update_tbar ();
}

void
update_cmd_output (void)
{
  XTextProperty pr;

  cmd_output_ptr = 0;

  if (XGetTextProperty (dpy, DefaultRootWindow (dpy), &pr, bartext_larswm))
    {
      if (pr.nitems > 0)
	{
	  strncpy (cmd_output, (char *) pr.value, MSGSIZE);
	  cmd_output[MSGSIZE] = '\0';
	  cmd_output_ptr = cmd_output;
	}

      XFree (pr.value);
    }

  update_tbar ();
  XSync (dpy, False);
}

void
draw_tbar (ScreenInfo * s)
{
  int tlen, max_tlen;
  char sbar[256], tmp[256];

  if (!s)
    return;

  XClearWindow (dpy, s->barwin);
#ifdef THREE_D
  XDrawLine (dpy, s->barwin, s->botwhitegc, 0, BAR_HEIGHT - 1,
	     BAR_WIDTH (s) - 1, BAR_HEIGHT - 1);
  XDrawLine (dpy, s->barwin, s->botwhitegc, BAR_WIDTH (s) - 1, BAR_HEIGHT - 1,
	     BAR_WIDTH (s) - 1, 0);
  XDrawLine (dpy, s->barwin, s->topwhitegc, 0, 0, BAR_WIDTH (s) - 1, 0);
  XDrawLine (dpy, s->barwin, s->topwhitegc, 0, 0, 0, BAR_HEIGHT - 1);
#endif

  sbar[0] = '\0';

  if (current && (current->screen == s)
      && !((bar_hidden >= 0) && (bar_hidden < numhidden)))
    {
      strcpy (sbar, "(");

      if (current->class)
	strcat (sbar, current->class);

      strcat (sbar, "~");

      if (current->instance)
	strcat (sbar, current->instance);

      strcat (sbar, ")  [");

      tmp[0] = current->isnotile ? 'n' : '-';
      tmp[1] = current->isfloat ? 'f' : '-';
      tmp[2] = current->istool ? 't' : '-';
      tmp[3] = current->issticky ? 's' : '-';
      tmp[4] = current->iszoom ? 'z' : '-';
      tmp[5] = ']';
      tmp[6] = ' ';
      tmp[7] = ' ';
      tmp[8] = '\0';

      strcat (sbar, tmp);
    }

  if (!
      ((bar_hidden <= MENU_BASE)
       || ((bar_hidden >= 0) && (bar_hidden < numhidden))))
    {
      strcat (sbar, prefs.dtname[s->num][s->desktop]);
      strcat (sbar, "  ");
    }

  tmp[0] = '[';
  tmp[1] = s->notile_raised[s->desktop] ? 'U' : 'T';
  tmp[2] = s->clickthru[s->desktop] ? 'C' : '-';
  tmp[3] = s->tile_resize[s->desktop] ? 'R' : '-';
  tmp[4] = s->skip_focus[s->desktop] ? 'S' : '-';
  tmp[5] = s->bigmr[s->desktop] ? 'B' : '-';
  tmp[6] = numhidden ? 'H' : '-';
  tmp[7] = ']';
  tmp[8] = '\0';

  strcat (sbar, tmp);

  if (!
      ((bar_hidden <= MENU_BASE)
       || ((bar_hidden >= 0) && (bar_hidden < numhidden))))
    {
      if (cmd_output_ptr)
	{
	  strcat (sbar, "  ");
	  strcat (sbar, cmd_output);
	}
    }

  tlen = strlen (sbar);

  if (tlen > 0)
    {
      int plen = XTextWidth (font, sbar, tlen);
      XDrawString (dpy, s->barwin, s->gc, BAR_WIDTH (s) - plen - BAR_HEIGHT,
		   BAR_TEXT_Y, sbar, tlen);

      /* space left on left */
      max_tlen = BAR_WIDTH (s) - plen - (3 * BAR_HEIGHT);
      if (max_tlen < 1)
	max_tlen = 1;
    }

  /* print current window's label */
  if (bar_hidden <= MENU_BASE)
    {
      XDrawString (dpy, s->barwin, s->gc, BAR_HEIGHT, BAR_TEXT_Y,
		   menu[-bar_hidden + MENU_BASE], MLEN);
    }
  else if ((bar_hidden >= 0) && (bar_hidden < numhidden))
    {
      Client *c = hiddenc[bar_hidden];
      unsigned short int changed = 0;

      if (c->screen == s)
	sprintf (tmp, "Hidden [%d/%d]  %s", bar_hidden + 1, numhidden,
		 c->iconname ? c->iconname : c->label);
      else
	sprintf (tmp, "Other Screen [%d/%d]  %s", bar_hidden + 1, numhidden,
		 c->iconname ? c->iconname : c->label);

      tlen = strlen (tmp);

      while (tlen > 0 && XTextWidth (font, tmp, tlen) > max_tlen)
	{
	  tlen--;
	  changed = 1;
	}

      if (changed && tlen > 10)
	{
	  tmp[tlen - 3] = '\0';
	  strcat (tmp, "...");
	}

      if (tlen > 0)
	{
	  XDrawString (dpy, s->barwin, s->gc, BAR_HEIGHT, BAR_TEXT_Y, tmp,
		       tlen);
	}
    }
  else if (current && current->label && (current->screen == s))
    {
      unsigned short int changed = 0;

      strcpy (tmp, current->label);
      tlen = strlen (tmp);

      while (tlen > 0 && XTextWidth (font, tmp, tlen) > max_tlen)
	{
	  tlen--;
	  changed = 1;
	}

      if (changed && tlen > 10)
	{
	  tmp[tlen - 3] = '\0';
	  strcat (tmp, "...");
	}

      if (tlen > 0)
	{
	  XDrawString (dpy, s->barwin, s->gc, BAR_HEIGHT, BAR_TEXT_Y, tmp,
		       tlen);
	}
    }
}

void
update_tbar (void)
{
  int i;

  for (i = 0; i < num_screens; i++)
    draw_tbar (&screens[i]);
}

void
raise_tbar (ScreenInfo * s)
{
  Client *c;

  if (!s)
    return;

  for (c = clients; c; c = c->next)
    if ((c->isfloat || c->isaside) && normal (c) &&
	(c->screen == s) &&
	(c->issticky || (c->desktop == c->screen->desktop)))
      XMapRaised (dpy, c->parent);

  XMapRaised (dpy, s->barwin);
}

ScreenInfo *
getbarscreen (Window w)
{
  int i;

  for (i = 0; i < num_screens; i++)
    {
      if (screens[i].barwin == w)
	return &screens[i];
    }

  return 0;
}
