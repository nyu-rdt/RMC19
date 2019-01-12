; Auto-generated. Do not edit!


(cl:in-package rdt_mqtt-msg)


;//! \htmlinclude PackedMessage.msg.html

(cl:defclass <PackedMessage> (roslisp-msg-protocol:ros-message)
  ((channel_name
    :reader channel_name
    :initarg :channel_name
    :type cl:string
    :initform "")
   (field
    :reader field
    :initarg :field
    :type cl:string
    :initform "")
   (value
    :reader value
    :initarg :value
    :type cl:integer
    :initform 0))
)

(cl:defclass PackedMessage (<PackedMessage>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <PackedMessage>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'PackedMessage)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name rdt_mqtt-msg:<PackedMessage> is deprecated: use rdt_mqtt-msg:PackedMessage instead.")))

(cl:ensure-generic-function 'channel_name-val :lambda-list '(m))
(cl:defmethod channel_name-val ((m <PackedMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader rdt_mqtt-msg:channel_name-val is deprecated.  Use rdt_mqtt-msg:channel_name instead.")
  (channel_name m))

(cl:ensure-generic-function 'field-val :lambda-list '(m))
(cl:defmethod field-val ((m <PackedMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader rdt_mqtt-msg:field-val is deprecated.  Use rdt_mqtt-msg:field instead.")
  (field m))

(cl:ensure-generic-function 'value-val :lambda-list '(m))
(cl:defmethod value-val ((m <PackedMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader rdt_mqtt-msg:value-val is deprecated.  Use rdt_mqtt-msg:value instead.")
  (value m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <PackedMessage>) ostream)
  "Serializes a message object of type '<PackedMessage>"
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'channel_name))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'channel_name))
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'field))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'field))
  (cl:let* ((signed (cl:slot-value msg 'value)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 18446744073709551616) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <PackedMessage>) istream)
  "Deserializes a message object of type '<PackedMessage>"
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'channel_name) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'channel_name) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'field) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'field) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'value) (cl:if (cl:< unsigned 9223372036854775808) unsigned (cl:- unsigned 18446744073709551616))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<PackedMessage>)))
  "Returns string type for a message object of type '<PackedMessage>"
  "rdt_mqtt/PackedMessage")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'PackedMessage)))
  "Returns string type for a message object of type 'PackedMessage"
  "rdt_mqtt/PackedMessage")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<PackedMessage>)))
  "Returns md5sum for a message object of type '<PackedMessage>"
  "414500ef2055d204b53a9b00ad95abae")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'PackedMessage)))
  "Returns md5sum for a message object of type 'PackedMessage"
  "414500ef2055d204b53a9b00ad95abae")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<PackedMessage>)))
  "Returns full string definition for message of type '<PackedMessage>"
  (cl:format cl:nil "string channel_name~%string field~%int64 value~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'PackedMessage)))
  "Returns full string definition for message of type 'PackedMessage"
  (cl:format cl:nil "string channel_name~%string field~%int64 value~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <PackedMessage>))
  (cl:+ 0
     4 (cl:length (cl:slot-value msg 'channel_name))
     4 (cl:length (cl:slot-value msg 'field))
     8
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <PackedMessage>))
  "Converts a ROS message object to a list"
  (cl:list 'PackedMessage
    (cl:cons ':channel_name (channel_name msg))
    (cl:cons ':field (field msg))
    (cl:cons ':value (value msg))
))
