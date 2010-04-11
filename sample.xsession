#!/bin/sh

#
# Copyright (c) 2004 Lars Bernhardsson, see README for licence details
#
# sample.xsession - Login script for larswm
#

# Clear root window settings and set background color
xsetroot && xsetroot -solid lightgray

# 2nd screen
# xsetroot -display :0.1 && xsetroot -display :0.1 -solid lightgray

# Start a couple of tools
xload -geometry 96x48-0-0 &
xbiff -geometry 48x48-100-0 &
xclock -geometry 48x48-152-0 &

# Start a background job that feeds date/time to larswm
larsclock &

# Start larswm.
exec larswm
