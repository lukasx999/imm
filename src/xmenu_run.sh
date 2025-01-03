#!/usr/bin/env bash
set -euo pipefail

APPDIR=/usr/share/applications

# TODO: remove duplicate entries
apps=$(find $APPDIR -type f -name '*.desktop')
apps_stripped=$(echo "$apps" | xargs -I {} grep 'Name=.*' -x -m 1 {} | sed 's/Name=//')

sel=$(echo "$apps_stripped" | xmenu -e)
[[ -z $sel ]] && exit 1

index=$(echo $sel | awk '{ print $1 }')
if [[ $index == 0 ]]; then exit 1; fi
filename=$(echo "$apps" | awk "NR==$index")

cmd=$(cat $filename | grep 'Exec=.*' -x -m 1 | sed -e 's/Exec=//' -e 's/\s.*$//')
sh -c $cmd & disown
