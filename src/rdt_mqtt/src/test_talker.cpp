#include "ros/ros.h"
#include "rdt_mqtt/PackedMessage.h"
#include <sstream>

int main(int argc, char* argv[]){
    ros::init(argc, argv, "MQTT_TestTalker");
    ros::NodeHandle node;
    
    ros::Publisher pub = node.advertise<rdt_mqtt::PackedMessage>("bridge", 1000);
    
    ros::Rate loop_rate(10);

    int number = 0;
    while(ros::ok()){
        rdt_mqtt::PackedMessage msg;
        std::stringstream ss;

        ss << "TestMessage" << number;
        msg.channel_name = "Test";
        msg.field = ss.str();
        msg.value = number;
        
        ROS_INFO("%s", msg.field.c_str());

        pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
        number++;
    }
}
