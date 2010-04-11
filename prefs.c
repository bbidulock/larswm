/* Copyright (c) 2000 Lars Bernhardsson, see README for licence details */
/* many ideas borrowed from blackbox. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <X11/extensions/shape.h>

#include "dat.h"
#include "fns.h"


#define DEFAULT_MOD (ShiftMask | ControlMask)
#define MOVE_MOD (ShiftMask | Mod1Mask)
#define CLOSE_MOD (ControlMask | Mod1Mask)
#define GROW_MOD (ShiftMask | ControlMask | Mod1Mask)

Prefs prefs;


void
set_defaults (void)
{
  int i, j;

  memset (&prefs, 0, sizeof (prefs));

  for (i = 0; i < MAXSCREENS; i++)
    {
      for (j = 0; j < MAXDESKTOPS; j++)
	{
	  prefs.clickthru[i][j] = 1;
	  prefs.skip_focus[i][j] = 1;
	  prefs.tile_resize[i][j] = 1;
	  prefs.left_track_width[i][j] = 60;
	}
    }

  prefs.desktops = 4;

  for (i = 0; i < MAXSCREENS; i++)
    {
      prefs.dtname[i][0] = "[A---]";
      prefs.dtname[i][1] = "[-B--]";
      prefs.dtname[i][2] = "[--C-]";
      prefs.dtname[i][3] = "[---D]";
      prefs.dtname[i][4] = "[E---]";
      prefs.dtname[i][5] = "[-F--]";
      prefs.dtname[i][6] = "[--G-]";
      prefs.dtname[i][7] = "[---H]";
      prefs.dtname[i][8] = "[I---]";
      prefs.dtname[i][9] = "[-J--]";
      prefs.dtname[i][10] = "[--K-]";
      prefs.dtname[i][11] = "[---L]";
      prefs.dtname[i][12] = "[M---]";
      prefs.dtname[i][13] = "[-N--]";
      prefs.dtname[i][14] = "[--O-]";
      prefs.dtname[i][15] = "[---P]";
    }

  prefs.prev_screen_key = XK_Prior;
  prefs.prev_screen_mod = DEFAULT_MOD;

  prefs.next_screen_key = XK_Next;
  prefs.next_screen_mod = DEFAULT_MOD;

  prefs.prev_desktop_key = XK_Left;
  prefs.prev_desktop_mod = DEFAULT_MOD;

  prefs.prev_desktop_drag_key = XK_Left;
  prefs.prev_desktop_drag_mod = MOVE_MOD;

  prefs.next_desktop_key = XK_Right;
  prefs.next_desktop_mod = DEFAULT_MOD;

  prefs.next_desktop_drag_key = XK_Right;
  prefs.next_desktop_drag_mod = MOVE_MOD;

  prefs.prev_window_key = XK_Up;
  prefs.prev_window_mod = DEFAULT_MOD;

  prefs.next_window_key = XK_Down;
  prefs.next_window_mod = DEFAULT_MOD;

  prefs.raise_notile_key = XK_BackSpace;
  prefs.raise_notile_mod = DEFAULT_MOD;

  prefs.move_northwest_key = XK_KP_Home;
  prefs.move_northwest_mod = DEFAULT_MOD;

  prefs.move_north_key = XK_KP_Up;
  prefs.move_north_mod = DEFAULT_MOD;

  prefs.move_northeast_key = XK_KP_Prior;
  prefs.move_northeast_mod = DEFAULT_MOD;

  prefs.move_west_key = XK_KP_Left;
  prefs.move_west_mod = DEFAULT_MOD;

  prefs.move_center_key = XK_KP_Begin;
  prefs.move_center_mod = DEFAULT_MOD;

  prefs.move_east_key = XK_KP_Right;
  prefs.move_east_mod = DEFAULT_MOD;

  prefs.move_southwest_key = XK_KP_End;
  prefs.move_southwest_mod = DEFAULT_MOD;

  prefs.move_south_key = XK_KP_Down;
  prefs.move_south_mod = DEFAULT_MOD;

  prefs.move_southeast_key = XK_KP_Next;
  prefs.move_southeast_mod = DEFAULT_MOD;

  prefs.move_select_key = XK_space;
  prefs.move_select_mod = DEFAULT_MOD;

  prefs.move_aside_key = XK_space;
  prefs.move_aside_mod = CLOSE_MOD;

  prefs.move_aside_other_key = XK_0;
  prefs.move_aside_other_mod = DEFAULT_MOD;

  prefs.move_sd_key = XK_space;
  prefs.move_sd_mod = MOVE_MOD;

  prefs.toggle_inc_key = XK_KP_Insert;
  prefs.toggle_inc_mod = MOVE_MOD;

  prefs.move_up_key = XK_KP_Up;
  prefs.move_up_mod = MOVE_MOD;

  prefs.move_down_key = XK_KP_Down;
  prefs.move_down_mod = MOVE_MOD;

  prefs.move_left_key = XK_KP_Left;
  prefs.move_left_mod = MOVE_MOD;

  prefs.move_right_key = XK_KP_Right;
  prefs.move_right_mod = MOVE_MOD;

  prefs.grow_vert_key = XK_KP_Up;
  prefs.grow_vert_mod = GROW_MOD;

  prefs.shrink_vert_key = XK_KP_Down;
  prefs.shrink_vert_mod = GROW_MOD;

  prefs.grow_horiz_key = XK_KP_Right;
  prefs.grow_horiz_mod = GROW_MOD;

  prefs.shrink_horiz_key = XK_KP_Left;
  prefs.shrink_horiz_mod = GROW_MOD;

  prefs.grow_both_key = XK_KP_Home;
  prefs.grow_both_mod = GROW_MOD;

  prefs.shrink_both_key = XK_KP_End;
  prefs.shrink_both_mod = GROW_MOD;

  prefs.zoom_vert_key = XK_KP_Home;
  prefs.zoom_vert_mod = MOVE_MOD;

  prefs.zoom_horiz_key = XK_KP_End;
  prefs.zoom_horiz_mod = MOVE_MOD;

  prefs.zoom_full_key = XK_KP_Begin;
  prefs.zoom_full_mod = MOVE_MOD;

  prefs.zoom_full2_key = XK_KP_Begin;
  prefs.zoom_full2_mod = GROW_MOD;

  prefs.hide_key = XK_z;
  prefs.hide_mod = CLOSE_MOD;

  prefs.unhide_key = XK_x;
  prefs.unhide_mod = CLOSE_MOD;

  prefs.close_key = XK_w;
  prefs.close_mod = CLOSE_MOD;

  prefs.move_next_desktop_key = XK_Next;
  prefs.move_next_desktop_mod = MOVE_MOD;

  prefs.move_prev_desktop_key = XK_Delete;
  prefs.move_prev_desktop_mod = MOVE_MOD;
  
  prefs.two_on_left_key = XK_Left;
  prefs.two_on_left_mod = GROW_MOD;

  prefs.two_on_left_shrink_key = XK_Up;
  prefs.two_on_left_shrink_mod = GROW_MOD;

  prefs.two_on_left_grow_key = XK_Down;
  prefs.two_on_left_grow_mod = GROW_MOD;

  /* provide defaults for up to 12 virtual desktops */
  prefs.goto_desktop_key[0] = XK_F1;
  prefs.goto_desktop_mod[0] = DEFAULT_MOD;

  prefs.goto_desktop_key[1] = XK_F2;
  prefs.goto_desktop_mod[1] = DEFAULT_MOD;

  prefs.goto_desktop_key[2] = XK_F3;
  prefs.goto_desktop_mod[2] = DEFAULT_MOD;

  prefs.goto_desktop_key[3] = XK_F4;
  prefs.goto_desktop_mod[3] = DEFAULT_MOD;

  prefs.goto_desktop_key[4] = XK_F5;
  prefs.goto_desktop_mod[4] = DEFAULT_MOD;

  prefs.goto_desktop_key[5] = XK_F6;
  prefs.goto_desktop_mod[5] = DEFAULT_MOD;

  prefs.goto_desktop_key[6] = XK_F7;
  prefs.goto_desktop_mod[6] = DEFAULT_MOD;

  prefs.goto_desktop_key[7] = XK_F8;
  prefs.goto_desktop_mod[7] = DEFAULT_MOD;

  prefs.goto_desktop_key[8] = XK_F9;
  prefs.goto_desktop_mod[8] = DEFAULT_MOD;

  prefs.goto_desktop_key[9] = XK_F10;
  prefs.goto_desktop_mod[9] = DEFAULT_MOD;

  prefs.goto_desktop_key[10] = XK_F11;
  prefs.goto_desktop_mod[10] = DEFAULT_MOD;

  prefs.goto_desktop_key[11] = XK_F12;
  prefs.goto_desktop_mod[11] = DEFAULT_MOD;

  prefs.move_desktop_key[0] = XK_F1;
  prefs.move_desktop_mod[0] = MOVE_MOD;
  
  prefs.move_desktop_key[1] = XK_F2;
  prefs.move_desktop_mod[1] = MOVE_MOD;
  
  prefs.move_desktop_key[2] = XK_F3;
  prefs.move_desktop_mod[2] = MOVE_MOD;
  
  prefs.move_desktop_key[3] = XK_F4;
  prefs.move_desktop_mod[3] = MOVE_MOD;
  
  prefs.move_desktop_key[4] = XK_F5;
  prefs.move_desktop_mod[4] = MOVE_MOD;
  
  prefs.move_desktop_key[5] = XK_F6;
  prefs.move_desktop_mod[5] = MOVE_MOD;
  
  prefs.move_desktop_key[6] = XK_F7;
  prefs.move_desktop_mod[6] = MOVE_MOD;
  
  prefs.move_desktop_key[7] = XK_F8;
  prefs.move_desktop_mod[7] = MOVE_MOD;
  
  prefs.move_desktop_key[8] = XK_F9;
  prefs.move_desktop_mod[8] = MOVE_MOD;
  
  prefs.move_desktop_key[9] = XK_F10;
  prefs.move_desktop_mod[9] = MOVE_MOD;
  
  prefs.move_desktop_key[10] = XK_F11;
  prefs.move_desktop_mod[10] = MOVE_MOD;
  
  prefs.move_desktop_key[11] = XK_F12;
  prefs.move_desktop_mod[11] = MOVE_MOD;
}

