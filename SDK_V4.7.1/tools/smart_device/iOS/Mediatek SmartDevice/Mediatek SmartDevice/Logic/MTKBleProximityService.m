//  betty wailiu456 1057 0458 change
//  MTKBleProximityService.m
//  BleProfile
//
//  Created by ken on 14-7-7.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import "MTKBleProximityService.h"
#import "DeviceParameter.h"
#import "MTKBleManager.h"

/* Service UUID */
NSString  *kLinkLossServiceUUIDString       = @"00001803-0000-1000-8000-00805f9b34fb";
NSString  *kImmediateAlertServiceUUIDString = @"00001802-0000-1000-8000-00805f9b34fb";
NSString  *kTxPowerServiceUUIDString        = @"00001804-0000-1000-8000-00805f9b34fb";
NSString  *kCurrentTimeUUIDString           = @"00001805-0000-1000-8000-00805f9b34fb";

/* Charactistic UUID */
NSString *kAlertLevelCharactisticUUIDString       = @"00002a06-0000-1000-8000-00805f9b34fb";
NSString *kTxPowerLevelCharactisticUUIDString     = @"00002a07-0000-1000-8000-00805f9b34fb";

@interface MTKBleProximityService() <CBPeripheralDelegate> {

@private
    CBPeripheral *servicePheripheral;
    
    CBCharacteristic *alertLevelCharactistic;
    CBCharacteristic *txPowerLevelCharactistic;
    
    int prevRssi;

    NSMutableArray *proximityAlarmList;
    MTKBleManager *bleManager;
}

@end

@implementation MTKBleProximityService

@synthesize rssi;
@synthesize txPower;
@synthesize deviceSetting;
@synthesize peripheral = servicePheripheral;
@synthesize alertStatus;
@synthesize isNotifyRemote;

/* Init */
- (id) initWithPeripheral:(CBPeripheral *)peripheral txpower: (CBCharacteristic *)txPowerChar alertLevel: (CBCharacteristic *)alertLevelChar bleManager: (MTKBleManager *) bleManagerHandler controller: (NSMutableArray *)controllerList{
    NSLog(@"[BLE][PXP]initWithPeripheral ++.\n");
    self = [super init];
    if (self) {
        bleManager = bleManagerHandler;
        servicePheripheral = peripheral;
        txPowerLevelCharactistic = txPowerChar;
        alertLevelCharactistic = alertLevelChar;
        proximityAlarmList = controllerList;
        
        [self setAlertStatus: NO_ALRT];
        [self setIsNotifyRemote: NO];
    }
    return self;
}

- (void) start
{
    NSLog(@"[BLE][PXP]start ++\n");

    [self updatePxpSetting];
    
    [self readTxPower];
}

- (void)stop {
    NSLog(@"[BLE][PXP]stop ++\n");
    [NSObject cancelPreviousPerformRequestsWithTarget: self selector:@selector(readTxPower) object: nil];
}

- (void) readTxPowerResultBack:(CBCharacteristic *)characteristic error: (NSError *)error {
    NSLog(@"[BLE][PXP]readTxPowerResultBack ++\n");
    
    if ([error code] != 0) {
        NSLog(@"[BLE][PXP]txPowerUpdatedFromCommon error %@.\n", error);
        
        if (proximityAlarmList) {
            for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
                [alarmDelegate txPowerReadBack: servicePheripheral status: kStatusFail txPower: -1];
            }
        }
        
        return;
    }
    
    int16_t txPowerValue = 0;
    [[characteristic value] getBytes: &txPowerValue length: (sizeof(txPowerValue))];
    NSLog(@"[[BLE][PXP]txPowerUpdatedFromCommon read TxPower: %d.\n", txPowerValue);
    
    txPower = txPowerValue;

    if (proximityAlarmList) {
        for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
            [alarmDelegate txPowerReadBack: servicePheripheral status: kStatusSucc txPower: txPowerValue];
        }
    }
    
    [self readRssiValue];
    
    [self performSelector: @selector(readTxPower) withObject: nil afterDelay: 5];
}

