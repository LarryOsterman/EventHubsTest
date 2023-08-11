# Notes on creating this sample.

Install Visual Studio 2022. The version this was built with is:

- Visual Studio 17.0.
  - C++ Workload
  - cmake support
  - vcpkg package manager installed.
- Azure CLI (az) installed.

## Create your Azure resources.

### Create your EventHubs instance.

Create an EventHubs instance in Azure. You can do this via the portal or via the CLI.

Login to the CLI:
```pwsh
az login
```

Create a resource group:
```pwsh
az group create --name <your group name> --location <your location> --subscription <your subscription>
```

This should output something like:
```json
{
  "id": "/subscriptions/<your subscription ID>/resourceGroups/<your group name>",
  "location": "<your location>",
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
 az eventhubs namespace create --resource-group <your group name> --name <your namespace name> --sku Standard  --subscription <your subscription>
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
  "metricId": "REDACTED",
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
  "id": "/subscriptions/<your subscription>/resourceGroups/<your group name>/providers/Microsoft.EventHub/namespaces/<your namespace name>/eventhubs/<your eventhub name>",
  "location": "westus",
  "messageRetentionInDays": 7,
  "name": "<your eventhub name>",
  "partitionCount": 4,
  "partitionIds": [
    "0",
    "1",
    "2",
    "3"
  ],
  "resourceGroup": "<your group name>",
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

The following will retrieve the connection string for the event hub:

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

You'll also need to create an environment variable named `EVENTHUB_NAME` whose value is the name of the event hub you just created.


### Create a storage account.

Create a storage account in Azure. You can do this via the portal or via the CLI.

Using the CLI
Create a storage account:
```pwsh
az storage account create --name <your storage account name> --resource-group <your resource group> --location westus --sku Standard_LRS
```

This should output something like:







## Create the sample application.

### Setting up cmake.

Start by creating a cmake project in visual studio.

This will create a CMakeLists.txt file and a sample application, for this repository, the sample application is named `EventHubsTest`.

You next need to create a vcpkg.json file in the same directory as the CMakeLists.txt file. Attached is the vcpkg.json file that was used for this project.

It declares the azure-storage-blobs-cpp, azure-identity-cpp, and azure-messaging-eventhubs-cpp packages as dependencies.

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

You can now open the CMakeLists.txt file and add the following lines to it:

```cmake
find_package(azure-storage-blobs-cpp CONFIG REQUIRED)
find_package(azure-identity-cpp CONFIG REQUIRED)
find_package(azure-messaging-eventhubs-cpp CONFIG REQUIRED)
```

This will find the packages that you declared as dependencies in the vcpkg.json file.

You can now add the following lines to the CMakeLists.txt file:

```cmake
    target_link_libraries(${PROJECT_NAME} PRIVATE azure::storage::blobs azure::identity azure::messaging::eventhubs)
```

This ensures that the storage, identity and eventhubs libraries and headers will be available to your application.

### Building out the eventhubs client application.

Everything Eventhubs related starts with either a 
[Consumer Client](https://azuresdkdocs.blob.core.windows.net/$web/cpp/azure-messaging-eventhubs/latest/class_azure_1_1_messaging_1_1_event_hubs_1_1_consumer_client.html)
or a [Producer Client](https://azuresdkdocs.blob.core.windows.net/$web/cpp/azure-messaging-eventhubs/latest/class_azure_1_1_messaging_1_1_event_hubs_1_1_producer_client.html).

The Producer Client is responsible for producing events to the Event Hub, while the Consumer Client is responsible for consuming events from the Event Hub.

Both the Producer Client and Consumer Clients are initialized with a connection string and eventhub name.

Alternatively, instead of a connection string, you can use a Azure::Core::Credentials::TokenCredential object
to connect the producer or consumer to the service.

#### Creating the client objects.
Producer Client:
```cpp
Azure::Messaging::EventHubs::ProducerClient producerClient{
	std::getenv("EVENTHUBS_CONNECTION_STRING"), std::getenv("EVENTHUB_NAME")};
```

Consumer Client:
```cpp
Azure::Messaging::EventHubs::ConsumerClient consumerClient{
	std::getenv("EVENTHUBS_CONNECTION_STRING"), std::getenv("EVENTHUB_NAME")};
```

#### Producer Client.

For the producer client, you want to use an `[EventDataBatch](https://azuresdkdocs.blob.core.windows.net/$web/cpp/azure-messaging-eventhubs/latest/class_azure_1_1_messaging_1_1_event_hubs_1_1_event_data_batch.html) to collect events to be sent to the Event Hub.

```cpp
  // By default, the producer will round-robin amongst all available partitions. You can use the
  // same producer instance to send to a specific partition.
  // To do so, specify the partition ID in the options when creating the batch.
  //
  // The event consumer sample reads from the 0th partition ID in the eventhub properties, so
  // configure this batch processor to send to that partition.
  Azure::Messaging::EventHubs::EventDataBatchOptions batchOptions;
  batchOptions.PartitionId = eventhubProperties.PartitionIds[0];

  Azure::Messaging::EventHubs::EventDataBatch batch{producerClient.CreateEventDataBatch(batchOptions)};
```

Once you have an `EventDataBatch` object, you an add messages to the event:

```cpp
  // Send an event with a body initialized at EventData constructor time.
  {
    Azure::Messaging::EventHubs::Models::EventData event{1, 1, 2, 3, 5, 8};
    batch.AddMessage(event);
  }
```

When you have collected messages to send, you send the event data to the Event Hub:

```cpp
  // Send the batch of events to the event hub.
  producerClient.SendEventDataBatch(batch);
```

#### Consumer Client.

To consume events from the Event Hub, you need to create a `PartitionClient` object. 
This object is used to receive messages from a given eventhubs partition.

```cpp
  // Create a PartitionClient object to receive events from the partition.
Azure::Messaging::EventHubs::PartitionClient partitionClient{consumerClient.CreatePartitionClient(eventhubProperties.PartitionIds[0]
```

By default, the partition client will start reading from the END of the partition, and thus will wait for new messages to be received.
If you want to receive messages from a different time, you set the start position on the partition client:

```cpp
  // This partition client is configured to read events from the start of the partition, since the
  // default is to read new events only.
  Azure::Messaging::EventHubs::PartitionClientOptions partitionClientOptions;
  partitionClientOptions.StartPosition.Earliest = true;
  partitionClientOptions.StartPosition.Inclusive = true;

  Azure::Messaging::EventHubs::PartitionClient partitionClient{consumerClient.CreatePartitionClient(
      eventhubProperties.PartitionIds[0], partitionClientOptions)};
  // Read up to 10 events from the partition.
  auto events{partitionClient.ReceiveEvents(10)};
```
