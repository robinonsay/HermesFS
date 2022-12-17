#!/bin/bash

RESET="\033[0m"
BLACK="\033[0;30m"
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
PURPLE="\033[0;35m"
CYAN="\033[0;36m"
WHITE="\033[0;37m"

BOLD_BLACK="\033[1;30m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
BOLD_YELLOW="\033[1;33m"
BOLD_BLUE="\033[1;34m"
BOLD_PURPLE="\033[1;35m"
BOLD_CYAN="\033[1;36m"
BOLD_WHITE="\033[1;37m"

abspath()
{
    WD=$(pwd)
    cd $1 && echo $(pwd)
    cd ${WD}
}

PROJECT_DIR=$(abspath ../)

for lib in $(ls -d libs/*); do
    cd ${lib} && ./build.sh -o ${PROJECT_DIR}/out
    STATUS=$?
    cd ${PROJECT_DIR}/src
    if [ ${STATUS} -ne 0 ]; then
        exit 1
    fi
done

