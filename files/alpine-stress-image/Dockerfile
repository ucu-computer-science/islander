FROM alpine:3.5
LABEL maintainer="Remon Lam <remon@containerstack.io>"

ENV STRESS_VERSION=1.0.4

COPY stress-$STRESS_VERSION.tar.gz .

RUN apk add --update bash g++ make curl && \
  tar xzvf stress-$STRESS_VERSION.tar.gz && \
  rm stress-$STRESS_VERSION.tar.gz && \
  cd stress-${STRESS_VERSION} && \
  ./configure && make && make install && \
  apk del g++ make curl && \
  rm -rf /tmp/* /var/tmp/* /var/cache/apk/* /var/cache/distfiles/*

CMD ['/usr/local/bin/stress']