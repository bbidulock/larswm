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


void
mainloop (int shape_event)
{
  Client *c = 0;
  ScreenInfo *s = 0;
  XEvent ev;

  for (;;)
    {
      XNextEvent (dpy, &ev);

      switch (ev.type)
	{
	case KeyPress:
	  keyevent (&ev.xkey);
	  break;
	case KeyRelease:
	  break;
	case ButtonPress:
	  button (&ev.xbutton);
	  XAllowEvents (dpy, SyncPointer, ev.xbutton.time);
	  break;
	case ButtonRelease:
	  break;
	case MapRequest:
	  mapreq (&ev.xmaprequest);

	  if ((c = getclient (ev.xmaprequest.window, 0)) != 0)
	    {
	      if (normal (c) && ((s = c->screen) != 0))
		{
		  if (c == current)
		    {
		      if (c->isnotile)
			{
			  raise_notile (s);
			}
		      else
			{
			  raise_tile (s, 0);
			}
		    }

		  if (!c->isnotile || c->istool)
		    {
		      tile_all (s);
		    }
		}
	    }
	  break;
	case ConfigureRequest:
	  configurereq (&ev.xconfigurerequest);
	  break;
	case UnmapNotify:
	  unmap (&ev.xunmap);

	  if ((c = getclient (ev.xunmap.window, 0)) != 0)
	    {
	      if ((s = c->screen) != 0)
		{
		  if (!c->isnotile || c->istool)
		    {
		      tile_all (s);
		    }

		  if (!current)
		    {
		      revert_window (s);
		    }
		}
	    }
	  break;
	case PropertyNotify:
	  property (&ev.xproperty);
	  break;
	case Expose:
	  expose (&ev.xexpose);
	  break;
	case CreateNotify:
	  newwindow (&ev.xcreatewindow);
	  break;
	case DestroyNotify:
	  destroy (ev.xdestroywindow.window);
	  break;
	case ClientMessage:
	  clientmesg (&ev.xclient);
	  break;
	case ColormapNotify:
	  cmap (&ev.xcolormap);
	  break;
	case ReparentNotify:
	  reparent (&ev.xreparent);
	  break;
	case SelectionNotify:
	  break;
	case EnterNotify:
	  enter (&ev.xcrossing);
	  break;
	case FocusIn:
	  focusin (&ev.xfocus);
	  break;
	default:
	  if (shape && ev.type == shape_event)
	    {
	      shapenotify ((XShapeEvent *) & ev);
	    }
	  break;
	}
    }
}

void
expose (XExposeEvent * e)
{
  Client *c;
  ScreenInfo *s;

  if ((c = getclient (e->window, 0)))
    draw_border (c, c == current ? 1 : 0);
  else if ((s = getbarscreen (e->window)))
    draw_tbar (s);
}

void
configurereq (XConfigureRequestEvent * e)
{
  XWindowChanges wc;
  Client *c;

  c = getclient (e->window, 0);
  e->value_mask &= ~CWSibling;

  if (c)
    {
      gravitate (c, 1);

      if (e->value_mask & CWX)
	c->x = e->x;

      if (e->value_mask & CWY)
	c->y = e->y;

      if (e->value_mask & CWWidth)
	c->dx = e->width;

      if (e->value_mask & CWHeight)
	c->dy = e->height;

      if (e->value_mask & CWBorderWidth)
	c->border = e->border_width;

      gravitate (c, 0);

      if (e->value_mask & CWStackMode)
	{
	  if (wc.stack_mode == Above)
	    top (c);
	  else
	    e->value_mask &= ~CWStackMode;
	}

      if (c->parent != c->screen->root && c->window == e->window)
	{
	  wc.x = c->x - BORDER;
	  wc.y = c->y - BORDER;
	  wc.width = c->dx + 2 * BORDER;
	  wc.height = c->dy + 2 * BORDER;
	  wc.border_width = 1;
	  wc.sibling = None;
	  wc.stack_mode = e->detail;
	  XConfigureWindow (dpy, c->parent, e->value_mask, &wc);
	  sendconfig (c);
	}
    }

  if (c && c->init)
    {
      wc.x = BORDER;
      wc.y = BORDER;
    }
  else
    {
      wc.x = e->x;
      wc.y = e->y;
    }

  wc.width = e->width;
  wc.height = e->height;
  wc.border_width = 0;
  wc.sibling = None;
  wc.stack_mode = Above;
  e->value_mask &= ~CWStackMode;
  e->value_mask |= CWBorderWidth;

  XConfigureWindow (dpy, e->window, e->value_mask, &wc);
}

void
mapreq (XMapRequestEvent * e)
{
  Client *c;

  c = getclient (e->window, 0);

  if (c == 0 || c->window != e->window)
    {
      return;
    }

  switch (c->state)
    {
    case WithdrawnState:
      if (c->parent == c->screen->root)
	{
	  if (!manage (c, 0))
	    return;
	  break;
	}
      XReparentWindow (dpy, c->window, c->parent, BORDER, BORDER);
      XAddToSaveSet (dpy, c->window);
      /* fall through... */

    case NormalState:
      if (c->trans != None)
	{
	  Client *p = getclient (c->trans, 0);

	  if (p)
	    {
	      c->desktop = p->desktop;
	    }
	}

      raise_tbar (c->screen);
      XMapWindow (dpy, c->window);
      XMapRaised (dpy, c->parent);
      top (c);
      wmsetstate (c, NormalState);

      if ((mouse_on_screen () == c->screen)
	  || (current && (current->screen == c->screen)))
	{
	  active (c);
	}
      break;

    case IconicState:
      unhidec (c, 1);
      break;
    }
}

