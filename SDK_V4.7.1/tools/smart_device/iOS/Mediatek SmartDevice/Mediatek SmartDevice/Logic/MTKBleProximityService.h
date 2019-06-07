//
//  MTKBleProximityService.h
//  BleProfile
//
//  Created by ken on 14-7-7.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <math.h>
#import "MTKDeviceParameterRecorder.h"
#import "DeviceParameter.h"

@class MTKBleManager;

typedef enum {
    kStatusFail = 0,
    kStatusSucc = 1,
}Status;

const static int RANGE_ALERT_NEAR     = 0;
const static int RANGE_ALERT_MIDDLE   = 1;
const static int RANGE_ALERT_FAR      = 2;

//customizable values
const static int RANGE_ALERT_THRESH_NEAR = 70;
const static int RANGE_ALERT_THRESH_MIDDLE = 85;
const static int RANGE_ALERT_THRESH_FAR = 98;

//link lost alert levle
const static int LINK_LOST_ALERT_NO     = 0;
const static int LINK_LOST_ALERT_MILD   = 1;
const static int LINK_LOST_ALERT_HIGH   = 2;

const static int RANGE_ALERT_IN     = 0;
const static int RANGE_ALERT_OUT    = 1;

//Read RSSI delay time
const static int RSSI_DELAY_TIME = 1.5;

//Alert Status
const static int NO_ALRT = 0;
const static int DISCONNECTED_ALERT = 1;
const static int IN_RANGE_ALERT = 2;
const static int OUT_RANGE_ALERT = 3;

//RSSI tolerance
const static int RSSI_TOLERANCE = 2;

typedef struct {
    BOOL alertEnabler;
    BOOL rangeAlertEnabler;
    int rangAlertInOut;
    int rangAlertDistance;
    int rangAlertThreshhold;
    bool disconnectAlertEnabler;
}DeviceSetting;

/* Service UUID */
extern NSString *kLinkLossServiceUUIDString;
extern NSString *kImmediateAlertServiceUUIDString;
extern NSString *kTxPowerServiceUUIDString;
extern NSString *kCurrentTimeUUIDString;

/* Charactistic UUID */
extern NSString *kAlertLevelCharactisticUUIDString;
extern NSString *kTxPowerLevelCharactisticUUIDString;

@protocol ProximityAlarmProtocol <NSObject>

- (void)distanceChangeAlarm: (CBPeripheral *)peripheral distance: (int)distanceValue;
- (void)alertStatusChangeAlarm: (BOOL)alerted;
@optional
- (void)rssiReadBack: (CBPeripheral *)peripheral status: (int)status rssi: (int)rss;
- (void)linkLostAlertLevelSetBack: (CBPeripheral *)peripheral status: (int)status;
- (void)txPowerReadBack: (CBPeripheral *)peripheral status: (int)status txPower: (int)txPwoer;

@end

@interface MTKBleProximityService : NSObject

/* Actions */
- (id) initWithPeripheral: (CBPeripheral *)peripheral txpower: (CBCharacteristic *)txPowerChar alertLevel: (CBCharacteristic *)alertLevelChar bleManager: (MTKBleManager *) bleManagerHandler controller: (NSMutableArray *)controllerList;

- (void)start;
- (void)stop;
- (void)setLinkLossAlertLevel: (int)level;
- (void)readTxPower;
- (int)queryDistanceValue;
- (void)stopRemoteAlert;
- (void)readRssiValue;
- (void) setPxpParameters: (int)alertEnabler
                    range: (int)rangeAlertEnabler
                     type: (int)rangeType
               disconnect: (int)disconnectEnabler;

- (DeviceParameter *) getPxpParameters;

- (void) readTxPowerResultBack:(CBCharacteristic *)characteristic error: (NSError *)error;
- (void) writeAlertLevelResultBack:(CBCharacteristic *)characteristic error: (NSError *)error;
- (void) getRssiRsultBack:(CBPeripheral *)peripheral error: (NSError *)error;

//testing function
- (void)alertRemote;

//private function
- (void) updatePxpSetting;
- (BOOL) setRssiValue: (int)rssi;
- (void) setRssiAndCheckRangeAlert: (int)newRssi;
- (void) checkRangeAlert: (int)distance;
- (void) rangeAlertNofityUxAndInformRemote;
- (void) setIsNotifyRemote: (BOOL)isNotify;
- (BOOL) getIsNotifyRemote;

@property (readonly) CBPeripheral *peripheral;
@property int rssi;
@property int txPower;
@property DeviceSetting deviceSetting;
@property int alertStatus;
@property (nonatomic)BOOL isNotifyRemote;

@end
