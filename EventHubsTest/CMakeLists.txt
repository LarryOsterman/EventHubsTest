# CMakeList.txt : CMake project for EventHubsTest, include source and define
# project specific logic here.
#
find_package(azure-messaging-eventhubs-cpp CONFIG REQUIRED)
find_package(azure-identity-cpp CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (EventHubsTest "EventHubsTest.cpp" "EventHubsTest.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EventHubsTest PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(EventHubsTest PRIVATE Azure::azure-messaging-eventhubs Azure::azure-identity)

