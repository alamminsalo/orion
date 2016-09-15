#!/bin/bash

QTVER=5.5

# macdeploy
~/Qt/$QTVER/clang_64/bin/macdeployqt orion.app -qmldir=~/git/orion/src/qml

# qml libs 
mkdir orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/Enginio orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/Qt orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/QtQml orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/QtQuick orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/QtQuick.2 orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/QtWebEngine orion.app/Contents/Resources/qml
cp -r ~/Qt/$QTVER/clang_64/qml/Communi orion.app/Contents/Resources/qml

# additional frameworks
cp -r ~/Qt/$QTVER/clang_64/lib/IrcCore.framework orion.app/Contents/Frameworks/IrcCore.framework
cp -r ~/Qt/$QTVER/clang_64/lib/IrcModel.framework orion.app/Contents/Frameworks/IrcModel.framework
cp -r ~/Qt/$QTVER/clang_64/lib/IrcUtil.framework orion.app/Contents/Frameworks/IrcUtil.framework

sh fixlibs.sh orion.app

