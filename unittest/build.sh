#!/bin/bash
CC="gcc"

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

usage()
{
    echo -e Usage: $0
    exit 0
}

OUT_DIR=../out

if ! [[ -d ${OUT_DIR} ]]; then
    wd=$(pwd)
    cd ../src && ./build.sh
    STATUS=$?
    cd ${wd}
fi

if [[ ${STATUS} -ne 0 ]]; then
    exit ${STATUS}
fi

EXE_NAME=testHmap
EXE_INCLUDES="-I../unittest"
EXE_INCLUDES+=" -I../src"
EXE_INCLUDES+=" -I../src/libs"
EXE_SOURCES="../unittest/test_hmap.c"

OUT=${OUT_DIR}/${EXE_NAME}
OBJS=${OUT_DIR}/*.o
EXE_COMPILE_FLAGS=" --debug -std=gnu99 -pedantic -Werror"
EXE_LINKER_FLAGS=
EXE_LINKER_DIRS=
EXE_LINKER_LIBS=
EXE_DEFINES=

echo -e ${YELLOW}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~${RESET}
echo -e "${BOLD_GREEN}Building ${GREEN}(${EXE_NAME})${BOLD_GREEN}...${RESET}"

${CC} ${EXE_COMPILE_FLAGS} ${EXE_DEFINES} ${EXE_INCLUDES} -fPIC -o ${OUT} ${EXE_SOURCES} ${OBJS} ${EXE_LINKER_FLAGS} ${EXE_LINKER_DIRS} ${EXE_LINKER_LIBS}
STATUS=$?
if [[ ${STATUS} -ne 0 ]]; then
    echo -e ${BOLD_RED}Build FAILED${RESET}
else
    echo -e ${BOLD_CYAN}Build Info:${RESET}
    echo -e "\t${CYAN}Output: ${YELLOW}${OUT}${RESET}"
fi
exit ${STATUS}
