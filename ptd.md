* Upstream`s:

https://github.com/meshtastic/protobufs

https://github.com/meshtastic/firmware

https://github.com/meshtastic/Meshtastic-Android

https://github.com/meshtastic/python



* PTD ветки:

https://github.com/IhorNehrutsa/protobufs/tree/ptd

https://github.com/IhorNehrutsa/firmware/tree/PTD

https://github.com/IhorNehrutsa/Meshtastic-Android/tree/ptd



* Компилятор protobuf:

https://github.com/protocolbuffers/protobuf/releases

https://github.com/protocolbuffers/protobuf/releases/download/v23.0-rc2/protoc-23.0-rc-2-win64.zip -> bin\protoc.exe



* Protocol Buffer Basics C#:

https://protobuf.dev/getting-started/csharptutorial/



* Development:

https://meshtastic.org/docs/developers

https://meshtastic.org/docs/development/device/module-api

https://meshtastic.org/docs/hardware/peripheral/



* Meshtastic protoBuf Schema Registry:

https://buf.build/meshtastic/protobufs/docs/main:meshtastic



# 0) Регенерируем Meshtastic protobufs вручную для посмотреть и сравнить
```DOS
md D:\LoRa\MESHTASTIC
```
WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:
```BASH
git clone https://github.com/IhorNehrutsa/protobufs.git
cd protobufs
git checkout -b ptd remotes/origin/ptd --
```

* Регенерируем protobufs для C# и Python -> cmd.exe:
```DOS
cd D:\LoRa\MESHTASTIC\protobufs
protoc.bat
```

* Смотрим protobufs для C# в папке
```
d:\LoRa\MESHTASTIC\protobufs\meshtastic_csharp_out
```



# 1) Build Meshtastic Firmware
```DOS
md D:\LoRa\MESHTASTIC
```
WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

// git submodule add https://gitlab.xiph.org/xiph/speex.git
// git submodule add https://gitlab.xiph.org/xiph/opus.git

* Клонируем:
```BASH
git clone https://github.com/IhorNehrutsa/firmware.git
cd firmware
git submodule update --init --remote
```

* Переключаемся:
```BASH
git checkout -b PTD remotes/origin/PTD --
cd protobufs
```

* Регенерируем protobufs:
```BASH
./bin/regen-protos.sh
```

* Смотрим protobufs для Firmware в папке
```
d:\LoRa\MESHTASTIC\firmware\protobufs
```

* Компилируем в PlatformIO IDE for Microsoft's Visual Studio Code:

https://platformio.org/platformio-ide



# 2) Build Meshtastic Android Client
```DOS
md D:\LoRa\MESHTASTIC
```
WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:
```BASH
git clone https://github.com/IhorNehrutsa/Meshtastic-Android.git
cd Meshtastic-Android
git submodule update --init --remote
```

* Переключаемся:
```BASH
git checkout -b ptd remotes/origin/ptd --
```

* Компилируем в Android Studio, protobufs регенерируются IDE.
* Смотрим protobufs для Android в папке
```
d:\LoRa\MESHTASTIC\Meshtastic-Android\app\src\main\proto
```



# 3) Build Meshtastic Python Client

https://python.meshtastic.org/index.html

```DOS
md D:\LoRa\MESHTASTIC
```
WINDOWS context menu -> Git Bash Here: D:\LoRa\MESHTASTIC

* Клонируем:
```BASH
git clone https://github.com/IhorNehrutsa/python.git
cd python
git submodule update --init --remote
```

* Переключаемся:
```BASH
git checkout -b ptd remotes/origin/ptd --
```

* Смотрим protobufs для Python в папке
```
d:\LoRa\MESHTASTIC\python\protobufs
```
* protobufs генерируются в папку
```
d:\LoRa\MESHTASTIC\python\meshtastic
```



# 4) PTD Buttons

 * PTD Buttons Meshtastic_3c68
```
#define BUTTON_UP     14 // 0x0004000
#define BUTTON_LEFT   12 // 0x0001000
#define BUTTON_CENTER 26 // 0x4000000
#define BUTTON_RIGHT  13 // 0x0002000
#define BUTTON_DOWN   15 // 0x0008000
//                          0x400f000
```

 * PTD Buttons Meshtastic_2388 & Meshtastic_a3e8
```
#define BUTTON_UP     14 // 0x0004000
#define BUTTON_LEFT   25 // 0x2000000
#define BUTTON_CENTER  2 // 0x0000004
#define BUTTON_RIGHT  13 // 0x0002000
#define BUTTON_DOWN   15 // 0x0008000
//                          0x200E004
```



# 5) Configuring GPIO Peripherals для справки

https://meshtastic.org/docs/hardware/peripheral/

https://meshtastic.org/docs/software/python/cli

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --nodes
```
Warning: Multiple serial ports were detected so one serial port must be specified with the '--port'.
  Ports detected:['COM21', 'COM22', 'COM6']
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --nodes
```
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
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM6 --info
```
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
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --info
```
Connected to radio

Owner: Meshtastic a3e8 (a3e8)
My info: { "myNodeNum": 2751046632, "hasGps": true, "maxChannels": 8, "firmwareVersion": "2.1.5.23272da", "rebootCount": 15, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 1.78, "airUtilTx": 0.014833333 }

Nodes in mesh: {"!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"time": 1683023581}, "lastHeard": 1683023581, "deviceMetrics": {"batteryLevel": 100, "voltage": 4.143, "airUtilTx": 0.20294446}}, "!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"time": 1682599275}, "snr": 11.75, "lastHeard": 1682693289, "deviceMetrics": {"channelUtilization": 5.153333, "airUtilTx": 0.02825}}}

Preferences: { "device": { "serialEnabled": true, "nodeInfoBroadcastSecs": 10800 }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3, "rxGpio": 34, "txGpio": 12, "broadcastSmartMinimumDistance": 100, "broadcastSmartMinimumIntervalSecs": 30 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": {}, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900, "airQualityInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }

Primary channel URL: https://meshtastic.org/e/#CgMSAQESCggBOAJAA0gBUAw
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM22 --info
```
Connected to radio

