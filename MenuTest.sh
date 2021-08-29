#!/bin/bash

export TERM=linux
export XDG_RUNTIME_DIR=/run/user/$UID/

while true; do
    selection=(dialog \
        --backtitle "AnberLauncher Test" \
        --title "AnberLauncher Test" \
        --no-collapse \
        --clear \
        --cancel-label "Select + Start to Exit" \
        --menu "$pair" 9 55 9
    )
    options=(
        "1)" "Test 1"
        "2)" "Test 2"
        "3)" "Exit"
    )
    choices=$("${selection[@]}" "${options[@]}" 2>&1 > /dev/tty1)
    for choice in $choices; do
        case $choice in
            "1)") ;;
            "2)") ;;
            "3)") exit ;;
        esac
    done
done
