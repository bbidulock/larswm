divert(-1)dnl
!-----------------------------------------------------------------------------
!
! Macros for creating larswm config files
!
! Created by Christopher Biggs <unixbigot@pobox.com>, when I realized that
! maintaining my .larswmrc was just too painful, with all the numbered 
! window-class and application key sections.   These macros allow the 
! numbering to be generated automagically.
!
! A second benefit is that the total size of this macro package plus my 
! larswm.mc input file is less than half the size of my original .larswmrc!
!

! 
! Counters used for auto-increment properties
!
define(`appkey_counter',`0')dnl
define(`toolclass_counter',`0')dnl
define(`floatclass_counter',`0')dnl
define(`stickyclass_counter',`0')dnl
define(`dotileclass_counter',`0')dnl
define(`dtclass_counter',`0')dnl

define(`COUNTER_INCR',`define(`$1',incr($1))')dnl

!
! PROP(`prop', `value') 			=> larswm.prop: value
!
define(`PROP',`larswm.$1: $2')


!
! DTPROP(`prop', <screen>, <dt>, `value') 	=> larswm.<screen>.<dt>.prop: value
! DTASPROP(`prop', <dt>, `value') 		=> larswm.?.<dt>.prop: value
! DTALLPROP(`prop', `value') 			=> larswm.?.?.prop: value
!
! DTPROP    sets property on one DeskTop (on one specific screen).
! DTASPROP  sets property on one DeskTop (on All Screens).
! DTALLPROP sets property on ALL desktops on all screens.

define(`DTPROP',`larswm.$2.$3.$1: $4')
define(`DTASPROP',`larswm.?.$2.$1: $3')
define(`DTALLPROP', `larswm.?.?.$1: $2')

!
! KEY(<key>, <mod>, <cmd>) 			=> larswm.<cmd>_key: <key>
!						   larswm.<cmd>_mod: <mod>
!
define(`KEY',`larswm.$3_key: $1
larswm.$3_mod: $2
')

!
! DTKEY(<key>, <mod>, <cmd>, <n>)		=> larswm.<cmd>_key.n: <key>
!						   larswm.<cmd>_mod.n: <mod>
!
define(`DTKEY',`larswm.$3_key.$4: $1
larswm.$3_mod.$4: $2
')

!
! APPBUTTON(<n>, <cmd>)				=> larswm.buttonn.application: <cmd>
!

define(`APPBUTTON',`larswm.button$1.application: $2')

!
! APPKEY(<key>, <mod>, <cmd>)			=> larswm.application.n: <cmd>
!						   larswm.application_key.n: <key>
!						   larswm.application_mod.n: <mod>
!
! XF86KEY(<key>, <cmd>)				=> larswm.application.n: <cmd>
!						   larswm.application_key.n: XF86<key>
!						   larswm.application_mod.n: None
! (n auto-increments, starting at zero)
define(`APPKEY',`larswm.application.appkey_counter: $3
larswm.application_key.appkey_counter: $1
larswm.application_mod.appkey_counter: $2
COUNTER_INCR(`appkey_counter')')

define(`XF86KEY',`larswm.application.appkey_counter: $2
larswm.application_key.appkey_counter: XF86$1
larswm.application_mod.appkey_counter: None
COUNTER_INCR(`appkey_counter')')


!
! TOOLCLASS(<class>)				=> larswm.toolclass.n: <class>
! FLOATCLASS(<class>)				=> larswm.floatclass.n: <class>
! STICKYCLASS(<class>)				=> larswm.stickyclass.n: <class>
! DOTILECLASS(<class>)				=> larswm.dotileclass.n: <class>
!
! (n auto-increments, starting at zero)
!
define(`upcase', `translit(`$*', `a-z', `A-Z')')dnl
	
define(`FOOCLASS2',`define(`$1CLASS',`larswm.$2.$3: '`COUNTER_INCR(`$3')'$`'1)')
define(`FOOCLASS',`FOOCLASS2(upcase($1),$1class,`$1class_counter')')

FOOCLASS(`tool')
FOOCLASS(`float')
FOOCLASS(`sticky')
FOOCLASS(`dotile')

!
! DTCLASS(<class>, dt)				=> larswm.dtclass.n: <class>
!  						=> larswm.dtnum.n: <class>
!
define(`DTCLASS',`larswm.dtclass.dtclass_counter: $1
larswm.dtnum.dtclass_counter: $2
COUNTER_INCR(`dtclass_counter')
')

divert(0)dnl