- (void) writeAlertLevelResultBack:(CBCharacteristic *)characteristic error: (NSError *)error {
    NSLog(@"[BLE][PXP]writeAlertLevelResultBack ++\n");
    
    if ([error code] != 0) {
        NSLog(@"[BLE][PXP]writeAlertLevelResultBack::error happend, %@\n", error);
        if (proximityAlarmList) {
            for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
                [alarmDelegate linkLostAlertLevelSetBack: servicePheripheral status: kStatusFail];
            }
        }
        return;
    }
    
    NSLog(@"[BLE][PXP]Write Alert Level Success.\n");
    
    if (proximityAlarmList) {
        for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
            [alarmDelegate linkLostAlertLevelSetBack: servicePheripheral status: kStatusSucc];
        }
    }
    
}

- (void) getRssiRsultBack:(CBPeripheral *)peripheral error: (NSError *)error {
    NSLog(@"[BLE][PXP]getRssiRsultBack ++\n");
    
    if (peripheral != servicePheripheral) {
        return;
    }
    
    if ([error code] != 0) {
        NSLog(@"[BLE][PXP]getRssiRsultBack error %@.\n", error);
        //read again
        //[self performSelector: @selector(readRssiValue) withObject: nil afterDelay: RSSI_DELAY_TIME];
        
        return;
    }
    
    int newRssi = [[peripheral RSSI] intValue];
     NSLog(@"[BLE][PXP]getRssiRsultBack success, newRssi = %d.\n", newRssi);
    
    if (proximityAlarmList) {
        for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
            [alarmDelegate rssiReadBack: peripheral status: kStatusSucc rssi: newRssi];
        }
    }
    
    [self setRssiAndCheckRangeAlert: newRssi];
    [self performSelector: @selector(readRssiValue) withObject: nil afterDelay: RSSI_DELAY_TIME];
}

/* Public Interfaces */
- (void)setLinkLossAlertLevel: (int)level
{
    NSLog(@"[BLE][PXP]setLinkLossAlertLevel, level = %d.\n", level);
    
    NSData *data = nil;
    int8_t value = (int8_t)level;
    
    if (!alertLevelCharactistic) {
        NSLog(@"[BLE][PXP]setLinkLossAlertLevel:: no alertLevelCharactistic.\n");
        return;
    }
    
    //testing
    Byte byte[] = {value};
    NSData *adata = [[NSData alloc] initWithBytes: byte length:1];
    
    data = [NSData dataWithBytes: &value length: sizeof(value)];
    [servicePheripheral writeValue: adata forCharacteristic: alertLevelCharactistic type: CBCharacteristicWriteWithResponse];
}

- (void)readTxPower
{
    NSLog(@"[BLE][PXP]readTxPower ++.\n");
    
    if (!txPowerLevelCharactistic) {
        NSLog(@"[BLE][PXP]readTxPower:: no txPowerLevelCharactistic.\n");
        return;
    }

    [servicePheripheral readValueForCharacteristic: txPowerLevelCharactistic];
}
 
- (void) readRssiValue
{
    NSLog(@"[BLE][PXP]readrssiValue ++\n");
    [servicePheripheral readRSSI];
    
    
    return;
}

- (int)queryDistanceValue
{
    return txPower - rssi;
}

- (void)stopRemoteAlert
{
    NSLog(@"[BLE][PXP]stopRemoteAlert ++\n");
    if (alertLevelCharactistic) {
        [self setLinkLossAlertLevel: LINK_LOST_ALERT_NO];
    }
}

- (void) setPxpParameters: (int)alertEnabler
                    range: (int)rangeAlertEnabler
                     type: (int)rangeType
               disconnect: (int)disconnectEnabler
{
    //return [MTKDevicePrarmeterRecorder setPxpParameters: nil alert: alertEnabler range:rangeAlertEnabler type: rangeType disconnect: disconnectEnabler];
}

