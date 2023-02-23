# OWFMS Configuration
Here is the list of parameters you can configure in the `owfms.properties` file.

## OWFMS Specific Parameters
### OWFMS behaviour
```properties
firmwaredb.refresh = 86400
firmwaredb.maxage = 90
autoupdater.enabled = true
```
#### firmwaredb.refresh
How often to refresh the FMS DB, in seconds. Should never be less than 6 hours. It does take 10-20 minutes to 
create a refresh. The default is 24 hours. 

#### firmwaredb.maxage
The maximum age of firmware kept in the DB (in days). Do not go more than 6 months. The default is 3 months.

#### autoupdater.enabled
The determins if the FMS autoupdates its database. You should leave this to `true`.


### S3 information
The actual data for all the firmware is kept in a TIP bucket. The following parameters allow you to change the bucket.
You should never need to do this unless you need to implement your own FMS server.

```properties
s3.bucketname = ucentral-ap-firmware
s3.region = us-east-1
s3.secret = *******************************************
s3.key =  *******************************************
s3.retry = 60
s3.bucket.uri = ucentral-ap-firmware.s3.amazonaws.com
```

#### s3.bucketname
The S3 bucket name.
#### s3.region
The region for this bucket.
#### s3.secret
The AWS secret for access to this S3 bucket
#### s3.key
The AWS key for access for this S3 bucket
#### s3.retry = 60
The AWS retry window in seconds.
#### s3.bucket.uri = ucentral-ap-firmware.s3.amazonaws.com
The URI to the S3 bucket

## Generic OpenWiFi SDK parameters
### REST API External parameters
These are the parameters required for the configuration of the external facing REST API server
```properties
openwifi.restapi.host.0.backlog = 100
openwifi.restapi.host.0.security = relaxed
openwifi.restapi.host.0.rootca = $OWFMS_ROOT/certs/restapi-ca.pem
openwifi.restapi.host.0.address = *
openwifi.restapi.host.0.port = 16004
openwifi.restapi.host.0.cert = $OWFMS_ROOT/certs/restapi-cert.pem
openwifi.restapi.host.0.key = $OWFMS_ROOT/certs/restapi-key.pem
openwifi.restapi.host.0.key.password = mypassword
```

#### openwifi.restapi.host.0.backlog
This is the number of concurrent REST API calls that maybe be kept in the backlog for processing. That's a good rule of thumb. Never go above 500.
#### openwifi.restapi.host.0.rootca
This is the root file of your own certificate CA in `pem` format.
#### openwifi.restapi.host.0.cert
This is your own server certificate in `pem` format..
#### openwifi.restapi.host.0.key
This is the private key associated with your own certificate in `pem` format.
#### openwifi.restapi.host.0.address
Leve this a `*` in the case you want to bind to all interfaces on your gateway host or select the address of a single interface.
#### openwifi.restapi.host.0.port
The port on which the REST API server is listening. By default, this is 16002.
#### openwifi.restapi.host.0.security
Leave this as `relaxed` for now for devices.
#### openwifi.restapi.host.0.key.password
If you key file uses a password, please enter it here.

### REST API Intra microservice parameters
The following parameters describe the configuration for the inter-microservice HTTP server. You may use the same certificate/key
you are using for your extenral server or another certificate.
```properties
openwifi.internal.restapi.host.0.backlog = 100
openwifi.internal.restapi.host.0.security = relaxed
openwifi.internal.restapi.host.0.rootca = $OWFMS_ROOT/certs/restapi-ca.pem
openwifi.internal.restapi.host.0.address = *
openwifi.internal.restapi.host.0.port = 17004
openwifi.internal.restapi.host.0.cert = $OWFMS_ROOT/certs/restapi-cert.pem
openwifi.internal.restapi.host.0.key = $OWFMS_ROOT/certs/restapi-key.pem
openwifi.internal.restapi.host.0.key.password = mypassword
```

#### openwifi.internal.host.0.backlog
This is the number of concurrent REST API calls that maybe be kept in the backlog for processing. That's a good rule of thumb. Never go above 500.
#### openwifi.internal.host.0.rootca
This is the root file of your own certificate CA in `pem` format.
#### openwifi.internal.host.0.cert
This is your own server certificate in `pem` format..
#### openwifi.internal.host.0.key
This is the private key associated with your own certificate in `pem` format.
#### openwifi.internal.host.0.address
Leve this a `*` in the case you want to bind to all interfaces on your gateway host or select the address of a single interface.
#### openwifi.internal.host.0.port
The port on which the REST API server is listening. By default, this is 17002.
#### openwifi.internal.host.0.security
Leave this as `relaxed` for now for devices.
#### openwifi.internal.host.0.key.password
If you key file uses a password, please enter it here.

