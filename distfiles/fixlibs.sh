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

