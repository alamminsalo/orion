#!/bin/bash
#Fixes mac frameworks

DIR=$1

#Qt frameworks
FRAMEWORKS="QtOpenGL QtWidgets QtGui QtCore QtQuick QtQml QtNetwork QtMultimedia"

echo "Fixing lib paths..."

echo "- orion"
for FRAMEWORK in $FRAMEWORKS; do
	echo "  $FRAMEWORK"
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/MacOS/orion

	install_name_tool -change $QTDIR/lib/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/MacOS/orion
done

for ref in Resources/qml/{QtQuick/{Window.2/libwindow,Controls/libqtquickcontrols,Controls/Styles/Flat/libqtquickextrasflat,Controls.2/{libqtquickcontrols2,Material/libqtquickcontrols2materialstyle,Universal/libqtquickcontrols2universalstyle},Dialogs/{libdialog,Private/libdialogsprivate},Extras/libqtquickextras,LocalStorage/libqmllocalstorage,Particles.2/libparticles,PrivateWidgets/libwidgets,Scene2D/libqtquickscene2d,Scene3D/libqtquickscene3d,VirtualKeyboard/Styles/libqtvirtualkeyboardstyles,XmlListModel/libqmlxmllistmodel,Templates.2/libqtquicktemplates2,Layouts/libqquicklayouts},QtQuick.2/libqtquick2,Qt/labs/{calendar/libqtlabscalendar,folderlistmodel/libqmlfolderlistmodel,platform/libqtlabsplatform,settings/libqmlsettings,sharedimage/libsharedimage},QtQml/Models.2/libmodels}plugin.dylib PlugIns/quick/lib{models,qquicklayouts,qtquickcontrols2{material,universal}style,qtquickcontrols{,2},qtquickextrasflat,qtquicktemplates2,widgets,window}plugin.dylib Resources/qml/QtQml/StateMachine/libqtqmlstatemachine.dylib; do
echo "- $ref"

FRAMEWORKS="QtQuick QtQuickControls2 QtQuickTemplates2 QtQml QtNetwork QtCore QtGui QtWidgets"
for FRAMEWORK in $FRAMEWORKS; do
	echo "  $FRAMEWORK"
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/$ref

	install_name_tool -change $QTDIR/lib/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/$ref
done
done

echo "- multimedia"

#Multimedia libs
FRAMEWORKS="QtGui QtCore QtQuick QtQml QtNetwork QtMultimedia QtMultimediaQuick_p"
for FRAMEWORK in $FRAMEWORKS; do
	echo "  $FRAMEWORK"
	install_name_tool -id @executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK

	install_name_tool -change @rpath/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Resources/qml/QtMultimedia/libdeclarative_multimedia.dylib

	install_name_tool -change $QTDIR/lib/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/5/$FRAMEWORK \
	$DIR/Contents/Resources/qml/QtMultimedia/libdeclarative_multimedia.dylib
done
	

