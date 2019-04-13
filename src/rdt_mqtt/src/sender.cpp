#include "ros/ros.h"
#include "rdt_mqtt/PackedMessage.h"
#include "mqtt/client.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

const std::string ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("MQTT_Bridge");

//mqtt::client cli;


class in_mem_persistence : virtual public mqtt::iclient_persistence {
  bool openStatus;

  std::map<std::string, std::string> storage;

public:
  in_mem_persistence() : openStatus(false) {}

  void open(const std::string &clientID, const std::string &server) override {
  }

  void close() override {
  }

  void clear() override {
    storage.clear();
  }

  bool contains_key(const std::string &key) override {
    return storage.find(key) != storage.end();
  }

  const mqtt::string_collection &keys() const override {
    static mqtt::string_collection ks;
    ks.clear();
    for (const auto &k : storage) {
      ks.push_back(k.first);
    }
    return ks;
  }

  void put(const std::string &key,
           const std::vector<mqtt::string_view> &bufs) override {
    std::string holder;
    for (const auto &b : bufs) {
      holder += b.str();
    }
    storage[key] = std::move(holder);
  }

  mqtt::string_view get(const std::string &key) const override {
    auto item = storage.find(key);
    if (item == storage.end()) {
      throw mqtt::persistence_exception();
    }
    return mqtt::string_view(item->second);
  }

  void remove(const std::string &key) override {
    auto item = storage.find(key);
    if (item == storage.end()) {
      throw mqtt::persistence_exception();
    }
    storage.erase(item);
  }
};


class user_callback : public virtual mqtt::callback {
  void connection_lost(const std::string &cause) override {
    std::string error;
    if (cause.empty()) {
      error = "idk";
    } else {
      error = cause;
    }
	std::stringstream ss;
    ss << "Lost connection b/c of: " << error << std::endl;
	ROS_INFO("%s", ss.str().c_str());
  }

  void delivery_complete(mqtt::delivery_token_ptr token) override {
    std::stringstream ss; 
	ss << "Delivered: " << (token ? token->get_message_id() : -1)
              << std::endl;
	ROS_INFO("%s", ss.str().c_str());
  }

public:
};

in_mem_persistence persist;
mqtt::client cli(ADDRESS, CLIENT_ID, &persist);

void bridgeSender(const rdt_mqtt::PackedMessage::ConstPtr& msg){
    try {
        std::string topic = msg->channel_name;
        std::string contents = msg->field;
        int value = msg->value;

        std::stringstream finTopic;
        finTopic << topic << "/" << contents;
        ROS_INFO("SENDING %s", finTopic.str().c_str());
        cli.publish(mqtt::message(finTopic.str(), std::to_string(value), 0, true));
        ROS_INFO("SENT %s", finTopic.str().c_str());
    }
    catch (const mqtt::exception &ex){
        ROS_INFO("%s", ex.what());
    }
}


int main(int argc, char* argv[]){
    ros::init(argc, argv, "sender");
	ROS_INFO("Starting");
    user_callback cb;
    cli.set_callback(cb);

    mqtt::connect_options options;
    options.set_keep_alive_interval(20);
    options.set_clean_session(false);
    
    try { 
        cli.connect(options);
    }
    catch(const mqtt::exception &ex){
        ROS_INFO("Missed connection %s", ex.what());
    }
    
    ros::NodeHandle node;
    ros::Subscriber sub = node.subscribe("bridge", 1000, bridgeSender);
    ros::spin(); 
    return 0;
}
