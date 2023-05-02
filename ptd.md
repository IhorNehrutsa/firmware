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

https://python.meshtastic.org/index.html

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

https://meshtastic.org/docs/software/python/cli

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --nodes

Warning: Multiple serial ports were detected so one serial port must be specified with the '--port'.
  Ports detected:['COM21', 'COM22']

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --nodes

Connected to radio

╒═════╤═════════════════╤═══════╤═══════════╤════════════╤═════════════╤════════════╤═════════════════╤════════════════╤══════════╤═════════════════════╤════════════════╤═══════════╕
│   N │ User            │ AKA   │ ID        │ Latitude   │ Longitude   │ Altitude   │ Channel util.   │ Tx air util.   │ SNR      │ LastHeard           │ Since          │ Battery   │
╞═════╪═════════════════╪═══════╪═══════════╪════════════╪═════════════╪════════════╪═════════════════╪════════════════╪══════════╪═════════════════════╪════════════════╪═══════════╡
│   1 │ Meshtastic a3e8 │ a3e8  │ !a3f9a3e8 │ 46.3913°   │ 30.7068°    │ 5 m        │ 2.67%           │ 0.75%          │ N/A      │ 2023-05-02 14:04:40 │ just now       │ N/A       │
├─────┼─────────────────┼───────┼───────────┼────────────┼─────────────┼────────────┼─────────────────┼────────────────┼──────────┼─────────────────────┼────────────────┼───────────┤
│   2 │ Meshtastic 3c68 │ 3c68  │ !96a13c68 │ 46.3913°   │ 30.7068°    │ 16 m       │ N/A             │ N/A            │ 12.75 dB │ 2023-05-02 14:03:45 │ 56 seconds ago │ N/A       │
├─────┼─────────────────┼───────┼───────────┼────────────┼─────────────┼────────────┼─────────────────┼────────────────┼──────────┼─────────────────────┼────────────────┼───────────┤
│   3 │ Meshtastic 2388 │ 2388  │ !a3fb2388 │ 46.3912°   │ 30.7070°    │ -61 m      │ 14.81%          │ 0.10%          │ 10.00 dB │ 2023-05-02 14:02:39 │ 2 minutes ago  │ 99%       │
╘═════╧═════════════════╧═══════╧═══════════╧════════════╧═════════════╧════════════╧═════════════════╧════════════════╧══════════╧═════════════════════╧════════════════╧═══════════╛

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM6 --info

Connected to radio

Owner: Meshtastic 3c68 (3c68)
My info: { "myNodeNum": 2527149160, "maxChannels": 8, "firmwareVersion": "2.1.10.8fe03974", "rebootCount": 473, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 0.89, "airUtilTx": 0.16008334 }

Nodes in mesh: {"!96a13c68": {"num": 2527149160, "user": {"longName": "Meshtastic 3c68", "shortName": "3c68", "macaddr": "c8:2b:96:a1:3c:68", "hwModel": 100}, "position": {"latitudeI": 463913466, "longitudeI": 307068083, "altitude": 16, "time": 1683025419, "seqNumber": 4, "latitude": 46.3913466, "longitude": 30.7068083}, "lastHeard": 1683025419}, "!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"latitudeI": 463911583, "longitudeI": 307069816, "altitude": -61, "time": 1683025358, "latitude": 46.3911583, "longitude": 30.7069816}, "snr": 6.5, "lastHeard": 1683025354}, "!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"latitudeI": 463913233, "longitudeI": 307067750, "altitude": 5, "time": 1683025359, "latitude": 46.391323299999996, "longitude": 30.706775}, "snr": 6.5}}

Preferences: { "device": { "serialEnabled": true }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "mode": "FIXED_PIN", "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": { "enabled": true, "active": true, "alertMessage": true, "alertBell": true }, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }
  SECONDARY psk=secret { "psk": "D2J+e5SqfoxjYzaKZJzIxJg9VXs7j6NBMnwRtwo/ixs=", "name": "gpio" }

Primary channel URL: https://meshtastic.org/e/#CgMSAQESCggBOAJAA0gBUAw

Complete URL (includes all channels): https://meshtastic.org/e/#CgMSAQEKKBIgGnOgWkov6dyU0iL9RSmPqgSnoDw2XHlq9SpqQwO0uQcaBGdwaW8SCggBOAJAA0gBUAw

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --info

