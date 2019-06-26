//
//  MTKBleManager.h
//  BleProfile
//
//  Created by ken on 14-7-6.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#include <unistd.h>

/* Charactistic UUID */
extern NSString *const kAlertLevelCharacteristicUUIDStringNew;
extern NSString *const kTxPowerLevelCharacteristicUUIDString;

extern NSString *const kEnterBackgroundNotification;
extern NSString *const kEnterForegroundNotification;
extern NSString *const kFinishLaunchNotification;

extern NSString *const UserDefaultKey_disconnecManually;
extern NSString *const UserDefaultKey_savedIdentify;
extern NSString *const UserDefaultKey_killedForcely;

/*UI Protocols */
@protocol BleDiscoveryDelegate <NSObject>

- (void) discoveryDidRefresh: (CBPeripheral *)peripheral;
- (void) discoveryStatePoweredOff;

@end

@protocol BleConnectDlegate <NSObject>

- (void) connectDidRefresh:(int)connectionState deviceName:(CBPeripheral*)peripheral;
- (void) disconnectDidRefresh: (int)connectionState devicename: (CBPeripheral *)peripheral;
- (void) retrieveDidRefresh: (NSArray *)peripherals;

@end

@protocol BleScanningStateChangeDelegate <NSObject>

- (void) scanStateChange:(int)state;

@end

@protocol BluetoothAdapterStateChangeDelegate <NSObject>

-(void)onBluetoothStateChange:(int)state;

@end

/******************** scaning state **********************/
const static int SCANNING_STATE_ON = 1;
const static int SCANNING_STATE_OFF = 0;
/**********************************************************/

/******************** conntion state **********************/
const static int CONNECT_SUCCESS = 1;
const static int CONNECT_FAILED = 2;
const static int DISCONNECT_SUCCESS = 3;
const static int DISCONNECT_FAILED = 4;

const static int CONNECTION_STATE_CONNECTED = 2;
const static int CONNECTION_STATE_CONNECTING = 1;
const static int CONNECTION_STATE_DISCONNECTING = 3;
const static int CONNECTION_STATE_DISCONNECTED = 0;
/**********************************************************/

@interface MTKBleManager: NSObject <CBPeripheralDelegate>

+ (id) sharedInstance;

@property (nonatomic) int scanningState;

- (void)registerDiscoveryDelgegate: (id<BleDiscoveryDelegate>)discoveryDelegate;
- (void)registerConnectDelgegate: (id<BleConnectDlegate>)connectDelegate;
- (void)registerScanningStateChangeDelegate:(id<BleScanningStateChangeDelegate>)scanStateChangeDelegate;
- (void)registerBluetoothStateChangeDelegate:(id<BluetoothAdapterStateChangeDelegate>)bluetoothStateChangeDelegate;

/* Action */
- (void)startScanning;
- (void)stopScanning;
- (void)forgetPeripheral;

- (int)getCurrentConnectState;

- (void)connectPeripheral: (CBPeripheral *)peripheral;
- (void)disconnectPeripheral: (CBPeripheral *)peripheral;//phase out

- (void)unRegisterDiscoveryDelgegate: (id<BleDiscoveryDelegate>)discoveryDelegate;
- (void)unRegisterConnectDelgegate: (id<BleConnectDlegate>)connectDelegate;
- (void)unRegisterScanningStateChangeDelegate:(id<BleScanningStateChangeDelegate>)scanStateChangeDelegate;
- (void)unRegisterBluetoothStateChangeDelegate:(id<BluetoothAdapterStateChangeDelegate>)bluetoothStateChangeDelegate;


-(int)getScanningState;

/* Behave properly when heading into and out of the background */
- (void)enteredBackground;
- (void)enteredForeground;

/*Access to the devices */
@property (retain, nonatomic) NSMutableArray *foundPeripherals;
@property (retain, nonatomic) NSMutableArray *connectPeripherals;
@property (retain, nonatomic) NSMutableArray *connectedService;
@property (retain, nonatomic, strong) CBPeripheral *peripheral;
@property (retain, nonatomic, strong) CBPeripheral *tempPeripheral;


@end
