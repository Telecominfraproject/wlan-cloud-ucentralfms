cmake_minimum_required(VERSION 3.19)
project(ucentralfws VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build)
    file(READ build BUILD_NUM)
    if(BUILD_INCREMENT)
        MATH(EXPR BUILD_NUM "${BUILD_NUM}+1")
        file(WRITE build ${BUILD_NUM})
    endif()
else()
    set(BUILD_NUM 1)
    file(WRITE build ${BUILD_NUM})
endif()
add_definitions(-DAPP_VERSION="${CMAKE_PROJECT_VERSION}" -DBUILD_NUMBER="${BUILD_NUM}")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)
find_package(Poco REQUIRED COMPONENTS Crypto JWT Net Util NetSSL Data DataSQLite)

include_directories(/usr/local/include  /usr/local/opt/openssl/include src include/kafka)

add_executable(ucentralfws
                    build
                    src/FirmwareDS.cpp src/FirmwareDS.h src/RESTAPI_objects.h src/RESTAPI_objects.cpp
                    src/uStorageService.cpp src/uStorageService.h src/storage_tables.cpp
                    src/storage_sqlite.cpp src/SubSystemServer.cpp src/SubSystemServer.h
                    src/uAuthService.h src/uAuthService.cpp src/RESTAPI_handler.cpp src/RESTAPI_handler.h
                    src/utils.cpp src/utils.h)

target_link_libraries(ucentralfws PUBLIC
        ${Poco_LIBRARIES} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})
