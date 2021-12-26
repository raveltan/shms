# Smart Health Monitoring System
A smart health monitoring system with water consumption detection, humidity and temperature sensor.

## Development
To run this project directly as is, following tools will need to be installed:
- ESP32FS (Plugin for classic arduino-ide)
- Make
- arduino-cli
- arduino-language-server (OPTIONAL: autocompletion on many IDE)
- clangd 

To compile and upload the code to the device, you can run:
```bash
make compile && make upload
```

> WARNING: Makefile was made with the assumption that the project is being developed on GNU/Linux platform, please modify the Makefile in case if you are using other platform.
