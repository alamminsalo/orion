#!/bin/bash

set -e

BASE_DIR=$PWD

export QT_CI_PACKAGES="qt.qt5.5101.android_armv7"
$TRAVIS_BUILD_DIR/ci/install_qt5.sh 5.10.1

export NDK_VERSION=r17c
export ANDROID_DIR=$HOME/android 
export PACKAGES=platform-tool,build-tools-22.0.1,android-19

mkdir -p $ANDROID_DIR  && cd $ANDROID_DIR 

sudo apt install -y p7zip openjdk-7-jdk ant lib32z1-dev lib32stdc++6 > /dev/null
sudo update-alternatives --display java
sudo update-alternatives --set java /usr/lib/jvm/java-7-openjdk-amd64/jre/bin/java

wget -q https://raw.githubusercontent.com/benlau/qtci/master/bin/install-android-sdk
chmod +x install-android-sdk

echo "install android sdk, ndk."
./install-android-sdk $PACKAGES > /dev/null

mkdir -p $TRAVIS_BUILD_DIR/libs
wget -nv -c https://github.com/ph4r05/android-openssl/blob/master/jni/openssl/arch-armeabi-v7a/lib/libcrypto.so?raw=true -O $TRAVIS_BUILD_DIR/libs/libcrypto.so
wget -nv -c https://github.com/ph4r05/android-openssl/blob/master/jni/openssl/arch-armeabi-v7a/lib/libssl.so?raw=true -O $TRAVIS_BUILD_DIR/libs/libssl.so

cat << EOM > $BASE_DIR/ci/android.env
export QTDIR=$HOME/Qt/5.10.1/android_armv7
export PATH=$QTDIR/bin:$PWD/android-sdk-linux/tools:$PATH
export ANDROID_NDK_ROOT=$PWD/android-ndk-${NDK_VERSION}
export ANDROID_SDK_ROOT=$PWD/android-sdk-linux
EOM
