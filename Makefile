all: u

compile:
	arduino-cli compile --fqbn esp32:esp32:esp32 .

upload:
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 .

u:
	arduino-cli compile -u -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 .

mon:
	screen -t mon /dev/ttyUSB0 115200

list:
	arduino-cli board list
