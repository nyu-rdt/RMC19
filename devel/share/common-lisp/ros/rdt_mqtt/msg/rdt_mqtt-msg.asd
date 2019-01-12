
(cl:in-package :asdf)

(defsystem "rdt_mqtt-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "PackedMessage" :depends-on ("_package_PackedMessage"))
    (:file "_package_PackedMessage" :depends-on ("_package"))
  ))