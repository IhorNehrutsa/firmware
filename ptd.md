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



* Development:

https://meshtastic.org/docs/developers

https://meshtastic.org/docs/development/device/module-api


* Компилятор protobuf:

https://github.com/protocolbuffers/protobuf/releases

https://github.com/protocolbuffers/protobuf/releases/download/v23.0-rc2/protoc-23.0-rc-2-win64.zip -> bin\protoc.exe


* Meshtastic protoBuf Schema Registry:

https://buf.build/meshtastic/protobufs/docs/main:meshtastic


# 1) Build Meshtastic Firmware

md D:\LoRa\MESHTASTIC

WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:

git clone https://github.com/IhorNehrutsa/firmware.git

cd firmware

git submodule update --init


* Переключаемся:

git checkout -b PTD remotes/origin/PTD --

* Ререгенерируем protobufі:

./bin/regen-protos.sh

* PlatformIO IDE for Microsoft's Visual Studio Code:

https://platformio.org/platformio-ide


# 2) Build Meshtastic Android Client

md D:\LoRa\MESHTASTIC

WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:

git clone https://github.com/IhorNehrutsa/Meshtastic-Android.git

cd Meshtastic-Android

git submodule update --init

* Переключаемся:

git checkout -b ptd remotes/origin/ptd --


# 3) Build Meshtastic Python Client

md D:\LoRa\MESHTASTIC

WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:

git clone https://github.com/IhorNehrutsa/python.git

cd python

git submodule update --init

* Переключаемся:

git checkout -b ptd remotes/origin/ptd --


# 4) Configuring GPIO Peripherals

https://meshtastic.org/docs/hardware/peripheral/


D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --nodes

Warning: Multiple serial ports were detected so one serial port must be specified with the '--port'.
  Ports detected:['COM21', 'COM22']

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --nodes

Connected to radio
╒═════╤═════════════════╤═══════╤═══════════╤════════════╤═════════════╤════════════╤═════════════════╤════════════════╤══════════╤═════════════════════╤═══════════════╤═══════════╕
│   N │ User            │ AKA   │ ID        │ Latitude   │ Longitude   │ Altitude   │ Channel util.   │ Tx air util.   │ SNR      │ LastHeard           │ Since         │ Battery   │
╞═════╪═════════════════╪═══════╪═══════════╪════════════╪═════════════╪════════════╪═════════════════╪════════════════╪══════════╪═════════════════════╪═══════════════╪═══════════╡
│   1 │ Meshtastic a3e8 │ a3e8  │ !a3f9a3e8 │ N/A        │ N/A         │ N/A        │ N/A             │ 0.33%          │ N/A      │ 2023-05-02 13:30:02 │ just now      │ N/A       │
├─────┼─────────────────┼───────┼───────────┼────────────┼─────────────┼────────────┼─────────────────┼────────────────┼──────────┼─────────────────────┼───────────────┼───────────┤
│   2 │ Meshtastic 2388 │ 2388  │ !a3fb2388 │ 46.3919°   │ 30.7070°    │ 61 m       │ N/A             │ 0.24%          │ 12.00 dB │ 2023-05-02 13:21:21 │ 8 minutes ago │ 100%      │
╘═════╧═════════════════╧═══════╧═══════════╧════════════╧═════════════╧════════════╧═════════════════╧════════════════╧══════════╧═════════════════════╧═══════════════╧═══════════╛

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --info

Connected to radio

Owner: Meshtastic a3e8 (a3e8)
My info: { "myNodeNum": 2751046632, "hasGps": true, "maxChannels": 8, "firmwareVersion": "2.1.5.23272da", "rebootCount": 15, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 1.78, "airUtilTx": 0.014833333 }

Nodes in mesh: {"!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"time": 1683023581}, "lastHeard": 1683023581, "deviceMetrics": {"batteryLevel": 100, "voltage": 4.143, "airUtilTx": 0.20294446}}, "!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"time": 1682599275}, "snr": 11.75, "lastHeard": 1682693289, "deviceMetrics": {"channelUtilization": 5.153333, "airUtilTx": 0.02825}}}

Preferences: { "device": { "serialEnabled": true, "nodeInfoBroadcastSecs": 10800 }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3, "rxGpio": 34, "txGpio": 12, "broadcastSmartMinimumDistance": 100, "broadcastSmartMinimumIntervalSecs": 30 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": {}, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900, "airQualityInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM22 --info

Connected to radio

Owner: Meshtastic 2388 (2388)
My info: { "myNodeNum": 2751144840, "hasGps": true, "maxChannels": 8, "firmwareVersion": "2.1.5.23272da", "rebootCount": 12, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 9.176667, "airUtilTx": 0.4159722 }

Nodes in mesh: {"!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"latitudeI": 463919041, "longitudeI": 307069995, "altitude": 61, "time": 1683022277, "latitude": 46.3919041, "longitude": 30.7069995}, "lastHeard": 1683023620, "deviceMetrics": {"batteryLevel": 100, "voltage": 4.143, "airUtilTx": 0.24380556}}, "!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"time": 1683023581}, "snr": 12.5, "lastHeard": 1683023586, "deviceMetrics": {"channelUtilization": 8.713333, "airUtilTx": 0.05791667}, "lastReceived": {"from": 2751046632, "to": 4294967295, "id": 1790507125, "rxTime": 1683023586, "rxSnr": 12.5, "hopLimit": 3, "rxRssi": -32, "fromId": "!a3f9a3e8", "toId": "^all"}, "hopLimit": 3}, "!96a13c68": {"num": 2527149160, "user": {"longName": "Meshtastic 3c68", "shortName": "3c68", "macaddr": "c8:2b:96:a1:3c:68", "hwModel": 100}, "position": {"latitudeI": 463913466, "longitudeI": 307068083, "altitude": 16, "time": 1683019846, "latitude": 46.3913466, "longitude": 30.7068083}, "snr": 10.25, "lastHeard": 1683019850}}

Preferences: { "device": { "serialEnabled": true, "nodeInfoBroadcastSecs": 10800 }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3, "rxGpio": 34, "txGpio": 12, "broadcastSmartMinimumDistance": 100, "broadcastSmartMinimumIntervalSecs": 30 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": {}, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900, "airQualityInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }

