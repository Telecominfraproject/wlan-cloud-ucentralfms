# Firmware Service (FMS) CLI Documentation

## Before using the CLI
You must set the environment variable `OWSEC`. You must specify the host and port for the security service 
associated with the FMS Service. Here is an example
```csh
export OWSEC=mysecurityservice.example.com:16001
```
Once set, you can start using the `CLI`.

## General format
Most commands will take from 0 to 2 parameters. You should include all parameters in double quotes when possible.

## The commands
### getfirmwares
Get a lit of firmwares.

### latestfirmware <device_type>
Get the latest firmware for the device_type specified.

### revisions
Get a list of revisions available.

### devicetypes
Get the list of device types supported.

### firmwareage <device_type> <revision>   
Calculate how out of date a specific release it.

### gethistory <device serial number>
Get the device firmware history.

### connecteddevice <device serial number> 
Get the device status.

### connectedDevices
Get the list of connected devices.

### devicereport
Get the dashboard.

### setloglevel <subsystem> <loglevel>     
Set the log level for s specific subsystem.

### getloglevels
Get the current log levels for all subsystems.

### getloglevelnames
Get the log level names available.

### getsubsystemnames
Get the list of subsystems.

### systeminfo
Get basic system information.

### reloadsubsystem <subsystem name>       
Reload the configuration for a subsystem.

