<p align=center><img src=images/project/logo.svg?sanitize=true/ width="200px" height="200px"></p>

# uCentralFMS

## What is this?
The uCentralFMS is a micro-service part of the OpenWiFi ecosystem. uCentralFMS is a Firmware Management Service
to facilitate the task of upgrade and maintaining the proper firmware for all the devices 
used in your OpenWiFi solution. You may either [build it](#building) or use the [Docker version](#docker).

## OpenAPI
You may get static page with OpenAPI docs generated from the definition on [GitHub Page](https://telecominfraproject.github.io/wlan-cloud-ucentralfms/).

Also you may use [Swagger UI](https://petstore.swagger.io/#/) with OpenAPI definition file raw link (i.e. [latest version file](https://raw.githubusercontent.com/Telecominfraproject/wlan-cloud-ucentralfms/main/openapi/owfms.yaml)) to get interactive docs page.

## Building
In order to build the uCentralFMS, you will need to install its dependencies, which includes the following:
- cmake
- boost
- POCO 1.10.1 or later
- a C++17 compiler
- openssl
- libpq-dev (PortgreSQL development libraries)
- mysql-client (MySQL client)
- librdkafka
- cppkafka

Building is a 2 part process. The first part is to build a local copy of the framework tailored to your environment. This
framework is [Poco](https://github.com/pocoproject/poco). The version used in this project has a couple of fixes
from the master copy needed for cmake. Please use the version of this [Poco fix](https://github.com/stephb9959/poco). Building
Poco may take several minutes depending on the platform you are building on.

### Ubuntu
These instructions have proven to work on Ubuntu 20.4.
```
sudo apt install git cmake g++ libssl-dev libmariabd-dev unixodbc-dev 
sudo apt install libpq-dev libaprutil1-dev apache2-dev libboost-all-dev
sudo apt install librdkafka-dev liblua5.3-dev

git clone https://github.com/stephb9959/poco
cd poco
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --config Release
sudo cmake --build . --target install

git clone https://github.com/stephb9959/cppkafka
cd cppkafka
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --config Release
sudo cmake --build . --target install

cd ~
git clone https://github.com/Telecominfraproject/wlan-cloud-ucentralgw
cd wlan-cloud-ucentralgw
mkdir cmake-build
cd cmake-build
cmake ..
make
```

### Fedora
The following instructions have proven to work on Fedora 33
```
sudo yum install cmake g++ openssl-devel unixODBC-devel mysql-devel mysql apr-util-devel boost boost-devel
sudo yum install yaml-cpp-devel lua-devel 
sudo dnf install postgresql.x86_64 librdkafka-devel
sudo dnf install postgresql-devel

git clone https://github.com/stephb9959/poco
cd poco
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --config Release
sudo cmake --build . --target install

git clone https://github.com/stephb9959/cppkafka
cd cppkafka
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --config Release
sudo cmake --build . --target install

cd ~
git clone https://github.com/Telecominfraproject/wlan-cloud-ucentralfms
cd wlan-cloud-ucentralfms
mkdir cmake-build
cd cmake-build
cmake ..
make

```

### OSX Build
The following instructions have proven to work on OSX Big Sur. You need to install [Homebrew](https://brew.sh/). You must also have installed [XCode for OS X](https://www.freecodecamp.org/news/how-to-download-and-install-xcode/).
```
brew install openssl
brew install cmake
brew install libpq
brew install mysql-client
brew install apr
brew install apr-util
brew install boost
brew install yaml-cpp
brew install postgresql
brew install unixodbc
brew install librdkafka

git clone https://github.com/stephb9959/poco
cd poco
mkdir cmake-build 
cd cmake-build
cmake ..
cmake --build . --config Release -j
sudo cmake --build . --target install

git clone https://github.com/stephb9959/cppkafka
cd cppkafka
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --config Release
sudo cmake --build . --target install

cd ~
git clone https://github.com/Telecominfraproject/wlan-cloud-ucentralfms
cd wlan-cloud-ucentralfms
mkdir cmake-build
cd cmake-build
cmake ..
make -j
```

### After completing the build
After completing the build, you can remove the Poco source as it is no longer needed.

#### Expected directory layout
From the directory where your cloned source is, you will need to create the `certs`, `logs`, and `data`.
```shell
mkdir certs
mkdir logs
mkdir data
```

### Certificates
Love'em of hate'em, we gotta use'em. So we tried to make this as easy as possible for you.

#### The `certs` directory
For all deployments, you will need the following `certs` directory, populated with the proper files.

```asm
certs ---+--- 
         +--- restapi-ca.pem
         +--- restapi-cert.pem
         +--- restapi-key.pem
```

### Configuration
The configuration is kep in the file `owfms.properties`. This is a text file read by the service at startup time.

#### Basic configuration
You must set the environment variables:
- OWFMS_ROOT: represents where the root of the installation is for this service.
- OWFMS_CONFIG: represents the path where the configuration is kept.

#### The file section
#### RESTAPI
```json
openwifi.restapi.host.0.backlog = 100
openwifi.restapi.host.0.security = relaxed
openwifi.restapi.host.0.rootca = $OWFMS_ROOT/certs/restapi-ca.pem
openwifi.restapi.host.0.address = *
openwifi.restapi.host.0.port = 16004
openwifi.restapi.host.0.cert = $OWFMS_ROOT/certs/restapi-cert.pem
openwifi.restapi.host.0.key = $OWFMS_ROOT/certs/restapi-key.pem
openwifi.restapi.host.0.key.password = mypassword
```
Of importance are the `.port` which should point to the port used.

#### Internal microservice interface
```json
openwifi.internal.restapi.host.0.backlog = 100
openwifi.internal.restapi.host.0.security = relaxed
openwifi.internal.restapi.host.0.rootca = $OWFMS_ROOT/certs/restapi-ca.pem
openwifi.internal.restapi.host.0.address = *
openwifi.internal.restapi.host.0.port = 17004
openwifi.internal.restapi.host.0.cert = $OWFMS_ROOT/certs/restapi-cert.pem
openwifi.internal.restapi.host.0.key = $OWFMS_ROOT/certs/restapi-key.pem
openwifi.internal.restapi.host.0.key.password = mypassword
```
You can leave all the default values for this one. 

#### System values
In the following values, you need to change `.uri.public` and `uri.ui`. The `.uri.public` must point to an externally available FQDN to access the service. The `.uri.ui` must point to web server running 
the UI for the service. `firmwaredb.refresh` tells the service how often to refresh the firmware database in seconds. `firmwaredb.maxage` tells the service how old you
want to accept release for. This value is in days.

```json
openwifi.service.key = $OWFMS_ROOT/certs/restapi-key.pem
openwifi.service.key.password = mypassword
openwifi.system.data = $OWFMS_ROOT/data
openwifi.system.debug = false
openwifi.system.uri.private = https://localhost:17004
openwifi.system.uri.public = https://ucentral.dpaas.arilia.com:16004
openwifi.system.commandchannel = /tmp/app.owfms
openwifi.system.uri.ui = ucentral-ui.arilia.com
firmwaredb.refresh = 1800
firmwaredb.maxage = 90
```

#### S3 configuration
The service mua read the information about firmware from an Amazon S3 Bucket. You need to replace `s3.secret` and `s3.key` with your own. 

```json
s3.bucketname = ucentral-ap-firmware
s3.region = us-east-1
s3.secret = *******************************************
s3.key =  *******************************************
s3.retry = 60
s3.bucket.uri = ucentral-ap-firmware.s3.amazonaws.com

```
