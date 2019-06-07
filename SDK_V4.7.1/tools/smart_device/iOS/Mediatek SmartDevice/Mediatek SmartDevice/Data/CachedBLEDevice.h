//
//  CachedBLEDevice.h
//  BLEManager
//
//  Created by ken on 14-7-11.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MTKBleManager.h"
#import "MTKBleProximityService.h"

// device value change
const static int DEVICE_CONNECTION_STATE_CHANGE = 0;                    //indicate device connection state changed
const static int DEVICE_NAME_CHANGE = 1;                                //indicate device name changed
const static int DEVICE_PERIPHERAL_CHANGE = 3;                          //indicate device Peripheral changed, used to do connect, disconnect action


const static int CONFIGURATION_STATE_ON = 1;
const static int CONFIGURATION_STATE_OFF = 0;


@protocol CachedBLEDeviceDelegate

-(void)onDeviceAttributeChanged:(int)which;

@end

@interface CachedBLEDevice : NSObject

@property NSString* mDeviceName;
@property NSString* mDeviceIdentifier;

@property int mConnectionState;

@property MTKBleManager* mManager;
@property CBPeripheral* mDevicePeripheral;

@property NSMutableArray* mAttributeListeners;


+(CachedBLEDevice*)defaultInstance;


-(void)persistData:(int)which;

-(void)registerAttributeChangedListener:(id<CachedBLEDeviceDelegate>)delegate;
-(void)unregisterAttributeChangedListener:(id<CachedBLEDeviceDelegate>)delegate;

-(void)setDevicePeripheral:(CBPeripheral*)peripheral;
-(CBPeripheral*)getDevicePeripheral;
-(void)updateDeviceConnectionState:(CBPeripheral*)peripheral connectionState:(int)state;

@end
