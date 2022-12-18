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
    echo -e Usage: $0 [<out-dir>]
    exit 0
}

while getopts "i:so:" OPT; do
case "${OPT}" in
    i)
        INSTALL=${OPTARG};;
    s)
        SHARED=1;;
    o)
        OUT_DIR=${OPTARG};;
    [?])
        echo -e ${BOLD_RED}UNKOWN OPTION${RESET};usage;;
esac
done

if ! [ ${OUT_DIR} ]; then
    OUT_DIR=../../../out
fi

if ! [[ -d ${OUT_DIR} ]]; then
    mkdir -p ${OUT_DIR}
fi

LIB_NAME=shm
LIB_INCLUDES="-I../shm"
LIB_SOURCES="../shm/litefs_shm.linux.c"

if [[ ${SHARED} ]]; then
    LIB_COMPILE_FLAGS="-shared"
    OUT=${OUT_DIR}/liblitefs_${LIB_NAME}.so
else
    LIB_COMPILE_FLAGS="-c"
    OUT=${OUT_DIR}/litefs_${LIB_NAME}.o

fi
LIB_COMPILE_FLAGS+=" --debug -std=gnu99 -pedantic -Werror"
LIB_LINKER_FLAGS=
LIB_LINKER_DIRS=
LIB_LINKER_LIBS=
LIB_DEFINES=

echo -e ${YELLOW}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~${RESET}
echo -e "${BOLD_GREEN}Building ${GREEN}(${LIB_NAME})${BOLD_GREEN}...${RESET}"

${CC} ${LIB_COMPILE_FLAGS} ${LIB_DEFINES} ${LIB_INCLUDES} -fPIC -o ${OUT} ${LIB_SOURCES} ${LIB_LINKER_FLAGS} ${LIB_LINKER_DIRS} ${LIB_LINKER_LIBS}
STATUS=$?
if [[ ${STATUS} -ne 0 ]]; then
    echo -e ${BOLD_RED}Build FAILED${RESET}
else
    echo -e ${BOLD_CYAN}Build Info:${RESET}
    echo -e "\t${CYAN}Output: ${YELLOW}${OUT}${RESET}"
fi
exit ${STATUS}
