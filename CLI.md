# FMS CLI Documentation

## Before using the CLI
You must set the environment variable `UCENTRALSEC`. You must specify the host and port for the security service 
associated with the FMS Service. Here is an example
```csh
export UCENTRALSEC=mysecurityservice,example.com:16001
```
Once set, you can start using the `CLI`.

## General format
Most commands will take from 0 to 2 parameters. You should include all parameters in double quotes when possible.

## The commands

### `cli getfirmwares <device_type>`
This will list all firmwares that apply to the `device_type`. You can get a list of `device_types` with the `cli devicetypes` command.

### `latestfirmware <device_type>`
Get the latest firmware version for a given `device_type`.

### `cli revisions`
Get the list of currently available revisions.

### `cli devicetypes`
Retrieve the list of known `device_types`

### `cli firmwareage <device_type> <revision>`
If you specify your `device_type` and `revision`, the system will do its best to estimate how 
far in the past you `revision` is compared to the latest revision.

### `cli gethistory <serialNumber>`
Get the revision history for a given device.

### `cli connecteddevices`
Get a list of the currently known devices and the last connection information we have about the,

### `cli connecteddevice <serialNumber>`
Get the information relevant to a specific device.

### `cli devicereport`
Give a simplified dashboard report of the data in the service.

### `cli fmsversion`
Display the version of the service.

### `cli fmstimes`
Display the uptime and start time of the service.
