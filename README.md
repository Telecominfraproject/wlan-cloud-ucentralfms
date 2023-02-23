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
```bash
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
```bash
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
```bash
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

### OWFMS Service Configuration
The configuration is kept in a file called `owfms.properties`. To understand the content of this file,
please look [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralfms/blob/main/CONFIGURATION.md)

## Firewall Considerations
| Port  | Description                                   | Configurable |
|:------|:----------------------------------------------|:------------:|
| 16003 | Default port for REST API Access to the OWFMS |     yes      |

## Kafka topics
Toe read more about Kafka, follow the [document](https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/main/KAFKA.md)

## Contributions
We need more contributors. Should you wish to contribute,
please follow the [contributions](https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/CONTRIBUTING.md) document.

## Pull Requests
Please create a branch with the Jira addressing the issue you are fixing or the feature you are implementing.
Create a pull-request from the branch into master.

## Additional OWSDK Microservices
Here is a list of additional OWSDK microservices
| Name | Description | Link | OpenAPI |
| :--- | :--- | :---: | :---: |
| OWSEC | Security Service | [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralsec) | [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralsec/blob/main/openpapi/owsec.yaml) |
| OWGW | Controller Service | [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralgw) | [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/openapi/owgw.yaml) |
| OWFMS | Firmware Management Service | [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralfms) | [here](https://github.com/Telecominfraproject/wlan-cloud-ucentralfms/blob/main/openapi/owfms.yaml) |
| OWPROV | Provisioning Service | [here](https://github.com/Telecominfraproject/wlan-cloud-owprov) | [here](https://github.com/Telecominfraproject/wlan-cloud-owprov/blob/main/openapi/owprov.yaml) |
| OWANALYTICS | Analytics Service | [here](https://github.com/Telecominfraproject/wlan-cloud-analytics) | [here](https://github.com/Telecominfraproject/wlan-cloud-analytics/blob/main/openapi/owanalytics.yaml) |