### Microservice information
These are different Microservie parameters. Following is a brief explanation.
```properties
openwifi.service.key = $OWFMS_ROOT/certs/restapi-key.pem
openwifi.service.key.password = mypassword
openwifi.system.data = $OWFMS_ROOT/data
openwifi.system.uri.private = https://localhost:17004
openwifi.system.uri.public = https://ucentral.dpaas.arilia.com:16002
openwifi.system.uri.ui = https://ucentral-ui.arilia.com
openwifi.security.restapi.disable = false
openwifi.system.commandchannel = /tmp/app.ucentralfms
openwifi.autoprovisioning = true
```
#### openwifi.service.key
From time to time, the microservice must encrypt information. This is the key it should use. You may use the
same keey as you RESTAPI or your server.
#### openwifi.service.key.password
The password for the `openwifi.service.key`
#### openwifi.system.data
The location of system data. This path must exist.
#### openwifi.system.uri.private
The URI to reach the controller on the internal port.
#### openwifi.system.uri.public
The URI to reach the controller from the outside world.
#### openwifi.system.uri.ui
The URI of the UI to manage this service
#### openwifi.security.restapi.disable
This allows to disable security for internal and external API calls. This should only be used if the controller
sits behind an application load balancer that will actually do TLS. Setting this to `true` disables security.
#### openwifi.system.commandchannel
The UNIX socket command channel used by this service.
#### openwifi.autoprovisioning
Allow unknown devices to be provisioned by the system.

### ALB Support
In order to support an application load balancer health check verification, your need to provide the following parameters.
```properties
alb.enable = true
alb.port = 16104
```

### Kafka
The controller use Kafka, like all the other microservices. You must configure the kafka section in order for the
system to work.
```properties
openwifi.kafka.group.id = firmware
openwifi.kafka.client.id = firmware1
openwifi.kafka.enable = true
openwifi.kafka.brokerlist = my_Kafka.example.com:9092
openwifi.kafka.auto.commit = false
openwifi.kafka.queue.buffering.max.ms = 50
```

### openwifi.kafka.group.id
The group ID is a single word that should identify the type of service tuning. In the case `firmware`
### openwifi.kafka.client.id
The client ID is a single service within that group ID. Each participant must have a unique client ID.
### openwifi.kafka.enable
Kafka should always be enabled.
### openwifi.kafka.brokerlist
The list of servers where your Kafka server is running. Comma separated.
### openwifi.kafka.auto.commit
Auto commit flag in Kafka. Leave as `false`.
### openwifi.kafka.queue.buffering.max.ms
Kafka buffering. Leave as `50`.
### Kafka security
If you intend to use SSL, you should look into Kafka Connect and specify the certificates below.
```properties
penwifi.kafka.ssl.ca.location =
openwifi.kafka.ssl.certificate.location =
openwifi.kafka.ssl.key.location =
openwifi.kafka.ssl.key.password =
```

### DB Type
The controller supports 3 types of Database. SQLite should only be used for sites with less than 100 APs or for testing in the lab.
In order to select which database to use, you must set the `storage.type` value to sqlite, postgresql, or mysql.

```properties
storage.type = sqlite
#storage.type = postgresql
#storage.type = mysql
```

### Storage SQLite parameters
Additional parameters to set for SQLite. The only important one is `storage.type.sqlite.db` which is the database name on disk.
```properties
storage.type.sqlite.db = firmware.db
storage.type.sqlite.idletime = 120
storage.type.sqlite.maxsessions = 128
```

### Storage Postgres
Additional parameters to set if you select Postgres for your database. You must specify `host`, `username`, `password`,
`database`, and `port`.
```properties
storage.type.postgresql.maxsessions = 64
storage.type.postgresql.idletime = 60
storage.type.postgresql.host = localhost
storage.type.postgresql.username = firmware
storage.type.postgresql.password = firmware
storage.type.postgresql.database = firmware
storage.type.postgresql.port = 5432
storage.type.postgresql.connectiontimeout = 60
```

### Storage MySQL/MariaDB
Additional parameters to set if you select mysql for your database. You must specify `host`, `username`, `password`,
`database`, and `port`.
```properties
storage.type.mysql.maxsessions = 64
storage.type.mysql.idletime = 60
storage.type.mysql.host = localhost
storage.type.postgresql.username = firmware
storage.type.postgresql.password = firmware
storage.type.postgresql.database = firmware
storage.type.mysql.port = 3306
storage.type.mysql.connectiontimeout = 60
```

### Logging Parameters
The microservice provides extensive logging. If you would like to keep logging on disk, set the `logging.type = file`. If you only want
console logging, `set logging.type = console`. When selecting file, `logging.path` must exist. `logging.level` sets the
basic logging level for the entire controller. `logging.websocket` disables WebSocket logging.

```properties
logging.type = file
logging.path = $OWFMS_ROOT/logs
logging.level = information
logging.asynch = true
logging.websocket = false
```
