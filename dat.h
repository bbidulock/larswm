/* Copyright (c) 1994-1996 David Hogan, see README for licence details */
/* Many changes by Lars Bernhardsson. */
/* Less changes by Florian Forster <octopus@verplant.org> */

#define VERSION		"larswm 7.5.3+patches [22-JAN-2006] by Lars Bernhardsson <lab@fnurt.net>"

#define BORDER		2
#define TILE_PAD	2

#define L_OFF_R 32768
#define L_OFF_G 32768
#define L_OFF_B 32768

#define L_ON_R 65535
#define L_ON_G 0
#define L_ON_B 0

#define L_DELTA 16384

#define L_TOP(x) (unsigned short) (((x) + L_DELTA) > 65535 ? 65535 : (x) + L_DELTA)
#define L_BOT(x) (unsigned short) (((x) - L_DELTA) < 0 ? 0 : (x) - L_DELTA)

#define BAR_TEXT_Y	(1 + font->ascent)

#define BAR_X(s)	0
#define BAR_Y(s)	(DisplayHeight (dpy, (s)->num) - prefs.bar_height)
#define BAR_WIDTH(s)	(DisplayWidth (dpy, (s)->num) - prefs.tray_width)

#define DEFSHELL	"/bin/sh"

#define DEFRC		"/etc/X11/larswmrc"

#define MAXSCREENS	4
#define MAXDESKTOPS	16

#define MAXFLOATCLASS	64
#define MAXTOOLCLASS	64
#define MAXSTICKYCLASS	64
#define MAXNOTILECLASS	64
#define MAXDTCLASS	64

#define MAXHIDDEN	64

#define MAXAPPS		64
#define MAXBTNAPPS	3
#define MAXSWITCHCLASSES 64

/* for zooming */
#define DIM_VERT	1
#define DIM_HORIZ	2
#define DIM_OUTSIDE     4

/*for zooming vertically on left track (in per cent)*/
#define VERT_GROW 5

/* to keep track of if a window is zoomed max */
/* height/width/both or select zoomed */
#define ZOOM_UNZOOMED 0
#define ZOOM_NORMAL 1
#define ZOOM_SELECT 2

/* to prevent windows from being moved totally out */
#define MIN_VISIBLE 20

typedef struct Client Client;
typedef struct ScreenInfo ScreenInfo;
typedef struct Prefs Prefs;

struct Client
{
  Window window;
  Window parent;
  Window trans;
  Client *next;

  int x;
  int y;
  int dx;
  int dy;
  int border;

  int zx;
  int zy;
  int zdx;
  int zdy;
  int iszoom;
  int isleft;
  int isaside;
  int ax;
  int ay;

  XSizeHints size;
  int min_dx;
  int min_dy;

  int state;
  int hidden;
  int init;
  int reparenting;
  int proto;
  int desktop;
  int isfloat;
  int istool;
  int issticky;
  int isshaped;
  int isnotile;

  char *label;
  char *instance;
  char *class;
  char *name;
  char *iconname;

  Colormap cmap;
  int ncmapwins;
  Window *cmapwins;
  Colormap *wmcmaps;
  ScreenInfo *screen;
};

#define hidden(c)	((c)->state == IconicState)
#define withdrawn(c)	((c)->state == WithdrawnState)
#define normal(c)	((c)->state == NormalState)

/* c->proto */
#define Pdelete 	1
#define Ptakefocus	2

struct ScreenInfo
{
  int num;
  Window root;
  Window barwin;
  Colormap def_cmap;
  GC gc;
#ifdef THREE_D
  GC topwhitegc;
  GC botwhitegc;
  GC topblackgc;
  GC botblackgc;
#endif
  unsigned long black;
  unsigned long white;
#ifdef THREE_D
  unsigned long topwhite;
  unsigned long botwhite;
  unsigned long topblack;
  unsigned long botblack;
#endif
  int min_cmaps;
  Cursor place;
  Cursor sweep;
  char display[256];

  int desktop;
  int tile_height;
  int res_height;

/*
  two_on_left:
  When it is 0, there will be one window at left track.
  When it <> 0, there wiil be two windows and the value indicates the height of window.
 */
  int two_on_left[MAXDESKTOPS];
  int bigmr[MAXDESKTOPS];
  int clickthru[MAXDESKTOPS];
  int tile_resize[MAXDESKTOPS];
  int tile_show_bottom[MAXDESKTOPS];
  int skip_focus[MAXDESKTOPS];
  int left_track_width[MAXDESKTOPS];
  int left_track_height[MAXDESKTOPS];
  int notile_raised[MAXDESKTOPS];
  Client *focused[MAXDESKTOPS];
  Client *notilefocused[MAXDESKTOPS];
};

struct Prefs
{
  int clickthru[MAXSCREENS][MAXDESKTOPS];
  int tile_resize[MAXSCREENS][MAXDESKTOPS];
  int tile_show_bottom[MAXSCREENS][MAXDESKTOPS];
  int skip_focus[MAXSCREENS][MAXDESKTOPS];
  int left_track_width[MAXSCREENS][MAXDESKTOPS];

  int desktops;
  int panelsize;
  int bar_height;
  int tray_width;
  char *dtname[MAXSCREENS][MAXDESKTOPS];

  char *bgstr;
  char *fgstr;
  char *fname;