Owner: Meshtastic 2388 (2388)
My info: { "myNodeNum": 2751144840, "hasGps": true, "maxChannels": 8, "firmwareVersion": "2.1.5.23272da", "rebootCount": 26, "bitrate": 77.526985, "messageTimeoutMsec": 300000, "minAppVersion": 20300, "hasWifi": true, "channelUtilization": 2.6699998, "airUtilTx": 1.4699723 }

Nodes in mesh: {"!a3fb2388": {"num": 2751144840, "user": {"longName": "Meshtastic 2388", "shortName": "2388", "macaddr": "c8:c9:a3:fb:23:88", "hwModel": "TBEAM"}, "position": {"time": 1683187036}, "lastHeard": 1683187036, "deviceMetrics": {"batteryLevel": 98, "voltage": 4.134, "airUtilTx": 1.4551389}}, "!a3f9a3e8": {"num": 2751046632, "user": {"longName": "Meshtastic a3e8", "shortName": "a3e8", "macaddr": "c8:c9:a3:f9:a3:e8", "hwModel": "TBEAM"}, "position": {"latitudeI": 463914999, "longitudeI": 307068950, "time": 1683187004, "latitude": 46.3914999, "longitude": 30.706895}, "snr": 11.75, "lastHeard": 1683187004, "deviceMetrics": {"batteryLevel": 10, "voltage": 3.605, "channelUtilization": 18.248333, "airUtilTx": 0.046944443}}, "!96a13c68": {"num": 2527149160, "user": {"longName": "Meshtastic 3c68", "shortName": "3c68", "macaddr": "c8:2b:96:a1:3c:68", "hwModel": 100}, "position": {"latitudeI": 463913466, "longitudeI": 307068083, "altitude": 16, "time": 1683186682, "latitude": 46.3913466, "longitude": 30.7068083}, "snr": 12.5, "lastHeard": 1683186698}}

