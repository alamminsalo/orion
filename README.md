
#Orion

QML/C++-written desktop client for Twitch.tv

Features desktop notifications and integrated player

##Screencaptures

![alt tag](https://raw.githubusercontent.com/alamminsalo/orion/master/resources/screenshots/4.png)

![alt tag](https://raw.githubusercontent.com/alamminsalo/orion/master/resources/screenshots/5.png)

##Dependencies

libmpv (not supported on windows) or QtAV

libcommuni



##Building on linux

(Using arch linux examples, but can be applied to other distros as well)

####Install needed libraries and software

```
pacman -S git gcc qtcreator qt5-svg qt5-webengine qtav
```

####Get libcommuni, build and install it:

```
git clone https://github.com/communi/libcommuni
cd libcommuni
qmake
make
sudo make install
```
####Get orion from github

```
git clone https://github.com/alamminsalo/orion
cd orion
mkdir build && cd build
qmake ../
make
```

If all goes well, orion/build directory should now contain a working binary named 'orion'!

By running 'run.sh', a desktop entry is created for the menus also


