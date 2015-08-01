#!/bin/bash

TITLE=$1

ICON=$(pwd)$3

MSG=$2

#Set your dialog of choice here
#Examples:

#using notify-send (DEFAULT)
notify-send -i "$ICON" "$TITLE" "$MSG" -t 5000

#using kdialog
#kdialog --passivepopup "$MSG" 5000 --title "$TITLE" --icon=$(echo $ICON)
