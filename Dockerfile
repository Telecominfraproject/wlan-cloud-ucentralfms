FROM alpine AS builder

RUN apk add --update --no-cache \
    openssl openssh \
    ncurses-libs \
    bash util-linux coreutils curl libcurl \
    make cmake gcc g++ libstdc++ libgcc git zlib-dev \
    openssl-dev boost-dev curl-dev util-linux-dev \
    unixodbc-dev postgresql-dev mariadb-dev \
    librdkafka-dev

RUN git clone https://github.com/stephb9959/poco /poco
RUN git clone https://github.com/stephb9959/cppkafka /cppkafka
RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp /aws-sdk-cpp

WORKDIR /aws-sdk-cpp
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake .. -DBUILD_ONLY="s3" \
             -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_CXX_FLAGS="-Wno-error=stringop-overflow -Wno-error=uninitialized" \ 
             -DAUTORUN_UNIT_TESTS=OFF
RUN cmake --build . --config Release -j8
RUN cmake --build . --target install

WORKDIR /cppkafka
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake ..
RUN cmake --build . --config Release -j8
RUN cmake --build . --target install

WORKDIR /poco
RUN mkdir cmake-build
WORKDIR cmake-build
RUN cmake ..
RUN cmake --build . --config Release -j8
RUN cmake --build . --target install

ADD CMakeLists.txt build /owfms/
ADD cmake /owfms/cmake
ADD src /owfms/src

WORKDIR /owfms
RUN mkdir cmake-build
WORKDIR /owfms/cmake-build
RUN cmake ..
RUN cmake --build . --config Release -j8

FROM alpine

ENV OWFMS_USER=owfms \
    OWFMS_ROOT=/owfms-data \
    OWFMS_CONFIG=/owfms-data

RUN addgroup -S "$OWFMS_USER" && \
    adduser -S -G "$OWFMS_USER" "$OWFMS_USER"

RUN mkdir /openwifi
RUN mkdir -p "$OWFMS_ROOT" "$OWFMS_CONFIG" && \
    chown "$OWFMS_USER": "$OWFMS_ROOT" "$OWFMS_CONFIG"
RUN apk add --update --no-cache librdkafka curl-dev mariadb-connector-c libpq unixodbc su-exec gettext ca-certificates bash jq curl

COPY --from=builder /owfms/cmake-build/owfms /openwifi/owfms
COPY --from=builder /cppkafka/cmake-build/src/lib/* /lib/
COPY --from=builder /poco/cmake-build/lib/* /lib/
COPY --from=builder /aws-sdk-cpp/cmake-build/aws-cpp-sdk-core/libaws-cpp-sdk-core.so /lib/
COPY --from=builder /aws-sdk-cpp/cmake-build/aws-cpp-sdk-s3/libaws-cpp-sdk-s3.so /lib/

COPY owfms.properties.tmpl /
COPY docker-entrypoint.sh /
RUN wget https://raw.githubusercontent.com/Telecominfraproject/wlan-cloud-ucentral-deploy/main/docker-compose/certs/restapi-ca.pem \
    -O /usr/local/share/ca-certificates/restapi-ca-selfsigned.pem

COPY readiness_check /readiness_check

EXPOSE 16004 17004 16104

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/openwifi/owfms"]
