/* Copyright (c) 1994-1996 David Hogan, see README for licence details */

/* main.c */
void usage (void);
void initscreen (ScreenInfo *, int);
ScreenInfo *getscreen (Window);
void sendcmessage (Window, Atom, long, int);
void sendconfig (Client *);
void cleanup (void);

/* event.c */
void mainloop (int);
void selection (XSelectionEvent *);
void expose (XExposeEvent *);
void configurereq (XConfigureRequestEvent *);
void mapreq (XMapRequestEvent *);
void unmap (XUnmapEvent *);
void newwindow (XCreateWindowEvent *);
void destroy (Window);
void clientmesg (XClientMessageEvent *);
void cmap (XColormapEvent *);
void property (XPropertyEvent *);
void shapenotify (XShapeEvent *);
void reparent (XReparentEvent *);
void enter (XCrossingEvent *);
void focusin (XFocusChangeEvent *);


/* manage.c */
int manage (Client *, int);
void scanwins (ScreenInfo *);
void setshape (Client *);
void withdraw (Client *);
void gravitate (Client *, int);
void cmapfocus (Client *);
void cmapnofocus (ScreenInfo *);
void getcmaps (Client *);
int _getprop (Window, Atom, Atom, long, unsigned char **);
char *getprop (Window, Atom);
int getstate (Window, int *);
void wmsetstate (Client *, int);
void setlabel (Client *);
void getproto (Client *);
void gettrans (Client *);

/* mouse.c */
void move_opaque (Client *);
void resize_opaque (Client *);

/* buttons.c */
void button (XButtonEvent *);
void spawn (ScreenInfo *, char *);
void window_of_class (ScreenInfo *, char *, char *);
void wmdelete (Client *, int);
void hide (Client *);
void unhide (int, int);
void unhidec (Client *, int);
void renamec (Client *, char *);
void move_other_desktop(Client *, int);

/* client.c */
void setactive (Client *, int);
void draw_border (Client *, int);
void active (Client *);
void nofocus (void);
void top (Client *);
Client *getclient (Window, int);
void rmclient (Client *);

/* error.c */
int handler (Display *, XErrorEvent *);
void fatal (char *);

/* tiling.c */
void getmouse (int *, int *, ScreenInfo *);
void setmouse (int, int, ScreenInfo *);
int is_float (Client *);
int is_notile (Client *);
int is_tool (Client *);
int is_sticky (Client *);
int is_dt (Client *);
void toggle_inc (ScreenInfo *);
void toggle_notile (ScreenInfo *);
void raise_tile (ScreenInfo *, int);
void raise_notile (ScreenInfo *);
void toggle_isfloat (Client *);
void toggle_istool (Client *);
void toggle_issticky (Client *);
void toggle_clickthru (ScreenInfo *);
void toggle_tile_resize (ScreenInfo *);
int count_clients (ScreenInfo *);
void calc_track_sizes (ScreenInfo *);
void tile_all (ScreenInfo *);
void move_hide (Client *);
void move_show (Client *);
void update_desktop (ScreenInfo *);
void next_desktop (ScreenInfo *);
void next_desktop_drag (ScreenInfo *);
void prev_desktop (ScreenInfo *);
void prev_desktop_drag (ScreenInfo *);
void goto_desktop (ScreenInfo *, int);
void toggle_skip_focus (ScreenInfo *);
void revert_window (ScreenInfo *);
void revert_same (ScreenInfo *);
void prev_window (ScreenInfo *);
void prev_window_focus (ScreenInfo *);
void next_window (ScreenInfo *);
void next_window_focus (ScreenInfo *);
void prev_screen (ScreenInfo *);
void next_screen (ScreenInfo *);
void shrink_left (ScreenInfo *);
void grow_left (ScreenInfo *);
void max_left (ScreenInfo *);
void restore_left (ScreenInfo *);
ScreenInfo *mouse_on_screen (void);
void move_northwest (Client *);
void move_north (Client *);
void move_northeast (Client *);
void move_west (Client *);
void move_center (Client *);
void move_east (Client *);
void move_southwest (Client *);
void move_south (Client *);
void move_southeast (Client *);
void move_up (Client *);
void move_down (Client *);
void move_left (Client *);
void move_right (Client *);
void move_select (Client *, int);
int count_aside (ScreenInfo *);
Client *find_aside (ScreenInfo *);
void arrange_aside (ScreenInfo *);
void move_aside (ScreenInfo *, Client *);
void move_aside_other (ScreenInfo *);
void move_aside_clear (ScreenInfo *, Client *);
void zoom (Client *, int);
void grow (Client *, int, int);
void move_sd (Client *);
void two_on_left (ScreenInfo *);
void two_on_left_grow (ScreenInfo *, int);

/* prefs.c */
void set_defaults (void);
void dump_prefs (void);
void validate_prefs (void);
void load_prefs (char *);

/* keys.c */
void initkeys (Client *);
void keyevent (XKeyEvent *);

/* bar.c */
void do_menu (ScreenInfo *);
void prev_menu (void);
void next_menu (void);
void show_hidden (void);
void prev_hidden (void);
void next_hidden (void);
void toggle_show_class (void);
void update_cmd_output (void);
void draw_tbar (ScreenInfo *);
void update_tbar (void);
void raise_tbar (ScreenInfo *);
ScreenInfo *getbarscreen (Window);
