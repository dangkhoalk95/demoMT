//
//  BackgroundManager.h
//  BLEManager
//
//  Created by user on 14-7-25.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MTKBleManager.h"
#import "CachedBLEDevice.h"

const static int SCAN_DEVICE_TIMEOUT = 300;
const static int CONNECT_DEVICE_TIMEOUT = 30;
const static int NOTIFICATION_TIMEOUT = 30;

const static int ALERT_TYPE_NONE = -10;
const static int ALERT_TYPE_DISCONNECTED = 10;
const static int ALERT_TYPE_OUT_OF_RANGE = 11;
const static int ALERT_TYPE_IN_RANGE = 12;

const static int APP_CURRENT_FOREGROUND = 20;
const static int APP_CURRENT_BACKGROUND = 21;

static NSString* DIALOG_TITLE_STRING = @"MediaTek SmartDevice Alert";
static NSString* DIALOG_DISCONNECTED_MESSAGE = @"disconnected";
static NSString* DIALOG_OUT_OF_RANGE_MESSAGE = @"out of range";
static NSString* DIALOG_IN_RANGE_MESSAGE = @"in range";
static NSString* DIALOG_DISMISS_BUTTON_TEXT = @"Dismiss";
//static NSString* DIALOG_CHECK_BUTTON_TEXT = @"Check";

static NSString* DISCONNECT_FROM_UX = @"disconnect_from_ux";

const static int ATTRIBUTE_CONNECTION_STATE_CHANGE = 30;
const static int ATTRIBUTE_SCAN_STATE_CHANGE = 31;

@protocol StateChangeDelegate <NSObject>

-(void)onAdapterStateChange:(int)state;
-(void)onConnectionStateChange:(CBPeripheral*)peripheral connectionState:(int)state;
-(void)onScanningStateChange:(int)state;

@end


@interface BackgroundManager : NSObject <BleDiscoveryDelegate, BleScanningStateChangeDelegate,
    BluetoothAdapterStateChangeDelegate, CachedBLEDeviceDelegate, BleConnectDlegate>

@property (nonatomic, strong) MTKBleManager* mManager;

@property (nonatomic, strong) CBPeripheral* tempPeripheral;

@property (nonatomic) BOOL mNotificationShowedUp;

@property (nonatomic) BOOL mScanTimerStarted;
@property (nonatomic) BOOL mConnectTimerStarted;
@property (nonatomic) BOOL mConnectTimeout;

@property (nonatomic) int mAppCurrentState;

+(BackgroundManager*)sharedInstance;

-(void)registerStateChangeDelegate:(id<StateChangeDelegate>)delegate;
-(void)unRegisterStateChangeDelegate:(id<StateChangeDelegate>)delegate;

-(void)startScan:(BOOL)timerOrNot;
-(void)stopScan;

-(BOOL)connectDevice:(CBPeripheral*)peripheral;
-(BOOL)disconnectDevice:(CBPeripheral*)peripheral;
-(void)stopConnectTimer;

-(void)setDisconnectFromUx:(BOOL)disconnectFromUx;

-(void)setAppBackgroundOrForeground:(BOOL)background;

-(int)getScanningState;

@end
