#!/bin/bash

TITLE=$2$([ ! $4 = "off" ] &&  echo " is streaming" || echo " is now offline")

ICON=$(pwd)/logos/$1.*

MSG=$3

#Set your dialog of choice here
#Examples:

#using notify-send (DEFAULT)
notify-send -i $ICON "$TITLE" "$MSG" -t 5000

#using kdialog
#kdialog --passivepopup "$MSG" 5000 --title "$TITLE" --icon=$(echo $ICON)