  char *application[MAXAPPS];
  char *btnapplication[MAXBTNAPPS];
  char *switchclass[MAXSWITCHCLASSES];
  char *switchclass_app[MAXSWITCHCLASSES];

  char *floatclass[MAXFLOATCLASS];
  char *toolclass[MAXTOOLCLASS];
  char *notileclass[MAXNOTILECLASS];
  char *stickyclass[MAXSTICKYCLASS];
  char *dtclass[MAXDTCLASS];
  int dtnum[MAXDTCLASS];

  /* keycodes for shortcuts */
  KeySym prev_screen_key;
  unsigned long prev_screen_mod;

  KeySym next_screen_key;
  unsigned long next_screen_mod;

  KeySym prev_desktop_key;
  unsigned long prev_desktop_mod;

  KeySym next_desktop_key;
  unsigned long next_desktop_mod;

  KeySym prev_desktop_drag_key;
  unsigned long prev_desktop_drag_mod;

  KeySym next_desktop_drag_key;
  unsigned long next_desktop_drag_mod;

  KeySym prev_window_key;
  unsigned long prev_window_mod;

  KeySym prev_window_focus_key;
  unsigned long prev_window_focus_mod;

  KeySym next_window_key;
  unsigned long next_window_mod;

  KeySym next_window_focus_key;
  unsigned long next_window_focus_mod;

  KeySym raise_notile_key;
  unsigned long raise_notile_mod;

  KeySym move_northwest_key;
  unsigned long move_northwest_mod;

  KeySym move_north_key;
  unsigned long move_north_mod;

  KeySym move_northeast_key;
  unsigned long move_northeast_mod;

  KeySym move_west_key;
  unsigned long move_west_mod;

  KeySym move_center_key;
  unsigned long move_center_mod;

  KeySym move_east_key;
  unsigned long move_east_mod;

  KeySym move_southwest_key;
  unsigned long move_southwest_mod;

  KeySym move_south_key;
  unsigned long move_south_mod;

  KeySym move_southeast_key;
  unsigned long move_southeast_mod;

  KeySym move_select_key;
  unsigned long move_select_mod;

  KeySym move_aside_key;
  unsigned long move_aside_mod;

  KeySym move_aside_other_key;
  unsigned long move_aside_other_mod;

  KeySym move_sd_key;
  unsigned long move_sd_mod;

  KeySym toggle_inc_key;
  unsigned long toggle_inc_mod;

  KeySym move_up_key;
  unsigned long move_up_mod;

  KeySym move_down_key;
  unsigned long move_down_mod;

  KeySym move_left_key;
  unsigned long move_left_mod;

  KeySym move_right_key;
  unsigned long move_right_mod;

  KeySym grow_vert_key;
  unsigned long grow_vert_mod;

  KeySym shrink_vert_key;
  unsigned long shrink_vert_mod;

  KeySym grow_horiz_key;
  unsigned long grow_horiz_mod;

  KeySym shrink_horiz_key;
  unsigned long shrink_horiz_mod;

  KeySym grow_both_key;
  unsigned long grow_both_mod;

  KeySym shrink_both_key;
  unsigned long shrink_both_mod;

  KeySym zoom_vert_key;
  unsigned long zoom_vert_mod;

  KeySym zoom_horiz_key;
  unsigned long zoom_horiz_mod;

  KeySym zoom_full_key;
  unsigned long zoom_full_mod;

  KeySym zoom_full2_key;
  unsigned long zoom_full2_mod;

  KeySym hide_key;
  unsigned long hide_mod;

  KeySym unhide_key;
  unsigned long unhide_mod;

  KeySym close_key;
  unsigned long close_mod;

  KeySym move_next_desktop_key;
  unsigned long move_next_desktop_mod;

  KeySym move_prev_desktop_key;
  unsigned long move_prev_desktop_mod;

  KeySym two_on_left_key;
  unsigned long two_on_left_mod;

  KeySym two_on_left_shrink_key;
  unsigned long two_on_left_shrink_mod;

  KeySym two_on_left_grow_key;
  unsigned long two_on_left_grow_mod;

  KeySym toggle_skip_focus_key;
  unsigned long toggle_skip_focus_mod;

  KeySym goto_desktop_key[MAXDESKTOPS];
  unsigned long goto_desktop_mod[MAXDESKTOPS];

  KeySym move_desktop_key[MAXDESKTOPS];
  unsigned long move_desktop_mod[MAXDESKTOPS];

  KeySym application_key[MAXAPPS];
  unsigned long application_mod[MAXAPPS];

  KeySym switchclass_key[MAXSWITCHCLASSES];
  unsigned long switchclass_mod[MAXSWITCHCLASSES];
};

/* main.c */
extern Display *dpy;
extern ScreenInfo *screens;
extern int num_screens;
extern int initting;
extern XFontStruct *font;
extern char **myargv;
extern Bool shape;
extern char *shell;

extern Atom exit_larswm;
extern Atom restart_larswm;
extern Atom bartext_larswm;

extern Atom wm_state;
extern Atom wm_change_state;
extern Atom wm_protocols;
extern Atom wm_delete;
extern Atom wm_colormaps;

/* client.c */
extern Client *clients;
extern Client *current;
extern Client *prev_current;

/* buttons.c */
extern Client *hiddenc[];
extern int numhidden;

/* error.c */
extern int ignore_badwindow;

/* prefs.c */
extern Prefs prefs;

/* bar.c */
extern int bar_hidden;
