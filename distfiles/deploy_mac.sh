#!/bin/bash
set -e -x

# macdeploy
$QTDIR/bin/macdeployqt orion.app -qmldir=./src/qml

# qml libs 
mkdir -p orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/Qt orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/QtQml orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/QtQuick orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/QtQuick.2 orion.app/Contents/Resources/qml

sh distfiles/fixlibs.sh orion.app

