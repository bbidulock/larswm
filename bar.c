/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
#define MENU_ENTRIES 7

static char *menu[] = {
  "Menu [1/7]  Toggle clickthru    (Desktop)",
  "Menu [2/7]  Toggle tile_resize  (Desktop)",
  "Menu [3/7]  Toggle show_bottom  (Desktop)",
  "Menu [4/7]  Toggle skip_focus   (Desktop)",
  "Menu [5/7]  Toggle floatclass    (Window)",
  "Menu [6/7]  Toggle toolclass     (Window)",
  "Menu [7/7]  Toggle stickyclass   (Window)",
};

/* Make sure all menu entries really are this length! */
#define MLEN 41

/*
 * buffer_append - append to a fixed sized buffer in a snprintf like fassion.
 *
 * Arguments:
 * - buf_ptr    Pointer to a character pointer. The character pointer will be
 *              modified to is points to the null-byte after this function
 *              returns.
 * - buf_len    Pointer to an integer. Number of bytes left in the buffer. At
 *              most this many minus one bytes will be written to the buffer.
 *              The buffer is assured to end with a null-byte.
 * - format     See printf(3)
 * - ...        See printf(3)
 *
 * Return value:
 * Number of bytes written to the buffer, not including the trailing null-byte.
 * If an error occured a negative value will be returned.
 */
int buffer_append (char **buf_ptr, int *buf_len, const char *format, ...)
{
  int len;
  va_list args;

  if (*buf_len <= 1)
    return (0);

  va_start (args, format);
  len = vsnprintf (*buf_ptr, *buf_len, format, args);
  va_end (args);

  if (len >= *buf_len)
    {
      fprintf (stderr, "[larswm] Warning: Buffer text was truncated.\n");
      len = *buf_len - 1;
      *buf_ptr[len] = '\0';
    }
  else if (len < 0)
    {
      fprintf (stderr, "[larswm] Error: `buffer_append' failed.\n");
      return (len);
    }

  *buf_ptr += len;
  *buf_len -= len;

  return (len);
}

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
        toggle_show_bottom (s);
	break;
      case 3:
        toggle_skip_focus (s);
        break;
      case 5:
	toggle_isfloat (current);
	break;
      case 6:
	toggle_istool (current);
	break;
      case 7:
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

