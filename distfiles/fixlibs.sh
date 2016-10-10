#!/bin/bash

DIR=$1

#Qt frameworks
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

#libcommuni framework
FRAMEWORKS="IrcCore IrcUtil IrcModel"
for FRAMEWORK in $FRAMEWORKS; do
	echo $FRAMEWORK
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/3/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	$DIR/Contents/MacOS/orion

	install_name_tool -change ~/Qt/5.5/clang_64/lib/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	$DIR/Contents/Resources/qml/Communi/libcommuniplugin.dylib

	install_name_tool -change ~/Qt/5.5/clang_64/lib/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	$DIR/Contents/Frameworks/IrcUtil.framework/Versions/3/IrcUtil

	install_name_tool -change ~/Qt/5.5/clang_64/lib/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/3/$FRAMEWORK \
	$DIR/Contents/Frameworks/IrcModel.framework/Versions/3/IrcModel
done

