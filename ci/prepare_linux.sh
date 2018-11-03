#!/bin/bash
BASE_DIR=$PWD

$BASE_DIR/ci/build_mpv.sh

export QT_VERSION=5.10.1
export QT_CI_PACKAGES="qt.qt5.5101.gcc_64"
$BASE_DIR/ci/install_qt5.sh $QT_VERSION

cp $HOME/Qt/qt-$QT_VERSION.env $BASE_DIR/ci/linux.env
source $BASE_DIR/ci/linux.env
