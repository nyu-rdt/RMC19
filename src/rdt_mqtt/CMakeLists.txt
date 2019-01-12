cmake_minimum_required(VERSION 2.8.3)
project(rdt_mqtt)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  genmsg
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

add_message_files(
  FILES
  PackedMessage.msg
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES rdt-mqtt
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

include_directories(
    include ${catkin_INCLUDE_DIRS}
)

add_executable(tester src/test_talker.cpp)
target_link_libraries(tester ${catkin_LIBRARIES})
add_dependencies(tester rdt_mqtt_generate_messages_cpp)

add_executable(sender src/sender.cpp)
target_link_libraries(sender ${catkin_LIBRARIES})
target_link_libraries(sender -L/usr/local/lib/)
target_link_libraries(sender -lpaho-mqttpp3)
target_link_libraries(sender -lpaho-mqtt3a)
add_dependencies(sender rdt_mqtt_generate_messages_cpp)

