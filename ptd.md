* Upstream`s:

https://github.com/meshtastic/firmware
https://github.com/meshtastic/protobufs
https://github.com/meshtastic/Meshtastic-Android
https://github.com/meshtastic/python

* PTD ветки:

https://github.com/IhorNehrutsa/firmware/tree/PTD
https://github.com/IhorNehrutsa/protobufs/tree/ptd
https://github.com/IhorNehrutsa/Meshtastic-Android/tree/ptd


* Protocol Buffer Basics C#:

https://protobuf.dev/getting-started/csharptutorial/

* Компилятор protobuf:

https://github.com/protocolbuffers/protobuf/releases
https://github.com/protocolbuffers/protobuf/releases/download/v23.0-rc2/protoc-23.0-rc-2-win64.zip -> bin\protoc.exe

* Meshtastic protoBuf Schema Registry:

https://buf.build/meshtastic/protobufs/docs/main:meshtastic



1) Build Meshtastic Firmware

md D:\LoRa\MESHTASTIC

Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:

git clone https://github.com/IhorNehrutsa/firmware.git

cd firmware

git submodule update --init


* Переключаемся:

git.exe checkout -b PTD remotes/origin/PTD --

* Ререгенерируем protobufі:

./bin/regen-protos.sh

* PlatformIO IDE for Microsoft's Visual Studio Code:

https://platformio.org/platformio-ide 
