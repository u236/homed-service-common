#!/bin/sh

URL="https://codeload.github.com/u236/homed-service-zigbee/tar.gz/master"
TMP="/tmp/homed-zigbee-library-update"
DIR="/usr/share/homed-zigbee"

rm -rf ${TMP} && mkdir -p ${TMP}

printf "Get HOMEd ZigBee library files..."

if curl -s ${URL} | tar -xzf - -C ${TMP}; then
    rm -rf ${DIR}/*
    cp -r ${TMP}/homed-service-zigbee-master/deploy/data/usr/share/homed-zigbee/* ${DIR}
    printf "OK\n"
else
    printf "FAILED\n"
fi

rm -rf ${TMP}
