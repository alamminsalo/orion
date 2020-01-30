#!/bin/bash

cat << EOM > ci/osx.env
export QTDIR=$(brew info --json=v1 qt | jq -r '.[0].bottle.stable.cellar + "/" + .[0].name + "/" + .[0].installed[0].version')
export PATH=$QTDIR/bin:$PATH
EOM