void draw_tbar (ScreenInfo *s)
{
  /*
   * The bar contains two parts:
   * bartext_status  - Misc. information (right aligned)
   * bartext_winname - The window's name
   */
  char  bartext_status[1024];
  char *bartext_status_ptr;
  int   bartext_status_len;

  char       bartext_winname[1024];
  int        bartext_winname_len;
  static int bartext_winname_len_max[MAXSCREENS];
  int        bartext_winname_width_max;

  bartext_status_len = sizeof (bartext_status);
  bartext_status_ptr = &bartext_status[0];
  bartext_status_ptr[bartext_status_len - 1] = '\0';

  bartext_winname_len = sizeof (bartext_winname);
  bartext_winname[0] = '\0';

  /* Initialize the `static' variable on the first run */
  if (bartext_winname_len_max[0] == 0)
    {
      int i;

      for (i = 0; i < MAXSCREENS; i++)
        bartext_winname_len_max[i] = bartext_winname_len;
    }

  if (!s)
    return;

  XClearWindow (dpy, s->barwin);
#ifdef THREE_D
  XDrawLine (dpy, s->barwin, s->botwhitegc, 0, prefs.bar_height - 1,
	     BAR_WIDTH (s) - 1, prefs.bar_height - 1);
  XDrawLine (dpy, s->barwin, s->botwhitegc, BAR_WIDTH (s) - 1, prefs.bar_height - 1,
	     BAR_WIDTH (s) - 1, 0);
  XDrawLine (dpy, s->barwin, s->topwhitegc, 0, 0, BAR_WIDTH (s) - 1, 0);
  XDrawLine (dpy, s->barwin, s->topwhitegc, 0, 0, 0, prefs.bar_height - 1);
#endif

  /*
   * When this screen contains the active window, append
   * ``(class~instance) [nftsz]  '' to the left
   */
  if (current && (current->screen == s)
      && !((bar_hidden >= 0) && (bar_hidden < numhidden)))
    {
      buffer_append (&bartext_status_ptr, &bartext_status_len, "(%s~%s)  [%c%c%c%c%c]  ",
          current->class != NULL ? current->class : "[no class]",
          current->instance != NULL ? current->instance : "[no instance]",
          current->isnotile ? 'n' : '-',
          current->isfloat ? 'f' : '-',
          current->istool ? 't' : '-',
          current->issticky ? 's' : '-',
          current->iszoom ? 'z' : '-');
    }

  /*
   * Add desktop name, unless menu if selected.
   */
  if (!((bar_hidden <= MENU_BASE) || ((bar_hidden >= 0) && (bar_hidden < numhidden))))
    {
      buffer_append (&bartext_status_ptr, &bartext_status_len, "%s  ",
          prefs.dtname[s->num][s->desktop]);
    }

  buffer_append (&bartext_status_ptr, &bartext_status_len, "[%c%c%c%c%c%c%c]",
      s->notile_raised[s->desktop] ? 'U' : 'T',
      s->clickthru[s->desktop] ? 'C' : '-',
      s->tile_resize[s->desktop] ? 'R' : '-',
      s->skip_focus[s->desktop] ? 'S' : '-',
      s->bigmr[s->desktop] ? 'B' : '-',
      s->tile_show_bottom[s->desktop] ? '_' : '-',
      numhidden ? 'H' : '-');

  /*
   * Add `cmd_output' (e.g. `larsclock') unless menu selected.
   */
  if (!((bar_hidden <= MENU_BASE) || ((bar_hidden >= 0) && (bar_hidden < numhidden))))
    {
      buffer_append (&bartext_status_ptr, &bartext_status_len, "  %s",
          cmd_output);
    }

  bartext_winname_width_max = BAR_WIDTH (s) - (2 * prefs.bar_height);
  if (sizeof (bartext_status) != bartext_status_len)
    {
      int length_string;
      int width_pixels;

      length_string = strlen (bartext_status);
      width_pixels  = XTextWidth (font, bartext_status, length_string);

      XDrawString (dpy, s->barwin, s->gc, BAR_WIDTH (s) - width_pixels - prefs.bar_height,
          BAR_TEXT_Y, bartext_status, length_string);

      bartext_winname_width_max -= (width_pixels + prefs.bar_height);
    }

  /*
   * There's no point in doing more work, if there's no space to print the
   * result to..
   */
  if (bartext_winname_width_max <= 0)
    return;

  /*
   * Print the menu entry
   */
  if (bar_hidden <= MENU_BASE)
    {
      XDrawString (dpy, s->barwin, s->gc, prefs.bar_height, BAR_TEXT_Y,
		   menu[-bar_hidden + MENU_BASE], MLEN);
      return;
    }
  else if (((bar_hidden >= 0) && (bar_hidden < numhidden))
      || ((current && current->label) && (current->screen == s)))
    {
      int changed = 0;
      int len;

      if ((bar_hidden >= 0) && (bar_hidden < numhidden))
        {
          Client *c = hiddenc[bar_hidden];

          if (c->screen == s)
            len = snprintf (bartext_winname, bartext_winname_len, "Hidden [%d/%d]  %s",
                bar_hidden + 1, numhidden, c->iconname ? c->iconname : c->label);
          else
            len = snprintf (bartext_winname, bartext_winname_len, "Other Screen [%d/%d]  %s",
                bar_hidden + 1, numhidden, c->iconname ? c->iconname : c->label);
        }
      else if (current && current->label && (current->screen == s))
        {
          len = snprintf (bartext_winname, bartext_winname_len, "%s",
              current->label);
        }

      if (len >= bartext_winname_len)
        {
          len = bartext_winname_len - 1;
          bartext_winname[len] = '\0';
        }

      if (len > bartext_winname_len_max[s->num])
        {
          len = bartext_winname_len_max[s->num];
          changed = 1;
        }

      while ((len > 0)
          && (XTextWidth (font, bartext_winname, len) > bartext_winname_width_max))
        {
          len--;
          changed = 1;
        }

      if (changed)
        bartext_winname_len_max[s->num] = len;

      if (len <= 0)
        return;

      bartext_winname[len] = '\0';
      if (changed && len > 10)
	{
          bartext_winname[len - 3] = '.';
          bartext_winname[len - 2] = '.';
          bartext_winname[len - 1] = '.';
	}

      XDrawString (dpy, s->barwin, s->gc, prefs.bar_height, BAR_TEXT_Y, bartext_winname, len);
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

/*
 * vim:shiftwidth=2:smarttab:expandtab
 */
