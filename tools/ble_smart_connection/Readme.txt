Ble_Smart_Connection APK

# Version 1.1.0: 2017-07-27
- Refactor & cleanup code.

# Version 1.0.9: 2017-07-04
- Add new permission for BT Scan.
- Modify TargetSDKLevel from 23 to 21.



Release List
1. Ble_Smart_Connection.apk
2. SourceCode.rar
3. Readme.txt



mt7697lib (BLE_SmartConnection library) API:
1. bleConnect()
   // connect MT7697 Devie by BLE
   // sample code: DeviceActivity, mDevice.bleConnect();
2. bleDisconnect()
   // disconnect MT7697 by BLE
3. getBLEStatus()
   // get MT7697 BLE connection status
4. requestWifiConnect(String ssid, String password)
   // get WIFI Auth/Encrypt Mode by ssid/pwd£¬then send ssid/password/Auth/Encrypt Mode to MT7697
5. wifiChange()
   // return MT7697 current WIFI status£º1.Disconnected   2.Connected:  SSID, IP value
   // sample code: SmartConnectionFragment mDevice.requestWifiConnect(ssid, password)
   //              mDevice.wifiChange()



Test/Demo Flow:
1. "./Build.sh mt7697_hdk iot_sdk" to get BLE_SMTCN demo load.
2. Download to MT7697 device by using mt76x7-flash-tool.exe.
3. Connect UART and input "ble wifi smart" to start BLE advertising through SerialPort_Tool.
4. Launch Ble_Smart_Connection APK to scan surrounding BLE device.
5. Select "BLE_SMTCN" to connect BLE, then input your "SSID/Password" and click "Wifi_Smart_Connect" button to request MT7697 device wifi_smart_connection.
6. After MT7697 wifi connected successfully, APK "MT7697 Device Wifi Info" UI will show MT7697 wifi status/SSID/IP info. (For IP info, need wait 1~2 minutes.)



RxJava Study Link
GitHub: https://github.com/ReactiveX/RxJava
How-To-Use: https://mcxiaoke.gitbooks.io/rxdocs/content/topics/How-To-Use-RxJava.html