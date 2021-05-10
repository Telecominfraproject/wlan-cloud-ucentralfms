#!/bin/bash

key="0T1jZ0zO9fkvWI8pk722ZAeSKmqey1jiDXNafRlpdiy4lletCCaVAgWdSH5LD5f8"

curl -v -H "X-API-KEY: $key" \
	-F upload=@latest.json \
	-F upload=@20210508-linksys_ea8300-uCentral-trunk-43e1a2d-upgrade.bin \
	https://local.dpaas.arilia.com:15065
