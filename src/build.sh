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

rm ${PROJECT_DIR}/out/manifest

for lib in $(ls -d libs/*); do
    cd ${lib} && ./build.sh -o ${PROJECT_DIR}/out
    STATUS=$?
    cd ${PROJECT_DIR}/src
    if [ ${STATUS} -ne 0 ]; then
        exit 1
    fi
done

source ${PROJECT_DIR}/mission.conf
STATUS=$?
if [[ ${STATUS} -ne 0 ]]; then
    echo -e "${BOLD_RED}ERRORcould not source conf file${RESET}"
    exit ${STATUS}
fi
for app_path in $MANIFEST; do
    build_path=${app_path}/src/build.sh
    if ! [[ -f ${build_path} ]]; then
        echo -e "${BOLD_RED}ERROR: Could not find build script ${build_path}${RESET}"
        exit 1
    fi
    cd $(dirname ${build_path}) && ./$(basename ${build_path})
    cd ${PROJECT_DIR}/src
done

SOURCES="${PROJECT_DIR}/src/litefs.linux.c"
OBJS=$(ls ${PROJECT_DIR}/out/*.o)
OUT="${PROJECT_DIR}/out/litefs"
INCLUDES="-I${PROJECT_DIR}/src"
COMPILE_FLAGS="--debug -std=gnu99 -pedantic -Werror"
LINKER_FLAGS=
LINKER_DIRS=
LINKER_LIBS="-ldl"
DEFINES=
echo -e ${BOLD_BLUE}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~${RESET}
echo -e ${BOLD_GREEN}Building LiteFS...${RESET}
gcc ${COMPILE_FLAGS} ${DEFINES} ${INCLUDES} -fPIC -o ${OUT} ${SOURCES} ${OBJS} ${LINKER_FLAGS} ${LINKER_DIRS} ${LINKER_LIBS}
STATUS=$?
if [ ${STATUS} -ne 0 ]; then
    echo -e ${BOLD_RED}Build FAILED${RESET}
    exit 1
fi
echo -e ${BOLD_CYAN}Build Info:${RESET}
echo -e "\t${CYAN}EXE OUT: ${YELLOW}${OUT}${RESET}"
for lib in $(ls ${PROJECT_DIR}/out/*/*.so); do
    lib_path=$(abspath $(dirname ${lib}))
    echo ${lib_path}/$(basename ${lib}) >> ${PROJECT_DIR}/out/manifest
done