- (DeviceParameter *) getPxpParameters
{
    return [MTKDeviceParameterRecorder getParameters: [[servicePheripheral identifier] UUIDString]];
}

- (void) updatePxpSetting
{
    NSLog(@"[BLE][PXP]updatePxpSetting ++\n");
    DeviceParameter *para = [MTKDeviceParameterRecorder getParameters:[[servicePheripheral identifier] UUIDString]];
    if (para) {
        deviceSetting.alertEnabler = [para mAlertEnabler];
        deviceSetting.rangeAlertEnabler = [para mRangeAlertEnabler];
        deviceSetting.rangAlertInOut = [para mRangeType];
        deviceSetting.rangAlertDistance = [para mRangeValue];
        deviceSetting.disconnectAlertEnabler = [para mDisconnectAlertEnabler];
    } else {
        NSLog(@"[BLE][PXP]updatePxpSetting::set para to default value\n");
        deviceSetting.alertEnabler = YES;
        deviceSetting.rangeAlertEnabler = YES;
        deviceSetting.rangAlertInOut = 1;
        deviceSetting.rangAlertDistance = 2;
        deviceSetting.disconnectAlertEnabler = YES;
    }
/*
    //test
    deviceSetting.alertEnabler = YES;
    deviceSetting.rangeAlertEnabler = YES;
    deviceSetting.rangAlertInOut = 1;
    deviceSetting.rangAlertDistance = 2;
    deviceSetting.disconnectAlertEnabler = YES;
    //test end
*/
    switch (deviceSetting.rangAlertDistance) {
        case RANGE_ALERT_NEAR:
            deviceSetting.rangAlertThreshhold = RANGE_ALERT_THRESH_NEAR;
            break;
            
        case RANGE_ALERT_MIDDLE:
            deviceSetting.rangAlertThreshhold = RANGE_ALERT_THRESH_MIDDLE;
            break;
        
        case RANGE_ALERT_FAR:
            deviceSetting.rangAlertThreshhold = RANGE_ALERT_THRESH_FAR;
            break;
            
        default:
            deviceSetting.rangAlertDistance = RANGE_ALERT_NEAR;
            deviceSetting.rangAlertThreshhold = RANGE_ALERT_THRESH_NEAR;
            break;
    }
    
    return;
}

- (void) setRssiAndCheckRangeAlert: (int)newRssi
{
    NSLog(@"[BLE][PXP]setRssiAndCheckRangeAlert::newRssi = %d.\n", newRssi);
    
    if (![self setRssiValue: newRssi]) {
        NSLog(@"[BLE][PXP]setRssiAndCheckRangeAlert:: FALSE\n");
        return;
    }
    
    if (deviceSetting.alertEnabler && deviceSetting.rangeAlertEnabler) {
        int distance = txPower - self.rssi;
        [self checkRangeAlert: distance];
        
    }
}

- (BOOL) setRssiValue: (int)newRssi
{
    NSLog(@"[BLE][PXP]setRssiValue: prevRssi: %d current rssi: %d,  new Rssi = %d.\n", prevRssi, self.rssi, newRssi);
    
    if (newRssi == 0) {
        return NO;
    } else if (self.rssi == 0) {
        self.rssi = newRssi;
        prevRssi = newRssi;
        return NO;
    } else if ( self.rssi == newRssi) {
        return NO;
    }
    
    int absRssi = abs(self.rssi);
    int absDiff = abs(self.rssi - newRssi);
    
    NSLog(@"[BLE][PXP]diff rate: %f.\n", (double)absDiff/(double)absRssi);
    
    if ((double)absDiff/(double)absRssi <= 0.25) {
        prevRssi = self.rssi;
        self.rssi = newRssi;
        
        if (proximityAlarmList) {
            for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
                [alarmDelegate distanceChangeAlarm: servicePheripheral distance: (txPower - self.rssi)];
            }
        }
    } else {
        prevRssi = self.rssi;
        self.rssi = newRssi;
        return NO;
    }
    
    return YES;
}

