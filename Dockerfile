ARG ALPINE_VERSION=3.16.2
ARG POCO_VERSION=poco-tip-v1
ARG FMTLIB_VERSION=9.0.0
ARG CPPKAFKA_VERSION=tip-v1
ARG JSON_VALIDATOR_VERSION=2.1.0
ARG AWS_SDK_VERSION=1.9.315

FROM alpine:$ALPINE_VERSION AS build-base

RUN apk add --update --no-cache \
    make cmake g++ git \
    unixodbc-dev postgresql-dev mariadb-dev \
    librdkafka-dev boost-dev openssl-dev \
    zlib-dev nlohmann-json \
    curl-dev

FROM build-base AS poco-build

ARG POCO_VERSION

ADD https://api.github.com/repos/AriliaWireless/poco/git/refs/tags/${POCO_VERSION} version.json
RUN git clone https://github.com/AriliaWireless/poco --branch ${POCO_VERSION} /poco

WORKDIR /poco
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake ..
RUN cmake --build . --config Release -j8
RUN cmake --build . --target install

FROM build-base AS fmtlib-build

ARG FMTLIB_VERSION

ADD https://api.github.com/repos/fmtlib/fmt/git/refs/tags/${FMTLIB_VERSION} version.json
RUN git clone https://github.com/fmtlib/fmt --branch ${FMTLIB_VERSION} /fmtlib

WORKDIR /fmtlib
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake ..
RUN make
RUN make install

FROM build-base AS cppkafka-build

ARG CPPKAFKA_VERSION

ADD https://api.github.com/repos/AriliaWireless/cppkafka/git/refs/tags/${CPPKAFKA_VERSION} version.json
RUN git clone https://github.com/AriliaWireless/cppkafka --branch ${CPPKAFKA_VERSION} /cppkafka

WORKDIR /cppkafka
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake ..
RUN cmake --build . --config Release -j8
RUN cmake --build . --target install

FROM build-base AS json-schema-validator-build

ARG JSON_VALIDATOR_VERSION

ADD https://api.github.com/repos/pboettch/json-schema-validator/git/refs/tags/${JSON_VALIDATOR_VERSION} version.json
RUN git clone https://github.com/pboettch/json-schema-validator --branch ${JSON_VALIDATOR_VERSION} /json-schema-validator

WORKDIR /json-schema-validator
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake ..
RUN make
RUN make install

FROM build-base AS aws-sdk-cpp-build

ARG AWS_SDK_VERSION

ADD https://api.github.com/repos/aws/aws-sdk-cpp/git/refs/tags/${AWS_SDK_VERSION} version.json
RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp --branch ${AWS_SDK_VERSION} /aws-sdk-cpp

WORKDIR /aws-sdk-cpp
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake .. -DBUILD_ONLY="sns;s3" \
             -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_CXX_FLAGS="-Wno-error=stringop-overflow -Wno-error=uninitialized" \
             -DAUTORUN_UNIT_TESTS=OFF
RUN cmake --build . --config Release -j8
RUN cmake --build . --target install

FROM build-base AS owfms-build

ADD CMakeLists.txt build /owfms/
ADD cmake /owfms/cmake
ADD src /owfms/src
ADD .git /owfms/.git

COPY --from=poco-build /usr/local/include /usr/local/include
COPY --from=poco-build /usr/local/lib /usr/local/lib
COPY --from=cppkafka-build /usr/local/include /usr/local/include
COPY --from=cppkafka-build /usr/local/lib /usr/local/lib
COPY --from=json-schema-validator-build /usr/local/include /usr/local/include
COPY --from=json-schema-validator-build /usr/local/lib /usr/local/lib
COPY --from=aws-sdk-cpp-build /usr/local/include /usr/local/include
COPY --from=aws-sdk-cpp-build /usr/local/lib /usr/local/lib
COPY --from=fmtlib-build /usr/local/include /usr/local/include
COPY --from=fmtlib-build /usr/local/lib /usr/local/lib

WORKDIR /owfms
RUN mkdir cmake-build
WORKDIR /owfms/cmake-build
RUN cmake .. \
          -Dcrypto_LIBRARY=/usr/lib/libcrypto.so \
          -DBUILD_SHARED_LIBS=ON
RUN cmake --build . --config Release -j8

FROM alpine:$ALPINE_VERSION

ENV OWFMS_USER=owfms \
    OWFMS_ROOT=/owfms-data \
    OWFMS_CONFIG=/owfms-data

RUN addgroup -S "$OWFMS_USER" && \
    adduser -S -G "$OWFMS_USER" "$OWFMS_USER"

RUN mkdir /openwifi
RUN mkdir -p "$OWFMS_ROOT" "$OWFMS_CONFIG" && \
    chown "$OWFMS_USER": "$OWFMS_ROOT" "$OWFMS_CONFIG"

RUN apk add --update --no-cache librdkafka su-exec gettext ca-certificates bash jq curl \
    mariadb-connector-c libpq unixodbc postgresql-client

COPY readiness_check /readiness_check
COPY test_scripts/curl/cli /cli

COPY owfms.properties.tmpl /
COPY docker-entrypoint.sh /
COPY wait-for-postgres.sh /
RUN wget https://raw.githubusercontent.com/Telecominfraproject/wlan-cloud-ucentral-deploy/main/docker-compose/certs/restapi-ca.pem \
    -O /usr/local/share/ca-certificates/restapi-ca-selfsigned.pem

COPY --from=owfms-build /owfms/cmake-build/owfms /openwifi/owfms
COPY --from=cppkafka-build /cppkafka/cmake-build/src/lib/* /usr/local/lib
COPY --from=poco-build /poco/cmake-build/lib/* /usr/local/lib
COPY --from=aws-sdk-cpp-build /aws-sdk-cpp/cmake-build/aws-cpp-sdk-core/libaws-cpp-sdk-core.so /usr/local/lib
COPY --from=aws-sdk-cpp-build /aws-sdk-cpp/cmake-build/aws-cpp-sdk-s3/libaws-cpp-sdk-s3.so /usr/local/lib
COPY --from=aws-sdk-cpp-build /aws-sdk-cpp/cmake-build/aws-cpp-sdk-sns/libaws-cpp-sdk-sns.so /usr/local/lib

EXPOSE 16004 17004 16104

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/openwifi/owfms"]
