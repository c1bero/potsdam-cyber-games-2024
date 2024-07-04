#!/bin/bash

set -ex

set +x
if [ -z "$FLAG" ]; then
    export FLAG="Missing flag, please contact support."
fi
set -x

# Start lighttpd
lighttpd -f /etc/lighttpd/lighttpd.conf

# Start challenge
exec su --whitelist-environment="FLAG" -l ctf -c "socat TCP-LISTEN:1337,fork EXEC:'/usr/local/bin/treadmill'"
