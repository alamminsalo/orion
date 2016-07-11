#!/bin/bash

DIR=$1
FRAMEWORKS="QtOpenGL QtWidgets QtGui QtCore QtWebEngine QtQuick QtQml QtNetwork"

echo "Fixing lib paths..."
for FRAMEWORK in $FRAMEWORKS; do
	echo $FRAMEWORK
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/MacOS/orion
done

#echo "Additional libs..."
#install_name_tool -id @executable_path/../Frameworks/QtAV.framework/Versions/1/QtAV \
#$DIR/Contents/Frameworks/QtAV.framework/Versions/1/QtAV
#install_name_tool -change /Users/antti/Qt/5.5/clang_64/lib/QtAV.framework/Versions/1/QtAV \
#@executable_path/../Frameworks/QtAV.framework/Versions/1/QtAV \
#$DIR/Contents/MacOS/orion


