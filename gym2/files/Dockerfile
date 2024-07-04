###
# builder
FROM --platform=amd64 debian@sha256:d02c76d82364cedca16ba3ed6f9102406fa9fa8833076a609cabf14270f43dfc AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
    && true

WORKDIR /build
COPY ./Makefile /build/Makefile
COPY ./treadmill.c /build/treadmill.c
RUN make

###
# runner
FROM --platform=amd64 debian@sha256:d02c76d82364cedca16ba3ed6f9102406fa9fa8833076a609cabf14270f43dfc

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        lighttpd \
        socat \
    && apt-get clean

RUN rm -f "/var/www/html/index.lighttpd.html"
COPY ./lighttpd.conf /etc/lighttpd/lighttpd.conf
RUN adduser --disabled-password ctf

COPY ./entrypoint.sh /entrypoint.sh

COPY --from=builder --chown=root:root /build/treadmill /usr/local/bin/treadmill
COPY ./Dockerfile ./treadmill.c ./lighttpd.conf ./Makefile ./entrypoint.sh /var/www/html/

EXPOSE 80 1337
ENTRYPOINT [ "/entrypoint.sh" ]
