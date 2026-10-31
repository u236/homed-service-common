#!/bin/sh

URL="https://codeload.github.com/u236/homed-service-web/tar.gz/master"
TMP="/tmp/homed-web-frontend-update"
DIR="/usr/share/homed-web"

rm -rf ${TMP} && mkdir -p ${TMP}

printf "Get HOMEd Web frontend files..."

if curl -s ${URL} | tar -xzf - -C ${TMP}; then
    rm -rf ${DIR}/*
    cp -r ${TMP}/homed-service-web-master/deploy/data/usr/share/homed-web/* ${DIR}
    printf "OK\n"
else
    printf "FAILED\n"
fi

rm -rf ${TMP}
