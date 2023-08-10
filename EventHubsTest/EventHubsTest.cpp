// EventHubsTest.cpp : Defines the entry point for the application.
//

#include "EventHubsTest.h"

#include <azure/identity/azure_cli_credential.hpp>
#include <azure/messaging/eventhubs/consumer_client.hpp>
#include <memory>

using namespace std;

int main()
{
	auto connectionStringPtr = getenv("EVENTHUB_CONNECTION_STRING");
	if (connectionStringPtr == nullptr)
	{
		cout << "EVENTHUB_CONNECTION_STRING environment variable not set" << endl;
		return 1;
	}
	auto eventHubPtr = getenv("EVENTHUB_NAME");
	if (eventHubPtr == nullptr)
	{
		cout << "EVENTHUB_NAME environment variable not set" << endl;
		return 1;
	}
	std::string connectionString{ connectionStringPtr };
	std::string eventHubName{ eventHubPtr };
	auto cred{ std::make_shared<Azure::Identity::AzureCliCredential>() };

	Azure::Messaging::EventHubs::ConsumerClient consumer(connectionString, eventHubName);

	auto partitions = consumer.GetEventHubProperties();
	cout << "Properties:" << partitions << std::endl;

	Azure::Messaging::EventHubs::PartitionClientOptions partitionClientOptions;
	partitionClientOptions.StartPosition.Earliest = true;
	partitionClientOptions.StartPosition.Inclusive = true;
	auto partitionClient = consumer.CreatePartitionClient("2", partitionClientOptions);

	auto events = partitionClient.ReceiveEvents(1);
	for (const auto& event : events)
	{
		cout << "Event: " << event << std::endl;
	}

	cout << "Hello CMake." << endl;
	return 0;
}
