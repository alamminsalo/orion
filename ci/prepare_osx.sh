#!/bin/bash

cat << EOM > ci/osx.env
export QTDIR=$(brew --prefix qt)
export PATH=$QTDIR/bin:$PATH
EOM
