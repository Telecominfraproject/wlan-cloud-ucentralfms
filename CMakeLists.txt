cmake_minimum_required(VERSION 3.13)
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

set(BUILD_SHARED_LIBS 1)

add_definitions(-DAPP_VERSION="${CMAKE_PROJECT_VERSION}" -DBUILD_NUMBER="${BUILD_NUM}" -DAWS_CUSTOM_MEMORY_MANAGEMENT)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)
find_package(AWSSDK REQUIRED COMPONENTS s3)
find_package(Poco REQUIRED COMPONENTS Crypto JWT Net Util NetSSL Data DataSQLite)

include_directories(/usr/local/include  /usr/local/opt/openssl/include src include/kafka)

add_executable(ucentralfws
                    build
                    src/uFirmwareDS.cpp src/uFirmwareDS.h src/RESTAPI_objects.h src/RESTAPI_objects.cpp
                    src/uStorageService.cpp src/uStorageService.h src/storage_tables.cpp
                    src/storage_sqlite.cpp src/uSubSystemServer.cpp src/uSubSystemServer.h
                    src/uAuthService.h src/uAuthService.cpp src/RESTAPI_handler.cpp src/RESTAPI_handler.h
                    src/uUtils.cpp src/uUtils.h src/storage_callbacks.cpp src/storage_firmwares.cpp
                    src/storage_latestfirmware.cpp src/RESTAPI_server.cpp src/RESTAPI_server.h
                    src/RESTAPI_oauth2Handler.cpp src/RESTAPI_oauth2Handler.h
                    src/RESTAPI_unknownRequestHandler.cpp src/RESTAPI_unknownRequestHandler.h
                    src/RESTAPI_firmwaresHandler.cpp src/RESTAPI_firmwaresHandler.h
                    src/RESTAPI_callbacksHandler.cpp src/RESTAPI_callbacksHandler.h
                    src/RESTAPI_callbackHandler.cpp src/RESTAPI_callbackHandler.h
                    src/RESTAPI_firmwareHandler.cpp src/RESTAPI_firmwareHandler.h
                    src/RESTAPI_latestFirmwareListHandler.cpp src/RESTAPI_latestFirmwareListHandler.h
                    src/AwsNLBHealthCheck.cpp src/AwsNLBHealthCheck.h
                    src/RESTAPI_callbackChannel.cpp src/RESTAPI_callbackChannel.h
                    src/uNotificationMgr.cpp src/uNotificationMgr.h
                    src/s3bucketreader.cpp src/s3bucketreader.h src/RESTAPI_newFirmwareAvailable.cpp src/RESTAPI_newFirmwareAvailable.h src/uManifestCreator.cpp src/uManifestCreator.h)

target_link_libraries(ucentralfws PUBLIC
        ${Poco_LIBRARIES} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${AWSSDK_LINK_LIBRARIES})

# ${AWSSDK_LINK_LIBRARIES}