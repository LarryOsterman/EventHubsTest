# Notes on creating this sample.

Install Visual Studio 2022. The version this was built with is:

- Visual Studio 17.0.
  - C++ Workload
  - cmake support
  - vcpkg package manager installed.
- Azure CLI (az) installed.

## Create your EventHubs instance.

Create an EventHubs instance in Azure. You can do this via the portal or via the CLI.

Login to the CLI:
```pwsh
az login
```

Create a resource group:
```pwsh
az group create --name eventhubs-test --location WestUS
```

This should output something like:
```json
{
  "id": "/subscriptions/<your subscription ID>/resourceGroups/<your group name>",
  "location": "westus",
  "managedBy": null,
  "name": "<yourgroup name>",
  "properties": {
    "provisioningState": "Succeeded"
  },
  "tags": null,
  "type": "Microsoft.Resources/resourceGroups"
}
```

Create an EventHubs instance:
```pwsh
 az eventhubs namespace create --resource-group LarryOExampleGroup --name LarryOtestNamespace --sku Standard  --subscription faa080af-c1d8-40ad-9cce-e1a450ca5b57
 ```

This should output something like:

```json
{
  "createdAt": "2023-08-10T18:41:54.19Z",
  "disableLocalAuth": false,
  "id": "/subscriptions/<your subscription ID>/resourceGroups/<your group name>/providers/Microsoft.EventHub/namespaces/<your namespace>",
  "isAutoInflateEnabled": false,
  "kafkaEnabled": true,
  "location": "West US",
  "maximumThroughputUnits": 0,
  "metricId": "faa080af-c1d8-40ad-9cce-e1a450ca5b57:larryotestnamespace",
  "minimumTlsVersion": "1.2",
  "name": "<your namespace name>",
  "provisioningState": "Succeeded",
  "publicNetworkAccess": "Enabled",
  "resourceGroup": "<your resource group>",
  "serviceBusEndpoint": "https://<your namespace name>.servicebus.windows.net:443/",
  "sku": {
    "capacity": 1,
    "name": "Standard",
    "tier": "Standard"
  },
  "status": "Active",
  "tags": {},
  "type": "Microsoft.EventHub/Namespaces",
  "updatedAt": "2023-08-10T18:42:41.343Z",
  "zoneRedundant": false
}
```

Create an EventHub:
```pwsh
az eventhubs eventhub create --resource-group <your resource group> --namespace-name <your namespace name> --name <your eventhub name>
```

That should output something like:
```json
{
  "createdAt": "2023-08-10T21:02:07.62Z",
  "id": "/subscriptions/faa080af-c1d8-40ad-9cce-e1a450ca5b57/resourceGroups/LarryOExampleGroup/providers/Microsoft.EventHub/namespaces/LarryOtestNamespace/eventhubs/EventHub",
  "location": "westus",
  "messageRetentionInDays": 7,
  "name": "EventHub",
  "partitionCount": 4,
  "partitionIds": [
    "0",
    "1",
    "2",
    "3"
  ],
  "resourceGroup": "LarryOExampleGroup",
  "retentionDescription": {
    "cleanupPolicy": "Delete",
    "retentionTimeInHours": 168
  },
  "status": "Active",
  "type": "Microsoft.EventHub/namespaces/eventhubs",
  "updatedAt": "2023-08-10T21:02:16.29Z"
}
```

Now that the event hub has been set up, you need to create a SAS token for it.

```pwsh
az eventhubs namespace authorization-rule keys list --resource-group <your resource group> --namespace-name <your namespace name> --name RootManageSharedAccessKey
```

```json
{
  "keyName": "RootManageSharedAccessKey",
  "primaryConnectionString": "Endpoint=sb://REDACTED.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=REDACTED",
  "primaryKey": "REDACTED",
  "secondaryConnectionString": "Endpoint=sb://REDACTED.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=REDACTED",
  "secondaryKey": "REDACTED"
}
```


The value of the primaryConnectionString is what you need to use in the sample. Save it in an environment variable named `EVENTHUBS_CONNECTION_STRING`.


## Create a storage account.

Create a storage account in Azure. You can do this via the portal or via the CLI.

Login to the CLI:
```pwsh
az login
```

Create a resource group:
```pwsh
az group create --name eventhubs-test --location WestUS
```

This should output something like:
```json
{
  "id": "/subscriptions/<your subscription ID>/resourceGroups/<your group name>",
  "location": "westus",
  "managedBy": null,
  "name": "<yourgroup name>",
  "properties": {
	"provisioningState": "Succeeded"
  },
  "tags": null,
  "type": "Microsoft.Resources/resourceGroups"
}
```

Create a storage account:
```pwsh
az storage account create --name <your storage account name> --resource-group <your resource group> --location westus --sku Standard_LRS
```

This should output something like:





## Create the package.

Start by creating a cmake project in visual studio.

This will create a CMakeLists.txt file and a sample application.

You next need to create a vcpkg.json file in the same directory as the CMakeLists.txt file.

```json
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
  "name": "eventhubs-test-application",
  "version": "0.15.2",
  "dependencies": [
    "azure-storage-blobs-cpp",
    "azure-identity-cpp",
    "azure-messaging-eventhubs-cpp"
  ],
  "builtin-baseline": "c9fa965c2a1b1334469b4539063f3ce95383653c",
  "overrides": [
    {
      "name": "curl",
      "version": "8.2.1"
    },
    {
      "name": "azure-core-cpp",
      "version": "1.10.2"
    }
  ]
}
```

The vcpkg.json in the root of this project correctly configures the dependencies.

