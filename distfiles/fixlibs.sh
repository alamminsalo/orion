#!/bin/bash
#Fixes mac frameworks

DIR=$1

#Qt frameworks
FRAMEWORKS="QtOpenGL QtWidgets QtGui QtCore QtWebEngine QtQuick QtQml QtNetwork QtMultimedia QtWebChannel QtWebEngineCore"

echo "Fixing lib paths..."
for FRAMEWORK in $FRAMEWORKS; do
	echo $FRAMEWORK
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/MacOS/orion
done

#QtQuick.2 dylib
FRAMEWORKS="QtQuick QtQml QtNetwork QtCore QtGui"
for FRAMEWORK in $FRAMEWORKS; do
	echo $FRAMEWORK
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Resources/qml/QtQuick.2/libqtquick2plugin.dylib
done

#QtWebEngine
FRAMEWORKS="QtWebEngine QtWebEngineCore QtQuick QtQml QtCore QtNetwork QtGui QtWebChannel"
for FRAMEWORK in $FRAMEWORKS; do
	echo $FRAMEWORK
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Resources/qml/QtWebEngine/libqtwebengineplugin.dylib
done

#Multimedia libs
FRAMEWORKS="QtGui QtCore QtQuick QtQml QtNetwork QtMultimedia QtMultimediaQuick_p"
for FRAMEWORK in $FRAMEWORKS; do
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Resources/qml/QtMultimedia/libdeclarative_multimedia.dylib
done
	

