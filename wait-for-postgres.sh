#!/bin/bash
# wait-for-postgres.sh

set -e
  
host="$1"
shift

export PGUSER=$(grep 'storage.type.postgresql.username' $OWFMS_CONFIG/owfms.properties | awk -F '= ' '{print $2}')
export PGPASSWORD=$(grep 'storage.type.postgresql.password' $OWFMS_CONFIG/owfms.properties | awk -F '= ' '{print $2}')
  
until psql -h "$host" -c '\q'; do
  >&2 echo "Postgres is unavailable - sleeping"
  sleep 1
done
  
>&2 echo "Postgres is up - executing command"

if [ "$1" = '/openwifi/owfms' -a "$(id -u)" = '0' ]; then
    if [ "$RUN_CHOWN" = 'true' ]; then
      chown -R "$OWFMS_USER": "$OWFMS_ROOT" "$OWFMS_CONFIG"
    fi
    exec gosu "$OWFMS_USER" "$@"
fi

exec "$@"
