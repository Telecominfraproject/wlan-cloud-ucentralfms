cmake_minimum_required(VERSION 3.13)
project(ucentralfms VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
    set(MYSQL_ROOT_DIR /usr/local/opt/mysql-client)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
endif()

if(UNIX AND NOT APPLE)
    set(PostgreSQL_TYPE_INCLUDE_DIR /usr/include/postgresql)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
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

find_package(Boost      REQUIRED system)
find_package(OpenSSL    REQUIRED)
find_package(AWSSDK     REQUIRED COMPONENTS s3)
find_package(Poco       REQUIRED COMPONENTS Crypto JWT Net Util NetSSL Data DataSQLite)

if(SMALL_BUILD)
    find_package(Poco REQUIRED COMPONENTS Crypto JWT Net Util NetSSL Data DataSQLite)
else()
    find_package(CppKafka REQUIRED)
    find_package(PostgreSQL REQUIRED)
    find_package(MySQL REQUIRED)
    find_package(Poco REQUIRED COMPONENTS JSON Crypto JWT Net Util NetSSL Data DataSQLite DataPostgreSQL DataMySQL)
endif()

include_directories(/usr/local/include  /usr/local/opt/openssl/include src include/kafka /usr/local/opt/mysql-client/include)

add_executable( ucentralfms
                build
                src/Daemon.cpp src/Daemon.h
                src/StorageService.cpp src/StorageService.h
                src/storage_tables.cpp src/storage_sqlite.cpp
                src/SubSystemServer.cpp src/SubSystemServer.h
                src/RESTAPI_handler.cpp src/RESTAPI_handler.h
                src/storage_firmwares.cpp
                src/storage_mysql.cpp src/storage_pgql.cpp
                src/Utils.cpp src/Utils.h
                src/RESTAPI_server.cpp src/RESTAPI_server.h
                src/RESTAPI_firmwaresHandler.cpp src/RESTAPI_firmwaresHandler.h
                src/RESTAPI_firmwareHandler.cpp src/RESTAPI_firmwareHandler.h
                src/RESTAPI_GWobjects.cpp src/RESTAPI_GWobjects.h
                src/ALBHealthCheckServer.h
                src/ManifestCreator.cpp src/ManifestCreator.h
                src/KafkaManager.cpp src/KafkaManager.h
                src/MicroService.h src/MicroService.cpp
                src/AuthClient.h src/AuthClient.cpp
                src/RESTAPI_SecurityObjects.cpp src/RESTAPI_SecurityObjects.h
                src/RESTAPI_system_command.cpp src/RESTAPI_system_command.h
                src/OpenAPIRequest.h src/OpenAPIRequest.cpp
                src/RESTAPI_InternalServer.cpp src/RESTAPI_InternalServer.h
                src/RESTAPI_utils.cpp src/RESTAPI_utils.h
                src/RESTAPI_FMSObjects.cpp src/RESTAPI_FMSObjects.h
                src/storage_firmwares.h src/storage_history.cpp
                src/storage_history.h src/storage_deviceTypes.cpp
                src/storage_deviceTypes.h src/RESTAPI_historyHandler.cpp src/RESTAPI_historyHandler.h src/NewConnectionHandler.cpp src/NewConnectionHandler.h
                src/LatestFirmwareCache.cpp src/LatestFirmwareCache.h
                src/DeviceCache.cpp src/DeviceCache.h
                src/RESTAPI_firmwareAgeHandler.cpp src/RESTAPI_firmwareAgeHandler.h
                src/storage_deviceInfo.cpp src/storage_deviceInfo.h
                src/RESTAPI_connectedDevicesHandler.cpp src/RESTAPI_connectedDevicesHandler.h
                src/RESTAPI_connectedDeviceHandler.cpp src/RESTAPI_connectedDeviceHandler.h src/RESTAPI_deviceReportHandler.cpp src/RESTAPI_deviceReportHandler.h)

target_link_libraries(ucentralfms PUBLIC
        ${Poco_LIBRARIES} ${MySQL_LIBRARIES}
        ${Boost_LIBRARIES}
        ${ZLIB_LIBRARIES} ${AWSSDK_LINK_LIBRARIES}
        CppKafka::cppkafka )

