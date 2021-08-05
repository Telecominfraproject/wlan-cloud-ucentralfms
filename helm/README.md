# ucentralfms

This Helm chart helps to deploy uCentralSec to the Kubernetes clusters. It is mainly used in [assembly chart](https://github.com/Telecominfraproject/wlan-cloud-ucentral-deploy/tree/main/chart) as uCentralSec requires other services as dependencies that are considered in that Helm chart. This chart is purposed to define deployment logic close to the application code itself and define default values that could be overriden during deployment.


## TL;DR;

```bash
$ helm install .
```

## Introduction

This chart bootstraps an ucentralfms on a [Kubernetes](http://kubernetes.io) cluster using the [Helm](https://helm.sh) package manager.

## Installing the Chart

Currently this chart is not assembled in charts archives, so [helm-git](https://github.com/aslafy-z/helm-git) is required for remote the installation

To install the chart with the release name `my-release`:

```bash
$ helm install --name my-release git+https://github.com/Telecominfraproject/wlan-cloud-ucentralfms@helm?ref=main
```

The command deploys ucentralfms on the Kubernetes cluster in the default configuration. The [configuration](#configuration) section lists the parameters that can be configured during installation.

> **Tip**: List all releases using `helm list`

## Uninstalling the Chart

To uninstall/delete the `my-release` deployment:

```bash
$ helm delete my-release
```

The command removes all the Kubernetes components associated with the chart and deletes the release.

## Configuration

The following table lists the configurable parameters of the chart and their default values. If Default value is not listed in the table, please refer to the [Values](values.yaml) files for details.

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| replicaCount | number | Amount of replicas to be deployed | `1` |
| strategyType | string | Application deployment strategy | `'Recreate'` |
| nameOverride | string | Override to be used for application deployment |  |
| fullnameOverride | string | Override to be used for application deployment (has priority over nameOverride) |  |
| images.ucentralfms.repository | string | Docker image repository |  |
| images.ucentralfms.tag | string | Docker image tag | `'master'` |
| images.ucentralfms.pullPolicy | string | Docker image pull policy | `'Always'` |
| services.ucentralfms.type | string | uCentralSec service type | `'LoadBalancer'` |
| services.ucentralfms.ports.restapi.servicePort | number | REST API endpoint port to be exposed on service | `16001` |
| services.ucentralfms.ports.restapi.targetPort | number | REST API endpoint port to be targeted by service | `16001` |
| services.ucentralfms.ports.restapi.protocol | string | REST API endpoint protocol | `'TCP'` |
| services.ucentralfms.ports.restapiinternal.servicePort | string | Internal REST API endpoint port to be exposed on service | `17001` |
| services.ucentralfms.ports.restapiinternal.targetPort | number | Internal REST API endpoint port to be targeted by service | `17001` |
| services.ucentralfms.ports.restapiinternal.protocol | string | Internal REST API endpoint protocol | `'TCP'` |
| checks.ucentralfms.liveness.httpGet.path | string | Liveness check path to be used | `'/'` |
| checks.ucentralfms.liveness.httpGet.port | number | Liveness check port to be used (should be pointint to ALB endpoint) | `16101` |
| checks.ucentralfms.readiness.httpGet.path | string | Readiness check path to be used | `'/'` |
| checks.ucentralfms.readiness.httpGet.port | number | Readiness check port to be used (should be pointint to ALB endpoint) | `16101` |
| ingresses.restapi.enabled | boolean | Defines if REST API endpoint should be exposed via Ingress controller | `False` |
| ingresses.restapi.hosts | array | List of hosts for exposed REST API |  |
| ingresses.restapi.paths | array | List of paths to be exposed for REST API |  |
| volumes.ucentralfms | array | Defines list of volumes to be attached to uCentralSec |  |
| persistence.enabled | boolean | Defines if uCentralSec requires Persistent Volume (required for permanent files storage and SQLite DB if enabled) | `True` |
| persistence.accessModes | array | Defines PV access modes |  |
| persistence.size | string | Defines PV size | `'10Gi'` |
| public_env_variables | hash | Defines list of environment variables to be passed to uCentralSec | |
| configProperties | hash | Configuration properties that should be passed to the application in `ucentralfms.properties`. May be passed by key in set (i.e. `configProperties."rtty\.token"`) | |
| certs | hash | Defines files (keys and certificates) that should be passed to uCentralSec (PEM format is adviced to be used) (see `volumes.ucentralfms` on where it is mounted) |  |


Specify each parameter using the `--set key=value[,key=value]` argument to `helm install`. For example,

```bash
$ helm install --name my-release \
  --set replicaCount=1 \
    .
```

The above command sets that only 1 instance of your app should be running

Alternatively, a YAML file that specifies the values for the parameters can be provided while installing the chart. For example,

```bash
$ helm install --name my-release -f values.yaml .
```

> **Tip**: You can use the default [values.yaml](values.yaml) as a base for customization.