Connected to radio

Owner: Meshtastic a3e8 (a3e8)
My info: { "myNodeNum": 2751046632, "hasGps": true, "maxChannels": 8, "firmwareVersion": "2.1.5.23272da", "rebootCount": 15, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 1.78, "airUtilTx": 0.014833333 }

Nodes in mesh: {"!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"time": 1683023581}, "lastHeard": 1683023581, "deviceMetrics": {"batteryLevel": 100, "voltage": 4.143, "airUtilTx": 0.20294446}}, "!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"time": 1682599275}, "snr": 11.75, "lastHeard": 1682693289, "deviceMetrics": {"channelUtilization": 5.153333, "airUtilTx": 0.02825}}}

Preferences: { "device": { "serialEnabled": true, "nodeInfoBroadcastSecs": 10800 }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3, "rxGpio": 34, "txGpio": 12, "broadcastSmartMinimumDistance": 100, "broadcastSmartMinimumIntervalSecs": 30 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": {}, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900, "airQualityInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }

Primary channel URL: https://meshtastic.org/e/#CgMSAQESCggBOAJAA0gBUAw

Complete URL (includes all channels): https://meshtastic.org/e/#CgMSAQEKKBIgZJ_ewVvkL3ofg6RjbFLkIBkDVPQF3WrwOMiZmeAD734aBGdwaW8SCggBOAJAA0gBUAw

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM22 --info

Connected to radio

Owner: Meshtastic 2388 (2388)
My info: { "myNodeNum": 2751144840, "hasGps": true, "maxChannels": 8, "firmwareVersion": "2.1.5.23272da", "rebootCount": 12, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 9.176667, "airUtilTx": 0.4159722 }

Nodes in mesh: {"!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"latitudeI": 463919041, "longitudeI": 307069995, "altitude": 61, "time": 1683022277, "latitude": 46.3919041, "longitude": 30.7069995}, "lastHeard": 1683023620, "deviceMetrics": {"batteryLevel": 100, "voltage": 4.143, "airUtilTx": 0.24380556}}, "!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"time": 1683023581}, "snr": 12.5, "lastHeard": 1683023586, "deviceMetrics": {"channelUtilization": 8.713333, "airUtilTx": 0.05791667}, "lastReceived": {"from": 2751046632, "to": 4294967295, "id": 1790507125, "rxTime": 1683023586, "rxSnr": 12.5, "hopLimit": 3, "rxRssi": -32, "fromId": "!a3f9a3e8", "toId": "^all"}, "hopLimit": 3}, "!96a13c68": {"num": 2527149160, "user": {"longName": "Meshtastic 3c68", "shortName": "3c68", "macaddr": "c8:2b:96:a1:3c:68", "hwModel": 100}, "position": {"latitudeI": 463913466, "longitudeI": 307068083, "altitude": 16, "time": 1683019846, "latitude": 46.3913466, "longitude": 30.7068083}, "snr": 10.25, "lastHeard": 1683019850}}

Preferences: { "device": { "serialEnabled": true, "nodeInfoBroadcastSecs": 10800 }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3, "rxGpio": 34, "txGpio": 12, "broadcastSmartMinimumDistance": 100, "broadcastSmartMinimumIntervalSecs": 30 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": {}, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900, "airQualityInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }

Primary channel URL: https://meshtastic.org/e/#CgMSAQESCggBOAJAA0gBUAw

Complete URL (includes all channels): https://meshtastic.org/e/#CgMSAQEKKBIgjMzsiQ0ja-ANomczmW0aPl5AQXKhV5f0hf1CgaX2yFQaBGdwaW8SCggBOAJAA0gBUAw

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21  --ch-add gpio

Connected to radio

Writing modified channels to device

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --seturl https://meshtastic.org/e/#CgMSAQEKKBIgGnOgWkov6dyU0iL9RSmPqgSnoDw2XHlq9SpqQwO0uQcaBGdwaW8SCggBOAJAA0gBUAw

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM22 --seturl https://meshtastic.org/e/#CgMSAQEKKBIgGnOgWkov6dyU0iL9RSmPqgSnoDw2XHlq9SpqQwO0uQcaBGdwaW8SCggBOAJAA0gBUAw