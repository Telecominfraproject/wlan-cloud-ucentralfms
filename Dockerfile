ARG DEBIAN_VERSION=11.5-slim
ARG POCO_VERSION=poco-tip-v1
ARG CPPKAFKA_VERSION=tip-v1

FROM debian:$DEBIAN_VERSION AS build-base

RUN apt-get update && apt-get install --no-install-recommends -y \
    make cmake g++ git curl zip unzip pkg-config \
    libpq-dev libmariadb-dev libmariadbclient-dev-compat \
    librdkafka-dev libboost-all-dev libssl-dev \
    zlib1g-dev nlohmann-json3-dev ca-certificates libcurl4-openssl-dev

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

FROM build-base AS owfms-build

ADD CMakeLists.txt build /owfms/
ADD cmake /owfms/cmake
ADD src /owfms/src
ADD .git /owfms/.git
RUN git clone https://github.com/microsoft/vcpkg && \
    ./vcpkg/bootstrap-vcpkg.sh && \
    ./vcpkg/vcpkg install aws-sdk-cpp[s3] json-schema-validator

COPY --from=poco-build /usr/local/include /usr/local/include
COPY --from=poco-build /usr/local/lib /usr/local/lib
COPY --from=cppkafka-build /usr/local/include /usr/local/include
COPY --from=cppkafka-build /usr/local/lib /usr/local/lib

WORKDIR /owfms
RUN mkdir cmake-build
WORKDIR /owfms/cmake-build
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake ..
RUN cmake --build . --config Release -j8

FROM debian:$DEBIAN_VERSION

ENV OWFMS_USER=owfms \
    OWFMS_ROOT=/owfms-data \
    OWFMS_CONFIG=/owfms-data

RUN useradd "$OWFMS_USER"

RUN mkdir /openwifi
RUN mkdir -p "$OWFMS_ROOT" "$OWFMS_CONFIG" && \
    chown "$OWFMS_USER": "$OWFMS_ROOT" "$OWFMS_CONFIG"

RUN apt-get update && apt-get install --no-install-recommends -y \
    librdkafka++1 gosu gettext ca-certificates bash jq curl wget \
    libmariadb-dev-compat libpq5 postgresql-client libfmt7

COPY readiness_check /readiness_check
COPY test_scripts/curl/cli /cli

COPY owfms.properties.tmpl /
COPY docker-entrypoint.sh /
COPY wait-for-postgres.sh /
RUN wget https://raw.githubusercontent.com/Telecominfraproject/wlan-cloud-ucentral-deploy/main/docker-compose/certs/restapi-ca.pem \
    -O /usr/local/share/ca-certificates/restapi-ca-selfsigned.crt

COPY --from=owfms-build /owfms/cmake-build/owfms /openwifi/owfms
COPY --from=cppkafka-build /cppkafka/cmake-build/src/lib/ /usr/local/lib/
COPY --from=poco-build /poco/cmake-build/lib/ /usr/local/lib/

RUN ldconfig

EXPOSE 16004 17004 16104

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/openwifi/owfms"]