- (void) checkRangeAlert: (int)distance
{
    NSLog(@"[BLE][PXP]checkRangeAlert ++, disatance = %d, Threshhold = %d\n", distance, deviceSetting.rangAlertThreshhold);
    NSLog(@"[BLE][PXP]checkRangeAlert, inout = %d, alertStatus = %d\n", deviceSetting.rangAlertInOut, alertStatus);
    BOOL needUpdate = NO;
    
    switch (deviceSetting.rangAlertInOut) {
        case RANGE_ALERT_IN:
            if (distance <= deviceSetting.rangAlertThreshhold - RSSI_TOLERANCE) {
                if (alertStatus != IN_RANGE_ALERT) {
                    alertStatus = IN_RANGE_ALERT;
                    needUpdate = YES;
                }
            } else if (distance >= deviceSetting.rangAlertThreshhold + RSSI_TOLERANCE) {
                if (alertStatus != NO_ALRT) {
                    alertStatus = NO_ALRT;
                    needUpdate = YES;
                }
            }
            break;
            
        case RANGE_ALERT_OUT:
            if (distance >= deviceSetting.rangAlertThreshhold + RANGE_ALERT_OUT) {
                if (alertStatus != OUT_RANGE_ALERT) {
                    alertStatus = OUT_RANGE_ALERT;
                    needUpdate = YES;
                }
            } else if (distance <= deviceSetting.rangAlertThreshhold - RSSI_TOLERANCE) {
                if (alertStatus != NO_ALRT) {
                    alertStatus = NO;
                    needUpdate = YES;
                }
            }
            break;
            
        default:
            break;
    }
    
    if (needUpdate) {
        [self rangeAlertNofityUxAndInformRemote];
    }
}

- (void) rangeAlertNofityUxAndInformRemote
{
    NSLog(@"[BLE][PXP]rangeAlertNofityUxAndInformRemote ++\n");
//    
//    BOOL isFMPProcessing = NO;
//    if (alertLevelCharactistic) {
//        int8_t alertLevel = LINK_LOST_ALERT_NO;
//        [[alertLevelCharactistic value] getBytes: &alertLevel length: (sizeof(alertLevel))];
//        NSLog(@"[BLE][PXP]rangeAlertNotifytyUxAndInformRemote:: get alertLevel = %d\n", alertLevel);
//        if (alertLevel != LINK_LOST_ALERT_NO) {
//            isFMPProcessing = YES;
//        }
//    }
    
    if (alertStatus == NO_ALRT && isNotifyRemote) {
        NSLog(@"[BLE][PXP]rangeAlertNotifytyUxAndInformRemote::NO ALERT\n");
        [self setLinkLossAlertLevel: LINK_LOST_ALERT_NO];
        [self setIsNotifyRemote: NO];
    } else if (alertStatus != NO_ALRT) {
        NSLog(@"[BLE][PXP]rangeAlertNotifytyUxAndInformRemote::MID ALERT\n");
        [self setLinkLossAlertLevel: LINK_LOST_ALERT_MILD];
        [self setIsNotifyRemote: YES];
    }
    
}

- (void) setIsNotifyRemote: (BOOL)isNotify {
    NSLog(@"[BLE][PXP]setIsNotifyRemote::isNotify = %d\n", isNotify);
    
    if(isNotifyRemote != isNotify) {
        isNotifyRemote = isNotify;
        
        if (proximityAlarmList) {
            for (id<ProximityAlarmProtocol> alarmDelegate in proximityAlarmList) {
                [alarmDelegate alertStatusChangeAlarm:isNotifyRemote];
            }
        }
    }
}

- (BOOL) getIsNotifyRemote {
    return isNotifyRemote;
}

//testing function
- (void)alertRemote {
    NSLog(@"[BLE][PXP]ProximityService::alertRemote\n");
    [self setLinkLossAlertLevel: LINK_LOST_ALERT_HIGH];
}

@end
