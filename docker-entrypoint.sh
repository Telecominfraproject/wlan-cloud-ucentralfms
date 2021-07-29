#!/bin/sh
set -e

if [ "$1" = '/ucentral/ucentralfms' -a "$(id -u)" = '0' ]; then
    if [ "$RUN_CHOWN" = 'true' ]; then
      chown -R "$UCENTRALFMS_USER": "$UCENTRALFMS_ROOT" "$UCENTRALFMS_CONFIG"
    fi
    exec su-exec "$UCENTRALFMS_USER" "$@"
fi

exec "$@"
