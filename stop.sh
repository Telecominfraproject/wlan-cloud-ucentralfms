#!/usr/bin/env bash

pidfilename="${UCENTRALFMS_ROOT}/data/pidfile"

if [[ -f "${pidfilename}" ]]
then
  kill -9 $(cat ${pidfilename})
fi
