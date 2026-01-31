URL="https://api.github.com/repositories/527469371/contents/deploy/data/usr/share/homed-zigbee"
DIR="/usr/share/homed-zigbee"

for LINK in $(curl -s ${URL} | jq -r '.[].download_url'); do
    FILE="${LINK##*/}"
    printf "Get %s..." ${FILE}
    curl -so /tmp/${FILE} ${LINK}
    if cat /tmp/${FILE} | jq &>/dev/null; then
        printf "OK\n"
        mv /tmp/${FILE} ${DIR}
    else
        printf "FAILED\n"
        break
    fi
done
