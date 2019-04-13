// Auto-generated. Do not edit!

// (in-package rdt_mqtt.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class PackedMessage {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.channel_name = null;
      this.field = null;
      this.value = null;
    }
    else {
      if (initObj.hasOwnProperty('channel_name')) {
        this.channel_name = initObj.channel_name
      }
      else {
        this.channel_name = '';
      }
      if (initObj.hasOwnProperty('field')) {
        this.field = initObj.field
      }
      else {
        this.field = '';
      }
      if (initObj.hasOwnProperty('value')) {
        this.value = initObj.value
      }
      else {
        this.value = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type PackedMessage
    // Serialize message field [channel_name]
    bufferOffset = _serializer.string(obj.channel_name, buffer, bufferOffset);
    // Serialize message field [field]
    bufferOffset = _serializer.string(obj.field, buffer, bufferOffset);
    // Serialize message field [value]
    bufferOffset = _serializer.int64(obj.value, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type PackedMessage
    let len;
    let data = new PackedMessage(null);
    // Deserialize message field [channel_name]
    data.channel_name = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [field]
    data.field = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [value]
    data.value = _deserializer.int64(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += object.channel_name.length;
    length += object.field.length;
    return length + 16;
  }

  static datatype() {
    // Returns string type for a message object
    return 'rdt_mqtt/PackedMessage';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '414500ef2055d204b53a9b00ad95abae';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    string channel_name
    string field
    int64 value
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new PackedMessage(null);
    if (msg.channel_name !== undefined) {
      resolved.channel_name = msg.channel_name;
    }
    else {
      resolved.channel_name = ''
    }

    if (msg.field !== undefined) {
      resolved.field = msg.field;
    }
    else {
      resolved.field = ''
    }

    if (msg.value !== undefined) {
      resolved.value = msg.value;
    }
    else {
      resolved.value = 0
    }

    return resolved;
    }
};

module.exports = PackedMessage;
