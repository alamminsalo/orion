#!/bin/bash
set -e -x

sudo apt-fast -y install nasm xorg-dev libwayland-dev libgl1-mesa-dev libegl1-mesa-dev fontconfig libfribidi-dev yasm libxinerama-dev libxrandr-dev libasound2-dev libpulse-dev libjack-dev libvdpau-dev libva-dev libva-egl1 libva-x11-1 libva1 > /dev/null

export MPV_DIR=$HOME/mpv
export MPV_BUILD=$MPV_DIR/mpv-build

if [ -f $MPV_DIR/libmpv.so ]; then
    cd $MPV_DIR
else
    echo "libmpv not found. make."
    rm -rf $MPV_BUILD

    git clone https://github.com/mpv-player/mpv-build.git $MPV_BUILD
    
    git clone https://git.videolan.org/git/ffmpeg/nv-codec-headers.git $MPV_BUILD/nv-codec-headers
    cd $MPV_BUILD/nv-codec-headers
    make && sudo make install
    
    cd $MPV_BUILD
    echo --enable-libmpv-shared > mpv_options
    ./rebuild -j2

    cd $MPV_DIR && mkdir include
    cp -f -r $MPV_BUILD/mpv/libmpv $MPV_DIR/include/mpv
    cp -f $MPV_BUILD/mpv/build/libmpv.so $MPV_DIR/libmpv.so
    
    rm -rf $MPV_BUILD
fi

sudo ln -sf $MPV_DIR/include/mpv /usr/include/mpv
sudo ln -sf $MPV_DIR/libmpv.so /usr/lib/libmpv.so
sudo ln -sf $MPV_DIR/libmpv.so /usr/lib/libmpv.so.1