void
unmap (XUnmapEvent * e)
{
  Client *c;

  c = getclient (e->window, 0);

  if (c)
    {
      switch (c->state)
	{
	case IconicState:
	  if (e->send_event)
	    {
	      unhidec (c, 0);
	      withdraw (c);
	    }
	  break;

	case NormalState:
	  if (c == current)
	    current = 0;

	  if (!c->reparenting)
	    withdraw (c);
	  break;
	}

      c->reparenting = 0;
    }
}

void
newwindow (XCreateWindowEvent * e)
{
  Client *c;
  ScreenInfo *s;

  if (e->override_redirect)
    return;
  c = getclient (e->window, 1);

  /* Crashing bug patch by MG has been applied here. */
  if (c && c->window == e->window)
    {
      c->x = e->x;
      c->y = e->y;
      c->dx = e->width;
      c->dy = e->height;
      c->border = e->border_width;

      if (0 != (s = getscreen (e->parent)))
	c->screen = s;
      else
	c->screen = &screens[0];

      if (c->parent == None)
	c->parent = c->screen->root;
    }
}

void
destroy (Window w)
{
  Client *c;

  if (!(c = getclient (w, 0)))
    return;

  rmclient (c);

  ignore_badwindow = 1;
  XSync (dpy, False);
  ignore_badwindow = 0;
}

void
clientmesg (XClientMessageEvent * e)
{
  Client *c;

  if (e->message_type == exit_larswm)
    {
      cleanup ();
      exit (0);
    }
  else if (e->message_type == restart_larswm)
    {
      cleanup ();
      execvp (myargv[0], myargv);
      perror ("larswm: exec failed");
      exit (1);
    }
  else if (e->message_type == wm_change_state)
    {
      c = getclient (e->window, 0);

      if (e->format == 32 && e->data.l[0] == IconicState && c != 0)
	{
	  if (normal (c))
	    hide (c);
	}
    }
}

void
cmap (XColormapEvent * e)
{
  Client *c;
  int i;

  if (e->new)
    {
      c = getclient (e->window, 0);
      if (c)
	{
	  c->cmap = e->colormap;
	  if (c == current)
	    cmapfocus (c);
	}
      else
	for (c = clients; c; c = c->next)
	  {
	    for (i = 0; i < c->ncmapwins; i++)
	      if (c->cmapwins[i] == e->window)
		{
		  c->wmcmaps[i] = e->colormap;
		  if (c == current)
		    cmapfocus (c);
		  return;
		}
	  }
    }
}

void
property (XPropertyEvent * e)
{
  Client *c;
  long msize;

  /* check if text to be displayed on status bar was just changed */
  if ((e->atom == bartext_larswm) && (e->window == DefaultRootWindow (dpy)))
    {
      update_cmd_output ();
      return;
    }

  if (!(c = getclient (e->window, 0)))
    return;

  switch (e->atom)
    {
    case XA_WM_ICON_NAME:
      if (c->iconname != 0)
	XFree ((char *) c->iconname);
      c->iconname =
	(e->state == PropertyDelete) ? 0 : getprop (c->window, e->atom);
      setlabel (c);
      renamec (c, c->label);
      break;

    case XA_WM_NAME:
      if (c->name != 0)
	XFree ((char *) c->name);
      c->name =
	(e->state == PropertyDelete) ? 0 : getprop (c->window, e->atom);
      setlabel (c);
      renamec (c, c->label);
      break;

    case XA_WM_TRANSIENT_FOR:
      gettrans (c);
      break;

    case XA_WM_NORMAL_HINTS:
      if (XGetWMNormalHints (dpy, c->window, &c->size, &msize) == 0
	  || c->size.flags == 0)
	{
	  c->size.flags = PSize;
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
      break;

    default:
      if (e->atom == wm_colormaps)
	{
	  getcmaps (c);

	  if (c == current)
	    {
	      cmapfocus (c);
	    }
	}
      break;
    }
}

void
reparent (XReparentEvent * e)
{
  Client *c;
  XWindowAttributes attr;
  ScreenInfo *s;

  if (!getscreen (e->event) || e->override_redirect)
    return;
  if ((s = getscreen (e->parent)) != 0)
    {
      c = getclient (e->window, 1);
      if (c != 0 && (c->dx == 0 || c->dy == 0))
	{
	  XGetWindowAttributes (dpy, c->window, &attr);
	  c->x = attr.x;
	  c->y = attr.y;
	  c->dx = attr.width;
	  c->dy = attr.height;
	  c->border = attr.border_width;
	  c->screen = s;
	  if (c->parent == None)
	    c->parent = c->screen->root;
	}
    }
  else
    {
      c = getclient (e->window, 0);
      if (c != 0 && (c->parent == c->screen->root || withdrawn (c)))
	rmclient (c);
    }
}

void
shapenotify (XShapeEvent * e)
{
  Client *c;

  c = getclient (e->window, 0);
  if (c == 0)
    return;

  setshape (c);
}

void
enter (XCrossingEvent * e)
{
  Client *c;

  if (e->mode != NotifyGrab || e->detail != NotifyNonlinearVirtual)
    return;

  c = getclient (e->window, 0);

  if (c && c->parent && (c != current))
    {
      XMapRaised (dpy, c->parent);
      active (c);
    }
  else
    {
      XSetInputFocus (dpy, e->window, RevertToPointerRoot, e->time);
    }
}

void
focusin (XFocusChangeEvent * e)
{
  Client *c;

  if (e->detail != NotifyNonlinearVirtual)
    return;

  c = getclient (e->window, 0);

  if (c && c->window && c->parent && (c->window == e->window)
      && (c != current))
    {
      XMapRaised (dpy, c->parent);
      active (c);
    }
  else
    {
      XSetInputFocus (dpy, e->window, RevertToPointerRoot, CurrentTime);
    }
}
