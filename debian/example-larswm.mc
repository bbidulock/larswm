!!!!!!!!!!!!!!!!!!!!!!!!
! larswm configuration !
!!!!!!!!!!!!!!!!!!!!!!!!
!
! Don't edit .larswmrc , edit larswm.mc and regenerate your .larswmrc by
! running "m4 larswm.mc >.larswmrc"
!

include(`larswm.m4')dnl

!
! Default window properties
!
PROP(background,darkslategray)
PROP(foreground,lightgray)

!
! Virtual desktop settings
!
PROP(desktops, 4)

DTASPROP(dtname, 0, `Dink [1---]')
DTASPROP(dtname, 1, `Slay [-2--]')
DTASPROP(dtname, 2, `Hack [--3-]')
DTASPROP(dtname, 3, `Surf [---4]')

DTASPROP(left_track_width, 0, 69)
DTASPROP(left_track_width, 1, 69)
DTASPROP(left_track_width, 2, 69)
DTASPROP(left_track_width, 3, 69)


!
! Focus and tiling behaviour
!
DTALLPROP(tile_resize, False)
DTALLPROP(skip_focus, False)
DTALLPROP(clickthru, False)
DTPROP(clickthru, 0, 3, True)

!
! Window class patterns
!
TOOLCLASS(`XTerm~eventlog')
TOOLCLASS(`XDaliClock')
TOOLCLASS(`XOsview')
TOOLCLASS(`xmms~XMMS_Player')
TOOLCLASS(`XkbLEDPanel')

FLOATCLASS(`fXterm~xterm')

STICKYCLASS(`fXterm~xterm')
STICKYCLASS(`XDaliClock')
STICKYCLASS(`XOsview')

DOTILECLASS(`tXterm~xterm')
DOTILECLASS(`Emacs')
DOTILECLASS(`Mozilla~Navigator')
dnl DOTILECLASS(`Firefox-bin~Gecko')
dnl DOTILECLASS(`Thunderbird-bin~Gecko')

DTCLASS(`Mozilla~navigator', 3)
DTCLASS(`Thunderbird-bin', 0)

!
! Larswm command keys
!
KEY(End, 	Shift+Control, 		hide)
KEY(Home, 	Shift+Control,		unhide)
KEY(Delete,	Shift+Control,		close)
dnl
dnl ! Keys for the two-on-left patch
dnl KEY(Left, 	Shift+Control+Alt, 	two_on_left)
dnl KEY(Down, 	Shift+Control+Alt, 	two_on_left_shrink)
dnl KEY(Up, 	Shift+Control+Alt, 	two_on_left_grow)
dnl
DTKEY(F1,	Alt,			goto_desktop, 0)
DTKEY(F2,	Alt,			goto_desktop, 1)
DTKEY(F3,	Alt,			goto_desktop, 2)
DTKEY(F4,	Alt,			goto_desktop, 3)

!
! Application Keys and mouse buttons
!
APPBUTTON(2, 	`xcolour xterm -ls -class uXterm -fn neep-18 -geometry 80x25')
APPBUTTON(3, 	`xcolour xterm -ls -class tXterm -fn neep-18')

APPKEY(u, 	Control+Alt, 		`xcolour xterm -ls -class uXterm -fn neep-18 -geometry 80x25')
APPKEY(t, 	Control+Alt, 		`xcolour xterm -ls -class tXterm -fn neep-18')
APPKEY(m, 	Control+Alt,		`firefox -P default')
APPKEY(e, 	Control+Alt,		`emacs')
APPKEY(j, 	Control+Alt,		`jpilot')
APPKEY(q, 	Control+Alt,		`larsremote exit')
APPKEY(3, 	Control+Alt,		`xmms')
APPKEY(i, 	Control+Alt,		`xmix')
APPKEY(g, 	Control+Alt,		`emacs -f gnus')
APPKEY(n, 	Control+Alt,		`thunderbird')
APPKEY(Menu, 	None,			`xcolour xterm -ls -class uXterm -fn modd-12 -geometry 80x10+1-1 -e env SHELL=run bash')

!
! App keys for Logitech iTouch keyboard
!
XF86KEY(Standby,		`lock')

XF86KEY(Finance,		`xtightvncviewer')
XF86KEY(MySites,		`firefox -P proxytesting')
XF86KEY(Community,		`mp3-pl-osd')
XF86KEY(Favorites,		`mp3-osd -d 8')

XF86KEY(AudioPrev,		`xmms-remote --rew')
XF86KEY(AudioNext,		`xmms-remote --fwd')
XF86KEY(AudioMedia,		`xcolour xterm -ls -class uXterm -fn neep-18 -geometry 80x15 -e aumix')
XF86KEY(AudioRaiseVolume,	`aumix -v+2')
XF86KEY(AudioLowerVolume,	`aumix -v-2')
XF86KEY(AudioMute,		`mute')
XF86KEY(AudioPlay,		`xmms-remote --play-pause')
XF86KEY(AudioStop,		`xmms-remote --stop')

XF86KEY(iTouch,			`vmware')
XF86KEY(Mail,			`thunderbird')
dnl XF86KEY(Shop,		`')
dnl XF86KEY(Search,		`')
XF86KEY(HomePage,		`firefox -P default')

