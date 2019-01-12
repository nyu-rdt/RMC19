;; Auto-generated. Do not edit!


(when (boundp 'rdt-mqtt::PackedMessage)
  (if (not (find-package "RDT-MQTT"))
    (make-package "RDT-MQTT"))
  (shadow 'PackedMessage (find-package "RDT-MQTT")))
(unless (find-package "RDT-MQTT::PACKEDMESSAGE")
  (make-package "RDT-MQTT::PACKEDMESSAGE"))

(in-package "ROS")
;;//! \htmlinclude PackedMessage.msg.html


(defclass rdt-mqtt::PackedMessage
  :super ros::object
  :slots (_channel_name _field _value ))

(defmethod rdt-mqtt::PackedMessage
  (:init
   (&key
    ((:channel_name __channel_name) "")
    ((:field __field) "")
    ((:value __value) 0)
    )
   (send-super :init)
   (setq _channel_name (string __channel_name))
   (setq _field (string __field))
   (setq _value (round __value))
   self)
  (:channel_name
   (&optional __channel_name)
   (if __channel_name (setq _channel_name __channel_name)) _channel_name)
  (:field
   (&optional __field)
   (if __field (setq _field __field)) _field)
  (:value
   (&optional __value)
   (if __value (setq _value __value)) _value)
  (:serialization-length
   ()
   (+
    ;; string _channel_name
    4 (length _channel_name)
    ;; string _field
    4 (length _field)
    ;; int64 _value
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; string _channel_name
       (write-long (length _channel_name) s) (princ _channel_name s)
     ;; string _field
       (write-long (length _field) s) (princ _field s)
     ;; int64 _value
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _value (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _value) (= (length (_value . bv)) 2)) ;; bignum
              (write-long (ash (elt (_value . bv) 0) 0) s)
              (write-long (ash (elt (_value . bv) 1) -1) s))
             ((and (class _value) (= (length (_value . bv)) 1)) ;; big1
              (write-long (elt (_value . bv) 0) s)
              (write-long (if (>= _value 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _value s)(write-long (if (>= _value 0) 0 #xffffffff) s)))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; string _channel_name
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _channel_name (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; string _field
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _field (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; int64 _value
#+(or :alpha :irix6 :x86_64)
      (setf _value (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _value (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;;
   self)
  )

(setf (get rdt-mqtt::PackedMessage :md5sum-) "414500ef2055d204b53a9b00ad95abae")
(setf (get rdt-mqtt::PackedMessage :datatype-) "rdt-mqtt/PackedMessage")
(setf (get rdt-mqtt::PackedMessage :definition-)
      "string channel_name
string field
int64 value

")



(provide :rdt-mqtt/PackedMessage "414500ef2055d204b53a9b00ad95abae")


