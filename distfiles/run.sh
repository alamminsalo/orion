#!/bin/bash

#Export local shared libs

cd "$(dirname "$0")"
echo "DIR IN "$PWD
export LD_LIBRARY_PATH=./lib


#Create desktop entry if not found

if [ ! -f ~/.local/share/applications/Orion.desktop ]; then

    DESKTOP_ENTRY=~/.local/share/applications/Orion.desktop
    echo "Creating new desktop entry"
    
    touch $DESKTOP_ENTRY
    echo -e "[Desktop Entry]\nVersion=1.0\nType=Application\nName=Orion" > $DESKTOP_ENTRY
    echo "Icon="$PWD"/orion.svg" >> $DESKTOP_ENTRY
    echo "Exec="$PWD"/run.sh" >> $DESKTOP_ENTRY
    echo -e "Comment=Seek and watch streams on Twitch\nCategories=Games;\nTerminal=false\nStartupWMClass=orion" >> $DESKTOP_ENTRY
fi


#Run app

./orion