Preferences: { "device": { "serialEnabled": true, "nodeInfoBroadcastSecs": 10800 }, "position": { "positionBroadcastSecs": 900, "positionBroadcastSmartEnabled": true, "gpsEnabled": true, "gpsUpdateInterval": 120, "gpsAttemptTime": 900, "positionFlags": 3, "rxGpio": 34, "txGpio": 12, "broadcastSmartMinimumDistance": 100, "broadcastSmartMinimumIntervalSecs": 30 }, "power": { "waitBluetoothSecs": 60, "meshSdsTimeoutSecs": 7200, "sdsSecs": 4294967295, "lsSecs": 300, "minWakeSecs": 10 }, "network": { "ntpServer": "0.pool.ntp.org" }, "display": { "screenOnSecs": 600 }, "lora": { "usePreset": true, "region": "EU_433", "hopLimit": 3, "txEnabled": true, "txPower": 12 }, "bluetooth": { "enabled": true, "fixedPin": 123456 } }

Module preferences: { "mqtt": { "address": "mqtt.meshtastic.org", "username": "meshdev", "password": "large4cats" }, "serial": {}, "externalNotification": {}, "rangeTest": {}, "telemetry": { "deviceUpdateInterval": 900, "environmentUpdateInterval": 900, "airQualityInterval": 900 }, "cannedMessage": {}, "audio": {}, "remoteHardware": {} }

Channels:
  PRIMARY psk=default { "psk": "AQ==" }
  SECONDARY psk=secret { "psk": "++OlisuE0Hu+UWaT4Bz4kCPDf+0SPE4tZcPnBPUNKtA=", "name": "gpio" }

Primary channel URL: https://meshtastic.org/e/#CgMSAQESCggBOAJAA0gBUAw
Complete URL (includes all channels): https://meshtastic.org/e/#CgMSAQEKKBIg--OlisuE0Hu-UWaT4Bz4kCPDf-0SPE4tZcPnBPUNKtAaBGdwaW8SCggBOAJAA0gBUAw
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21  --ch-add gpio
```
Connected to radio
Writing modified channels to device
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --seturl https://meshtastic.org/e/#CgMSAQEKKBIgGnOgWkov6dyU0iL9RSmPqgSnoDw2XHlq9SpqQwO0uQcaBGdwaW8SCggBOAJAA0gBUAw

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM22 --seturl https://meshtastic.org/e/#CgMSAQEKKBIgGnOgWkov6dyU0iL9RSmPqgSnoDw2XHlq9SpqQwO0uQcaBGdwaW8SCggBOAJAA0gBUAw

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM6 --gpio-wrb 27 1 --dest !96a13c68
```
Connected to radio
Writing GPIO mask 0x8000000 with value 0x8000000 to !96a13c68
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM6 --gpio-wrb 27 0 --dest !96a13c68
```
Connected to radio
Writing GPIO mask 0x8000000 with value 0x0 to !96a13c68
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM6 --gpio-rd 0x400f000 --dest !96a13c68
```
Connected to radio
Reading GPIO mask 0x400f000 from !96a13c68
Received RemoteHardware type=READ_GPIOS_REPLY, gpio_value=67170304 value=67170304=0x400F000
end of gpio_rd
```

D:\LoRa\MESHTASTIC\python>python meshtastic_main.py --port COM21 --gpio-watch 0x400f000 --dest !96a13c68
```
Connected to radio
Watching GPIO mask 0x400f000 from !96a13c68. Press ctrl-c to exit
Received RemoteHardware type=GPIOS_CHANGED, gpio_value=67137536 value=67137536=0x4007000
Received RemoteHardware type=GPIOS_CHANGED, gpio_value=67170304 value=67170304=0x400F000
Received RemoteHardware type=GPIOS_CHANGED, gpio_value=67166208 value=67166208=0x400E000
Received RemoteHardware type=GPIOS_CHANGED, gpio_value=67166208 value=67166208=0x400E000
Received RemoteHardware type=GPIOS_CHANGED, gpio_value=67170304 value=67170304=0x400F000
```

12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890

https://github.com/UncleRus/esp-idf-speex

https://github.com/whyengineer/esp32_snow/tree/master/example/speex

https://github.com/search?q=esp32+opus&type=repositories

regex:
\si2s_\S*\(

pio run -v >txt_.txt

https://github.com/XasWorks/esp-libopus
https://github.com/atomic14/esp32-walkie-talkie
https://blog.infrasonicaudio.com/real-time-audio-synthesis-on-esp-32
https://blog.atomic14.com/2021/04/20/esp32-i2s-dma-buf-len-buf-count.html

#define tskIDLE_PRIORITY 0
#define configMAX_PRIORITIES 10 // Highest priority level
