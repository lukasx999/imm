#!/usr/bin/env bash

APPDIR=/usr/share/applications

apps=$(find $APPDIR -type f -name '*.desktop')
apps_stripped=$(echo "$apps" | xargs -I {} grep 'Name=.*' -x -m 1 {} | sed 's/Name=//' | sort)

sel=$(echo "$apps_stripped" | xmenu)
[[ -z $sel ]] && exit 1

# TODO: this
echo $sel

exit 0
filename="${APPDIR}/${sel}.desktop"
if [[ ! -f $filename ]]; then exit 1; fi

cmd=$(cat $filename | grep 'Exec=.*' -x -m 1 | sed -e 's/Exec=//' -e 's/\s.*$//')
sh -c $cmd & disown
