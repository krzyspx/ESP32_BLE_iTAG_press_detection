# ESP32_BLE_iTAG_press_detection
sketch allows: 
automatically search for active iTAG (only one at a time)

connect to iTAG with the "Press Key" service "ffe0"

read notifications about pressing key

decode the iTAG OFF state and set to a standby mode for waiting for iTAG ON

more

http://100-x-arduino.blogspot.com/

sketch base on Neil Kolban library and example

https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLETests/Arduino/BLE_client/BLE_client.ino