char *
show_bool (int x)
{
  if (x)
    return "True";
  else
    return "False";
}

char *
show_mod (unsigned long mod)
{
  static char r[48];

  r[0] = '\0';

  if (!mod)
    {
      return "None";
    }

  if (mod & ShiftMask)
    {
      strcat (r, "Shift");
    }

  if (mod & LockMask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Lock");
    }

  if (mod & ControlMask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Control");
    }

  if (mod & Mod1Mask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Alt");
    }

  if (mod & Mod2Mask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Mod2");
    }

  if (mod & Mod3Mask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Mod3");
    }

  if (mod & Mod4Mask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Mod4");
    }

  if (mod & Mod5Mask)
    {
      if (r[0])
	strcat (r, "+");
      strcat (r, "Mod5");
    }

  return r;
}

void
dump_prefs (void)
{
  printf ("!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf ("! larswm configuration !\n");
  printf ("!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf ("!\n");
  printf ("! Version: %s\n", VERSION);
  printf ("!\n");
  printf
    ("! If you edit this, do not forget to uncomment any lines that you change.\n");
  printf ("!\n");
  printf ("!\n");
  printf ("! Status bar and window border colors:\n");
  printf ("!\n");
  if (prefs.fgstr)
    printf ("! larswm.foreground: %s\n", prefs.fgstr);
  else
    printf ("! larswm.foreground: black\n");
  if (prefs.bgstr)
    printf ("! larswm.background: %s\n", prefs.bgstr);
  else
    printf ("! larswm.background: white\n");
  printf ("!\n");
  printf ("! Status bar font:\n");
  printf ("!\n");
  if (prefs.fname)
    printf ("! larswm.font: %s\n", prefs.fname);
  else
    printf ("! larswm.font: fixed\n");
  printf ("!\n");
  printf ("! Number of virtual desktops per physical screen: (1 - %d)\n",
	  MAXDESKTOPS);
  printf ("!\n");
  printf ("! larswm.desktops: %d\n", prefs.desktops);
  printf ("!\n");
  printf ("! Virtual desktop names:\n");
  printf ("!\n");
  printf ("! larswm.?.0.dtname: %s\n", prefs.dtname[0][0]);
  printf ("! larswm.?.1.dtname: %s\n", prefs.dtname[0][1]);
  printf ("! larswm.?.2.dtname: %s\n", prefs.dtname[0][2]);
  printf ("! larswm.?.3.dtname: %s\n", prefs.dtname[0][3]);
  printf ("!\n");
  printf
    ("! Resize windows when tiling? (If not, move to and stack in right track)\n");
  printf ("!\n");
  printf ("! larswm.?.?.tile_resize: %s\n",
	  show_bool (prefs.tile_resize[0][0]));
  printf ("!\n");
  printf
    ("! Keep bottom part of tiled windows visible? (If not, keep top part visible)\n");
  printf ("!\n");
  printf ("! larswm.?.?.tile_show_bottom: %s\n",
          show_bool (prefs.tile_show_bottom[0][0]));
  printf ("!\n");
  printf ("! Width of left track in percent: (5-95)\n");
  printf ("!\n");
  printf ("! larswm.?.?.left_track_width: %d\n",
	  prefs.left_track_width[0][0]);
  printf ("!\n");
  printf ("! Don't retile on focus change?\n");
  printf ("!\n");
  printf ("! larswm.?.?.skip_focus: %s\n",
	  show_bool (prefs.skip_focus[0][0]));
  printf ("!\n");
  printf ("! When clicking to focus, also pass click on to client?\n");
  printf ("!\n");
  printf ("! larswm.?.?.clickthru: %s\n", show_bool (prefs.clickthru[0][0]));
  printf ("!\n");

  printf ("! Application commands for shortcuts:\n");
  printf
    ("! Add one line for each application. Must have different numbers between 0 and %d.\n",
     MAXAPPS - 1);
  printf ("!\n");
  printf ("! larswm.application.0: command\n");
  printf ("!\n");
  printf ("! Application commands for mouse clicks:\n");
  printf ("!\n");
  printf ("! larswm.button1.application: command\n");
  printf ("! larswm.button2.application: command\n");
  printf ("! larswm.button3.application: command\n");
  printf ("!\n");

  printf ("! * In all the class lists, the Instance is optional *\n");
  printf ("!\n");
  printf
    ("! Tool applications: (Will NOT be tiled, will be tiled around and will be sticky)\n");
  printf
    ("! Add one line for each class. Must have different numbers between 0 and %d.\n",
     MAXTOOLCLASS - 1);
  printf ("!\n");
  printf ("! larswm.toolclass.0: Class~Instance\n");
  printf ("!\n");
  printf
    ("! Floating applications: (Will always stay on top of other applications)\n");
  printf
    ("! Add one line for each class. Must have different numbers between 0 and %d.\n",
     MAXFLOATCLASS - 1);
  printf ("!\n");
  printf ("! larswm.floatclass.0: Class~Instance\n");
  printf ("!\n");
  printf
    ("! Sticky applications: (Will be visible on all virtual desktops)\n");
  printf
    ("! Add one line for each class. Must have different numbers between 0 and %d.\n",
     MAXSTICKYCLASS - 1);
  printf ("!\n");
  printf ("! larswm.stickyclass.0: Class~Instance\n");
  printf ("!\n");
  printf
    ("! Applications to include in tiling: (Will be put in the tiled subdesktop)\n");
  printf
    ("! Add one line for each class. Must have different numbers between 0 and %d.\n",
     MAXNOTILECLASS - 1);
  printf ("!\n");
  printf ("! larswm.dotileclass.0: Class~Instance\n");
  printf ("!\n");
  printf ("! Associate a window class with a desktop number:\n");
  printf ("! Must provide both class and desktop number.\n");
  printf ("!\n");
  printf ("! larswm.dtclass.0: Class~Instance\n");
  printf ("! larswm.dtnum.0: 0\n");
  printf ("!\n");

  printf
    ("! Key bindings: (Modifiers: Shift, Lock, Control, Mod1 or Alt, Mod2, Mod3, Mod4 and Mod5)\n");
  printf ("!\n");
  printf ("! larswm.prev_screen_key: %s\n",
	  XKeysymToString (prefs.prev_screen_key));
  printf ("! larswm.prev_screen_mod: %s\n", show_mod (prefs.prev_screen_mod));
  printf ("!\n");
  printf ("! larswm.next_screen_key: %s\n",
	  XKeysymToString (prefs.next_screen_key));
  printf ("! larswm.next_screen_mod: %s\n", show_mod (prefs.next_screen_mod));
  printf ("!\n");

  printf ("! larswm.prev_desktop_key: %s\n",
	  XKeysymToString (prefs.prev_desktop_key));
  printf ("! larswm.prev_desktop_mod: %s\n",
	  show_mod (prefs.prev_desktop_mod));
  printf ("!\n");
  printf ("! larswm.next_desktop_key: %s\n",
	  XKeysymToString (prefs.next_desktop_key));
  printf ("! larswm.next_desktop_mod: %s\n",
	  show_mod (prefs.next_desktop_mod));
  printf ("!\n");

  printf ("! larswm.prev_desktop_drag_key: %s\n",
	  XKeysymToString (prefs.prev_desktop_drag_key));
  printf ("! larswm.prev_desktop_drag_mod: %s\n",
	  show_mod (prefs.prev_desktop_drag_mod));
  printf ("!\n");
  printf ("! larswm.next_desktop_drag_key: %s\n",
	  XKeysymToString (prefs.next_desktop_drag_key));
  printf ("! larswm.next_desktop_drag_mod: %s\n",
	  show_mod (prefs.next_desktop_drag_mod));
  printf ("!\n");

  printf ("! larswm.prev_window_key: %s\n",
	  XKeysymToString (prefs.prev_window_key));
  printf ("! larswm.prev_window_mod: %s\n", show_mod (prefs.prev_window_mod));
  printf ("!\n");
  printf ("! larswm.next_window_key: %s\n",
	  XKeysymToString (prefs.next_window_key));
  printf ("! larswm.next_window_mod: %s\n", show_mod (prefs.next_window_mod));
  printf ("!\n");

  printf ("! larswm.raise_notile_key: %s\n",
	  XKeysymToString (prefs.raise_notile_key));
  printf ("! larswm.raise_notile_mod: %s\n",
	  show_mod (prefs.raise_notile_mod));
  printf ("!\n");

  printf ("! larswm.move_northwest_key: %s\n",
	  XKeysymToString (prefs.move_northwest_key));
  printf ("! larswm.move_northwest_mod: %s\n",
	  show_mod (prefs.move_northwest_mod));
  printf ("!\n");
  printf ("! larswm.move_north_key: %s\n",
	  XKeysymToString (prefs.move_north_key));
  printf ("! larswm.move_north_mod: %s\n", show_mod (prefs.move_north_mod));
  printf ("!\n");
  printf ("! larswm.move_northeast_key: %s\n",
	  XKeysymToString (prefs.move_northeast_key));
  printf ("! larswm.move_northeast_mod: %s\n",
	  show_mod (prefs.move_northeast_mod));
  printf ("!\n");
  printf ("! larswm.move_west_key: %s\n",
	  XKeysymToString (prefs.move_west_key));
  printf ("! larswm.move_west_mod: %s\n", show_mod (prefs.move_west_mod));
  printf ("!\n");
  printf ("! larswm.move_center_key: %s\n",
	  XKeysymToString (prefs.move_center_key));
  printf ("! larswm.move_center_mod: %s\n", show_mod (prefs.move_center_mod));
  printf ("!\n");
  printf ("! larswm.move_east_key: %s\n",
	  XKeysymToString (prefs.move_east_key));
  printf ("! larswm.move_east_mod: %s\n", show_mod (prefs.move_east_mod));
  printf ("!\n");
  printf ("! larswm.move_southwest_key: %s\n",
	  XKeysymToString (prefs.move_southwest_key));
  printf ("! larswm.move_southwest_mod: %s\n",
	  show_mod (prefs.move_southwest_mod));
  printf ("!\n");
  printf ("! larswm.move_south_key: %s\n",
	  XKeysymToString (prefs.move_south_key));
  printf ("! larswm.move_south_mod: %s\n", show_mod (prefs.move_south_mod));
  printf ("!\n");
  printf ("! larswm.move_southeast_key: %s\n",
	  XKeysymToString (prefs.move_southeast_key));
  printf ("! larswm.move_southeast_mod: %s\n",
	  show_mod (prefs.move_southeast_mod));
  printf ("!\n");

  printf ("! larswm.toggle_inc_key: %s\n",
	  XKeysymToString (prefs.toggle_inc_key));
  printf ("! larswm.toggle_inc_mod: %s\n", show_mod (prefs.toggle_inc_mod));
  printf ("!\n");

  printf ("! larswm.move_up_key: %s\n", XKeysymToString (prefs.move_up_key));
  printf ("! larswm.move_up_mod: %s\n", show_mod (prefs.move_up_mod));
  printf ("!\n");
  printf ("! larswm.move_down_key: %s\n",
	  XKeysymToString (prefs.move_down_key));
  printf ("! larswm.move_down_mod: %s\n", show_mod (prefs.move_down_mod));
  printf ("!\n");
  printf ("! larswm.move_left_key: %s\n",
	  XKeysymToString (prefs.move_left_key));
  printf ("! larswm.move_left_mod: %s\n", show_mod (prefs.move_left_mod));
  printf ("!\n");
  printf ("! larswm.move_right_key: %s\n",
	  XKeysymToString (prefs.move_right_key));
  printf ("! larswm.move_right_mod: %s\n", show_mod (prefs.move_right_mod));
  printf ("!\n");

  printf ("! larswm.move_select_key: %s\n",
	  XKeysymToString (prefs.move_select_key));
  printf ("! larswm.move_select_mod: %s\n", show_mod (prefs.move_select_mod));
  printf ("!\n");
  printf ("! larswm.move_aside_key: %s\n",
	  XKeysymToString (prefs.move_aside_key));
  printf ("! larswm.move_aside_mod: %s\n", show_mod (prefs.move_aside_mod));
  printf ("!\n");
  printf ("! larswm.move_aside_other_key: %s\n",
	  XKeysymToString (prefs.move_aside_other_key));
  printf ("! larswm.move_aside_other_mod: %s\n",
	  show_mod (prefs.move_aside_other_mod));
  printf ("!\n");
  printf ("! larswm.move_sd_key: %s\n", XKeysymToString (prefs.move_sd_key));
  printf ("! larswm.move_sd_mod: %s\n", show_mod (prefs.move_sd_mod));
  printf ("!\n");

  printf ("! larswm.grow_vert_key: %s\n",
	  XKeysymToString (prefs.grow_vert_key));
  printf ("! larswm.grow_vert_mod: %s\n", show_mod (prefs.grow_vert_mod));
  printf ("!\n");
  printf ("! larswm.shrink_vert_key: %s\n",
	  XKeysymToString (prefs.shrink_vert_key));
  printf ("! larswm.shrink_vert_mod: %s\n", show_mod (prefs.shrink_vert_mod));
  printf ("!\n");
  printf ("! larswm.grow_horiz_key: %s\n",
	  XKeysymToString (prefs.grow_horiz_key));
  printf ("! larswm.grow_horiz_mod: %s\n", show_mod (prefs.grow_horiz_mod));
  printf ("!\n");
  printf ("! larswm.shrink_horiz_key: %s\n",
	  XKeysymToString (prefs.shrink_horiz_key));
  printf ("! larswm.shrink_horiz_mod: %s\n",
	  show_mod (prefs.shrink_horiz_mod));
  printf ("!\n");
  printf ("! larswm.grow_both_key: %s\n",
	  XKeysymToString (prefs.grow_both_key));
  printf ("! larswm.grow_both_mod: %s\n", show_mod (prefs.grow_both_mod));
  printf ("!\n");
  printf ("! larswm.shrink_both_key: %s\n",
	  XKeysymToString (prefs.shrink_both_key));
  printf ("! larswm.shrink_both_mod: %s\n", show_mod (prefs.shrink_both_mod));
  printf ("!\n");

  printf ("! larswm.zoom_vert_key: %s\n",
	  XKeysymToString (prefs.zoom_vert_key));
  printf ("! larswm.zoom_vert_mod: %s\n", show_mod (prefs.zoom_vert_mod));
  printf ("!\n");
  printf ("! larswm.zoom_horiz_key: %s\n",
	  XKeysymToString (prefs.zoom_horiz_key));
  printf ("! larswm.zoom_horiz_mod: %s\n", show_mod (prefs.zoom_horiz_mod));
  printf ("!\n");
  printf ("! larswm.zoom_full_key: %s\n",
	  XKeysymToString (prefs.zoom_full_key));
  printf ("! larswm.zoom_full_mod: %s\n", show_mod (prefs.zoom_full_mod));
  printf ("!\n");
  printf ("! larswm.zoom_full2_key: %s\n",
	  XKeysymToString (prefs.zoom_full2_key));
  printf ("! larswm.zoom_full2_mod: %s\n", show_mod (prefs.zoom_full2_mod));
  printf ("!\n");

  printf ("! larswm.hide_key: %s\n", XKeysymToString (prefs.hide_key));
  printf ("! larswm.hide_mod: %s\n", show_mod (prefs.hide_mod));
  printf ("!\n");
  printf ("! larswm.unhide_key: %s\n", XKeysymToString (prefs.unhide_key));
  printf ("! larswm.unhide_mod: %s\n", show_mod (prefs.unhide_mod));
  printf ("!\n");
  printf ("! larswm.close_key: %s\n", XKeysymToString (prefs.close_key));
  printf ("! larswm.close_mod: %s\n", show_mod (prefs.close_mod));
  printf ("!\n");

  printf ("! larswm.move_next_desktop_key: %s\n", XKeysymToString (prefs.move_next_desktop_key));
  printf ("! larswm.move_next_desktop_mod: %s\n", show_mod (prefs.move_next_desktop_mod));
  printf ("!\n");
  printf ("! larswm.move_prev_desktop_key: %s\n", XKeysymToString (prefs.move_prev_desktop_key));
  printf ("! larswm.move_prev_desktop_mod: %s\n", show_mod (prefs.move_prev_desktop_mod));
  printf ("!\n");

  printf ("! larswm.two_on_left_key: %s\n", XKeysymToString (prefs.two_on_left_key));
  printf ("! larswm.two_on_left_mod: %s\n", show_mod (prefs.two_on_left_mod));
  printf ("!\n");
  printf ("! larswm.two_on_left_shrink_key: %s\n", XKeysymToString (prefs.two_on_left_shrink_key));
  printf ("! larswm.two_on_left_shrink_mod: %s\n", show_mod (prefs.two_on_left_shrink_mod));
  printf ("!\n");
  printf ("! larswm.two_on_left_grow_key: %s\n", XKeysymToString (prefs.two_on_left_grow_key));
  printf ("! larswm.two_on_left_grow_mod: %s\n", show_mod (prefs.two_on_left_grow_mod));
  printf ("!\n");

  printf
    ("! Desktop shortcuts: Defaults are provided for the first 12 desktops, F1-F12.\n");
  printf
    ("! If you have more than 12 desktops, you will have to add key bindings for them\n");
  printf ("! or just use prev/next desktop to get to where you want.\n");
  printf ("!\n");
  printf ("! larswm.goto_desktop_key.0: %s\n",
	  XKeysymToString (prefs.goto_desktop_key[0]));
  printf ("! larswm.goto_desktop_mod.0: %s\n",
	  show_mod (prefs.goto_desktop_mod[0]));
  printf ("!\n");

  printf ("! Move window to desktop shortcuts: Defaults are for the first 12 desktops, F1-F12.\n");
  printf ("! If you have more than 12 desktops, you will have to add key bindings for them\n");
  printf ("! or just use move prev/next desktop to move window to where you want.\n");
  printf ("!\n");
  printf ("! larswm.move_desktop_key.0: %s\n",
	  XKeysymToString (prefs.move_desktop_key[0]));
  printf ("! larswm.move_desktop_mod.0: %s\n",
	  show_mod (prefs.move_desktop_mod[0]));
  printf ("!\n");
  
  printf ("! Application shortcuts:\n");
  printf
    ("! Use this to add keyboard shortcuts for the larswm.application[] entries.\n");
  printf ("!\n");
  printf ("! larswm.application_key.0: KeySym\n");
  printf ("! larswm.application_mod.0: Modifier\n");
  printf ("!\n");

  printf ("! Shortcuts to switch (focus) to window with defined name or class.\n");
  printf ("!\n");
  printf ("! larswm.switchclass.0: XTerm\n");
  printf ("! larswm.switchclass_app.0: xterm\n");
  printf ("! larswm.switchclass_key.0: KeySym\n");
  printf ("! larswm.switchclass_mod.0: Modifier\n");
  printf ("!\n");

  printf ("! End of larswm configuration.\n");
}

unsigned long
get_mod (char *s)
{
  unsigned long m = 0;

  if (strstr (s, "shift") || strstr (s, "Shift"))
    m |= ShiftMask;
  if (strstr (s, "lock") || strstr (s, "Lock"))
    m |= LockMask;
  if (strstr (s, "control") || strstr (s, "Control"))
    m |= ControlMask;
  if (strstr (s, "alt") || strstr (s, "Alt"))
    m |= Mod1Mask;
  if (strstr (s, "mod1") || strstr (s, "Mod1"))
    m |= Mod1Mask;
  if (strstr (s, "mod2") || strstr (s, "Mod2"))
    m |= Mod2Mask;
  if (strstr (s, "mod3") || strstr (s, "Mod3"))
    m |= Mod3Mask;
  if (strstr (s, "mod4") || strstr (s, "Mod4"))
    m |= Mod4Mask;
  if (strstr (s, "mod5") || strstr (s, "Mod5"))
    m |= Mod5Mask;

  return m;
}

int
get_bool (const char *s)
{
  if (strcasecmp ("true", s) == 0)
    return 1;
  else
    return 0;
}

void
validate_prefs (void)
{
  int i, j;

  if (prefs.desktops < 1)
    prefs.desktops = 1;
  if (prefs.desktops > MAXDESKTOPS)
    prefs.desktops = MAXDESKTOPS;

  for (i = 0; i < MAXSCREENS; i++)
    {
      for (j = 0; j < MAXDESKTOPS; j++)
	{
	  if (prefs.left_track_width[i][j] < 5)
	    prefs.left_track_width[i][j] = 5;

	  if (prefs.left_track_width[i][j] > 95)
	    prefs.left_track_width[i][j] = 95;
	}
    }
}

void
check_idx_intfunc (XrmDatabase db, char *r, char *c, int s, int d, int *p,
		   int (*func) (const char *))
{
  XrmValue v;
  char *vt;
  char res[32], cla[32];

  sprintf (res, "larswm.%d.%d.%s", s, d, r);
  sprintf (cla, "Larswm.%d.%d.%s", s, d, c);

  if (XrmGetResource (db, res, cla, &vt, &v))
    *p = func (v.addr);
}

void
check_idx_strfunc (XrmDatabase db, char *r, char *c, int s, int d, char **p,
		   char *(*func) (const char *))
{
  XrmValue v;
  char *vt;
  char res[32], cla[32];

  sprintf (res, "larswm.%d.%d.%s", s, d, r);
  sprintf (cla, "Larswm.%d.%d.%s", s, d, c);

  if (XrmGetResource (db, res, cla, &vt, &v))
    *p = func (v.addr);
}

void
load_prefs (char *filename)
{
  XrmDatabase db = (XrmDatabase) 0;
  XrmValue v;
  char *vt;
  int i, j;

  set_defaults ();

  if ((db = XrmGetFileDatabase (filename)) == NULL)
    if ((db = XrmGetFileDatabase (DEFRC)) == NULL)
      return;

  if (XrmGetResource (db, "larswm.desktops", "Larswm.Desktops", &vt, &v))
    prefs.desktops = atoi (v.addr);

  for (i = 0; i < MAXSCREENS; i++)
    {
      for (j = 0; j < MAXDESKTOPS; j++)
	{
	  check_idx_strfunc (db, "dtname", "Dtname", i, j,
			     &prefs.dtname[i][j], strdup);
	  check_idx_intfunc (db, "clickthru", "Clickthru", i, j,
			     &prefs.clickthru[i][j], get_bool);
	  check_idx_intfunc (db, "skip_focus", "Skip_focus", i, j,
			     &prefs.skip_focus[i][j], get_bool);
	  check_idx_intfunc (db, "tile_resize", "Tile_resize", i, j,
			     &prefs.tile_resize[i][j], get_bool);
          check_idx_intfunc (db, "tile_show_bottom", 
                                 "Tile_show_bottom", i, j,
                             &prefs.tile_show_bottom[i][j], get_bool);
	  check_idx_intfunc (db, "left_track_width", "Left_track_width", i, j,
			     &prefs.left_track_width[i][j], atoi);
	}
    }

  if (XrmGetResource (db, "larswm.background", "Larswm.Background", &vt, &v))
    prefs.bgstr = strdup (v.addr);

  if (XrmGetResource (db, "larswm.foreground", "Larswm.Foreground", &vt, &v))
    prefs.fgstr = strdup (v.addr);

  if (XrmGetResource (db, "larswm.font", "Larswm.Font", &vt, &v))
    prefs.fname = strdup (v.addr);

  for (i = 0; i < MAXAPPS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.application.%d", i);
      sprintf (cla, "Larswm.Application.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.application[i] = strdup (v.addr);
    }

  for (i = 0; i < MAXSWITCHCLASSES; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.switchclass.%d", i);
      sprintf (cla, "Larswm.Switchclass.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.switchclass[i] = strdup (v.addr);

      sprintf (res, "larswm.switchclass_app.%d", i);
      sprintf (cla, "Larswm.Switchclass_app.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.switchclass_app[i] = strdup (v.addr);
    }
  
  for (i = 0; i < MAXTOOLCLASS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.toolclass.%d", i);
      sprintf (cla, "Larswm.Toolclass.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.toolclass[i] = strdup (v.addr);
    }

  for (i = 0; i < MAXFLOATCLASS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.floatclass.%d", i);
      sprintf (cla, "Larswm.Floatclass.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.floatclass[i] = strdup (v.addr);
    }

  for (i = 0; i < MAXSTICKYCLASS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.stickyclass.%d", i);
      sprintf (cla, "Larswm.Stickyclass.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.stickyclass[i] = strdup (v.addr);
    }

  for (i = 0; i < MAXNOTILECLASS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.dotileclass.%d", i);
      sprintf (cla, "Larswm.Dotileclass.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.notileclass[i] = strdup (v.addr);
    }

  for (i = 0; i < MAXDTCLASS; i++)
    {
      char *ts = 0;
      int dt = -1;
      char res[32], cla[32];

      sprintf (res, "larswm.dtclass.%d", i);
      sprintf (cla, "Larswm.Dtclass.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	{
	  ts = strdup (v.addr);
	}

      sprintf (res, "larswm.dtnum.%d", i);
      sprintf (cla, "Larswm.Dtnum.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	{
	  dt = atoi (v.addr);
	}

      if (ts && (dt > -1) && (dt < prefs.desktops) && (dt < MAXDESKTOPS))
	{
	  prefs.dtclass[i] = ts;
	  prefs.dtnum[i] = dt;
	}
    }

  if (XrmGetResource
      (db, "larswm.prev_screen_key", "Larswm.Prev_screen_key", &vt, &v))
    prefs.prev_screen_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.prev_screen_mod", "Larswm.Prev_screen_mod", &vt, &v))
    prefs.prev_screen_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.next_screen_key", "Larswm.Next_screen_key", &vt, &v))
    prefs.next_screen_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.next_screen_mod", "Larswm.Next_screen_mod", &vt, &v))
    prefs.next_screen_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.prev_desktop_key", "Larswm.Prev_desktop_key", &vt, &v))
    prefs.prev_desktop_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.prev_desktop_mod", "Larswm.Prev_desktop_mod", &vt, &v))
    prefs.prev_desktop_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.prev_desktop_drag_key", "Larswm.Prev_desktop_drag_key",
       &vt, &v))
    prefs.prev_desktop_drag_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.prev_desktop_drag_mod", "Larswm.Prev_desktop_drag_mod",
       &vt, &v))
    prefs.prev_desktop_drag_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.next_desktop_key", "Larswm.Next_desktop_key", &vt, &v))
    prefs.next_desktop_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.next_desktop_mod", "Larswm.Next_desktop_mod", &vt, &v))
    prefs.next_desktop_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.next_desktop_drag_key", "Larswm.Next_desktop_drag_key",
       &vt, &v))
    prefs.next_desktop_drag_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.next_desktop_drag_mod", "Larswm.Next_desktop_drag_mod",
       &vt, &v))
    prefs.next_desktop_drag_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.prev_window_key", "Larswm.Prev_window_key", &vt, &v))
    prefs.prev_window_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.prev_window_mod", "Larswm.Prev_window_mod", &vt, &v))
    prefs.prev_window_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.next_window_key", "Larswm.Next_window_key", &vt, &v))
    prefs.next_window_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.next_window_mod", "Larswm.Next_window_mod", &vt, &v))
    prefs.next_window_mod = get_mod (v.addr);


  for (i = 0; i < MAXDESKTOPS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.goto_desktop_key.%d", i);
      sprintf (cla, "Larswm.Goto_desktop_key.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.goto_desktop_key[i] = XStringToKeysym (v.addr);

      sprintf (res, "larswm.goto_desktop_mod.%d", i);
      sprintf (cla, "Larswm.Goto_desktop_mod.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.goto_desktop_mod[i] = get_mod (v.addr);
    }


  for (i = 0; i < MAXBTNAPPS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.button%d.application", i + 1);
      sprintf (cla, "Larswm.Button%d.Application", i + 1);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.btnapplication[i] = strdup (v.addr);
    }


  for (i = 0; i < MAXAPPS; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.application_key.%d", i);
      sprintf (cla, "Larswm.Application_key.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.application_key[i] = XStringToKeysym (v.addr);

      sprintf (res, "larswm.application_mod.%d", i);
      sprintf (cla, "Larswm.Application_mod.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.application_mod[i] = get_mod (v.addr);
    }

  for (i = 0; i < MAXSWITCHCLASSES; i++)
    {
      char res[32], cla[32];

      sprintf (res, "larswm.switchclass_key.%d", i);
      sprintf (cla, "Larswm.Switchclass_key.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.switchclass_key[i] = XStringToKeysym (v.addr);

      sprintf (res, "larswm.switchclass_mod.%d", i);
      sprintf (cla, "Larswm.Switchclass_mod.%d", i);

      if (XrmGetResource (db, res, cla, &vt, &v))
	prefs.switchclass_mod[i] = get_mod (v.addr);
    }

  if (XrmGetResource
      (db, "larswm.raise_notile_key", "Larswm.Raise_notile_key", &vt, &v))
    prefs.raise_notile_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.raise_notile_mod", "Larswm.Raise_notile_mod", &vt, &v))
    prefs.raise_notile_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_northwest_key", "Larswm.Move_northwest_key", &vt, &v))
    prefs.move_northwest_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_northwest_mod", "Larswm.Move_northwest_mod", &vt, &v))
    prefs.move_northwest_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_north_key", "Larswm.Move_north_key", &vt, &v))
    prefs.move_north_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_north_mod", "Larswm.Move_north_mod", &vt, &v))
    prefs.move_north_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_northeast_key", "Larswm.Move_northeast_key", &vt, &v))
    prefs.move_northeast_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_northeast_mod", "Larswm.Move_northeast_mod", &vt, &v))
    prefs.move_northeast_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_west_key", "Larswm.Move_west_key", &vt, &v))
    prefs.move_west_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_west_mod", "Larswm.Move_west_mod", &vt, &v))
    prefs.move_west_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_center_key", "Larswm.Move_center_key", &vt, &v))
    prefs.move_center_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_center_mod", "Larswm.Move_center_mod", &vt, &v))
    prefs.move_center_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_east_key", "Larswm.Move_east_key", &vt, &v))
    prefs.move_east_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_east_mod", "Larswm.Move_east_mod", &vt, &v))
    prefs.move_east_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_southwest_key", "Larswm.Move_southwest_key", &vt, &v))
    prefs.move_southwest_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_southwest_mod", "Larswm.Move_southwest_mod", &vt, &v))
    prefs.move_southwest_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_south_key", "Larswm.Move_south_key", &vt, &v))
    prefs.move_south_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_south_mod", "Larswm.Move_south_mod", &vt, &v))
    prefs.move_south_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_southeast_key", "Larswm.Move_southeast_key", &vt, &v))
    prefs.move_southeast_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_southeast_mod", "Larswm.Move_southeast_mod", &vt, &v))
    prefs.move_southeast_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.toggle_inc_key", "Larswm.Toggle_inc_key", &vt, &v))
    prefs.toggle_inc_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.toggle_inc_mod", "Larswm.Toggle_inc_mod", &vt, &v))
    prefs.toggle_inc_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_up_key", "Larswm.Move_up_key", &vt, &v))
    prefs.move_up_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_up_mod", "Larswm.Move_up_mod", &vt, &v))
    prefs.move_up_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_down_key", "Larswm.Move_down_key", &vt, &v))
    prefs.move_down_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_down_mod", "Larswm.Move_down_mod", &vt, &v))
    prefs.move_down_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_left_key", "Larswm.Move_left_key", &vt, &v))
    prefs.move_left_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_left_mod", "Larswm.Move_left_mod", &vt, &v))
    prefs.move_left_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_right_key", "Larswm.Move_right_key", &vt, &v))
    prefs.move_right_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_right_mod", "Larswm.Move_right_mod", &vt, &v))
    prefs.move_right_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.move_select_key", "Larswm.Move_select_key", &vt, &v))
    prefs.move_select_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_select_mod", "Larswm.Move_select_mod", &vt, &v))
    prefs.move_select_mod = get_mod (v.addr);

  if (XrmGetResource
      (db, "larswm.move_aside_key", "Larswm.Move_aside_key", &vt, &v))
    prefs.move_aside_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_aside_mod", "Larswm.Move_aside_mod", &vt, &v))
    prefs.move_aside_mod = get_mod (v.addr);

  if (XrmGetResource
      (db, "larswm.move_aside_other_key", "Larswm.Move_aside_other_key", &vt,
       &v))
    prefs.move_aside_other_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_aside_other_mod", "Larswm.Move_aside_other_mod", &vt,
       &v))
    prefs.move_aside_other_mod = get_mod (v.addr);

  if (XrmGetResource
      (db, "larswm.move_sd_key", "Larswm.Move_sd_key", &vt, &v))
    prefs.move_sd_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.move_sd_mod", "Larswm.Move_sd_mod", &vt, &v))
    prefs.move_sd_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.grow_vert_key", "Larswm.Grow_vert_key", &vt, &v))
    prefs.grow_vert_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.grow_vert_mod", "Larswm.Grow_vert_mod", &vt, &v))
    prefs.grow_vert_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.shrink_vert_key", "Larswm.Shrink_vert_key", &vt, &v))
    prefs.shrink_vert_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.shrink_vert_mod", "Larswm.Shrink_vert_mod", &vt, &v))
    prefs.shrink_vert_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.grow_horiz_key", "Larswm.Grow_horiz_key", &vt, &v))
    prefs.grow_horiz_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.grow_horiz_mod", "Larswm.Grow_horiz_mod", &vt, &v))
    prefs.grow_horiz_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.shrink_horiz_key", "Larswm.Shrink_horiz_key", &vt, &v))
    prefs.shrink_horiz_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.shrink_horiz_mod", "Larswm.Shrink_horiz_mod", &vt, &v))
    prefs.shrink_horiz_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.grow_both_key", "Larswm.Grow_both_key", &vt, &v))
    prefs.grow_both_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.grow_both_mod", "Larswm.Grow_both_mod", &vt, &v))
    prefs.grow_both_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.shrink_both_key", "Larswm.Shrink_both_key", &vt, &v))
    prefs.shrink_both_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.shrink_both_mod", "Larswm.Shrink_both_mod", &vt, &v))
    prefs.shrink_both_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.zoom_vert_key", "Larswm.Zoom_vert_key", &vt, &v))
    prefs.zoom_vert_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.zoom_vert_mod", "Larswm.Zoom_vert_mod", &vt, &v))
    prefs.zoom_vert_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.zoom_horiz_key", "Larswm.Zoom_horiz_key", &vt, &v))
    prefs.zoom_horiz_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.zoom_horiz_mod", "Larswm.Zoom_horiz_mod", &vt, &v))
    prefs.zoom_horiz_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.zoom_full_key", "Larswm.Zoom_full_key", &vt, &v))
    prefs.zoom_full_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.zoom_full_mod", "Larswm.Zoom_full_mod", &vt, &v))
    prefs.zoom_full_mod = get_mod (v.addr);


  if (XrmGetResource
      (db, "larswm.zoom_full2_key", "Larswm.Zoom_full2_key", &vt, &v))
    prefs.zoom_full2_key = XStringToKeysym (v.addr);

  if (XrmGetResource
      (db, "larswm.zoom_full2_mod", "Larswm.Zoom_full2_mod", &vt, &v))
    prefs.zoom_full2_mod = get_mod (v.addr);


  if (XrmGetResource (db, "larswm.hide_key", "Larswm.Hide_key", &vt, &v))
    prefs.hide_key = XStringToKeysym (v.addr);

  if (XrmGetResource (db, "larswm.hide_mod", "Larswm.Hide_mod", &vt, &v))
    prefs.hide_mod = get_mod (v.addr);


  if (XrmGetResource (db, "larswm.unhide_key", "Larswm.Unhide_key", &vt, &v))
    prefs.unhide_key = XStringToKeysym (v.addr);

  if (XrmGetResource (db, "larswm.unhide_mod", "Larswm.Unhide_mod", &vt, &v))
    prefs.unhide_mod = get_mod (v.addr);


  if (XrmGetResource (db, "larswm.close_key", "Larswm.Close_key", &vt, &v))
    prefs.close_key = XStringToKeysym (v.addr);

  if (XrmGetResource (db, "larswm.close_mod", "Larswm.Close_mod", &vt, &v))
    prefs.close_mod = get_mod (v.addr);

  if (XrmGetResource (db, "larswm.two_on_left_key", "Larswm.Two_on_left_key", &vt, &v))
    prefs.two_on_left_key = XStringToKeysym (v.addr);

  if (XrmGetResource (db, "larswm.two_on_left_mod", "Larswm.Two_on_left_mod", &vt, &v))
    prefs.two_on_left_mod = get_mod (v.addr);


  validate_prefs ();
}
