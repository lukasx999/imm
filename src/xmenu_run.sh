#!/usr/bin/env bash
set -euo pipefail

APPDIR=/usr/share/applications

apps=$(ls $APPDIR | sed 's/\.desktop$//')
sel=$(echo "$apps" | xmenu)
[[ -z $sel ]] && exit 1

filename="${APPDIR}/${sel}.desktop"
if [[ ! -f $filename ]]; then exit 1; fi

cmd=$(cat $filename | grep 'Exec=.*' -x -m 1 | sed -e 's/Exec=//' -e 's/\s.*$//')
sh -c $cmd & disown
