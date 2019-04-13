Two thing networking things
1.Getting troy's protocol into ros
  -Files are put into the ros folders
  -gcs used is troy's old gcs with updated target ip address
  -The values are read in and passed to sender node with the byte values
  -The first 2 bytes are sent as part of the heartbeat apparently and the rest is motor number followed by the value to motor

2.Testing rewriting troy's protocol with teddy's sample socket code
  -Files are put into the Testing folder
  -sends and recieves all key presses on the keyboard as a single integer and planning on parsing it later
  -should be easy to revert back to char characters since each one is a bit ((recieved >> (char wanted - 'a'))& 1)
  -have not implemented heartbeat yet
