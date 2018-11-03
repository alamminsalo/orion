#!/bin/bash
set -e -x

ARTIFACTS=$1

# macdeploy
$QTDIR/bin/macdeployqt orion.app -qmldir=./src/qml

# qml libs 
mkdir -p orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/Qt orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/QtQml orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/QtQuick orion.app/Contents/Resources/qml
cp -r $QTDIR/qml/QtQuick.2 orion.app/Contents/Resources/qml

sh ci/deploy_fixlibs_osx.sh orion.app

dmgbuild -s distfiles/settings.py "Orion $TRAVIS_TAG" "orion-$TRAVIS_TAG.dmg"
mv ./orion-$TRAVIS_TAG.dmg $ARTIFACTS/orion-$PLATFORM-$TRAVIS_TAG.dmg