# Version 2.2.1: 2017-12-22
- Fix DemoKit & Notification Crash Issue.
- Enhance EPO Download.


# Version 2.2.0: 2017-09-04
- Workaround for Samsung & Special Case.
- Add more SP into requestMTU Black List.
- Remove Useless Code.
- Remove Notification Icon Option.


# Version 2.1.0: 2017-06-23
- Add some new config parameters in wearable_config.xml.
- Add SPP ACK Feature for Device OOM Issue.
- Fix Notification Issue.
- Modify GATT WriteCommand to WriteWithRsp.


# Version 2.0.0: 2017-05-03
- Use Android Studio as IDE.
- Add some new config parameters in wearable_config.xml.
- Add more libCommand.so for other CPU Arch.
- Document Enhancement.
- Remove redundant log.


Release List:
1. MediaTek_IoT_SmartDevice_vX.X.X.apk
2. MediaTek_IoT_SmartDevice_App_Introduction.pdf
3. Readme.txt
4. wearable.jar
5. SourceCode.rar
6. MediaTek_IoT_SmartDevice_App_Programming_Guide.pdf
7. MT2511_Health_Module_Programming_Guide.pdf
8. Javadoc.rar
9. Android_BT_Related_Topic_Explanation_(SmartDevice_APK).pdf



MediaTek IoT SmartDevice is an Android application used for MediaTek IoT device (based on MT2523/MT2533 chip).
It not only applies in MediaTek IoT Bluetooth device (MT2523/MT2533), but also for Smart Health Device (MT2523 + MT2511, watch_ref_design).

The APK includes below features:
- Scan nearby BT Devices
- Switch Mode (SPP/GATT)
- Connection (Based on MTK BTNotify transport protocol)
- Notification Push
- FOTA (Firmware Over-The-Air by BTNotify transport)
- EPO Download (only applies for MTK IoT GNSS Porject)
- Find Me & BAS (only enable in GATT mode)
- MT2511 Health Feature (only enable for health device)
  - Heart Rate Measure
  - Blood Pressure Measure
  - Blood Pressure Calibration & Personal Mode
  - Heart Rate Multi-connect & Comparison



Development Guide:
1. View <MediaTek_IoT_SmartDevice_App_Introduction> pdf file in order to understand all features about MediaTek IoT SmartDevice APP.
2. Import "SmartDevice Project"(unzip from SourceCode.rar) into your Android Studio IDE, then build App.
3. Study <MediaTek_IoT_SmartDevice_App_Programming_Guide> pdf and JavaDoc (Wearable SDK API List) to understand BTNotify transport protocol, related API, how to implement your own controller and customize your APP.
4. Study <Android_BT_Related_Topic_Explanation_(SmartDevice_APK)> pdf to further understand Android BT development knowledge.
5. Study implementation detail of MT2511 Health features by using <MT2511_Health_Module_Programming_Guide> pdf, if necessary.
