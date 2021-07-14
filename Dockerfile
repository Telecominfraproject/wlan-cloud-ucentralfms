FROM alpine AS builder

RUN apk update && \
    apk add --no-cache openssl openssh && \
    apk add --no-cache ncurses-libs && \
    apk add --no-cache bash util-linux coreutils curl && \
    apk add --no-cache make cmake gcc g++ libstdc++ libgcc git && \
    apk add --no-cache openssl-dev boost-dev curl-dev util-linux-dev zlib-dev  && \
    apk add --no-cache librdkafka-dev

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

ADD CMakeLists.txt /ucentralfms/
ADD src /ucentralfms/src

WORKDIR /ucentralfms
RUN mkdir cmake-build
WORKDIR /ucentralfms/cmake-build
RUN cmake ..
RUN cmake --build . --config Release -j8

FROM alpine

RUN addgroup -S ucentralfms && adduser -S -G ucentralfms ucentralfms

RUN mkdir /ucentral
RUN mkdir /ucentralfms-data
RUN apk add --update --no-cache librdkafka curl-dev 

COPY --from=builder /ucentralfms/cmake-build/ucentralfms /ucentral/ucentralfms
COPY --from=builder /cppkafka/cmake-build/src/lib/* /lib/
COPY --from=builder /poco/cmake-build/lib/* /lib/
COPY --from=builder /aws-sdk-cpp/cmake-build/aws-cpp-sdk-core/libaws-cpp-sdk-core.so /lib/
COPY --from=builder /aws-sdk-cpp/cmake-build/aws-cpp-sdk-s3/libaws-cpp-sdk-s3.so /lib/

EXPOSE 16004
EXPOSE 17004
EXPOSE 16104

USER ucentralfms
ENTRYPOINT /ucentral/ucentralfms
