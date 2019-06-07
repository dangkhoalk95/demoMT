//
//  BackgroundManager.m
//  BLEManager
//
//  Created by user on 14-7-25.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import "BackgroundManager.h"

@interface BackgroundManager()
{
@private

    NSMutableArray* stateChangeDelegateList;
    CBCentralManagerState centralManagerState;
    int scanningState;
}
@end

@implementation BackgroundManager

@synthesize mManager;

@synthesize tempPeripheral;

@synthesize mNotificationShowedUp;

@synthesize mAppCurrentState;

static BackgroundManager* instance;

+(BackgroundManager*)sharedInstance {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSLog(@"[BackgroundManager] [sharedInstance] begin to init");
        instance = [[BackgroundManager alloc] init];
        [instance initilize];
    });
    return instance;
}

-(void)initilize
{
    stateChangeDelegateList = [[NSMutableArray alloc] init];
    mManager = [MTKBleManager sharedInstance];
    [mManager registerDiscoveryDelgegate:self];
    [mManager registerBluetoothStateChangeDelegate:self];
    [mManager registerConnectDelgegate:self];
    [mManager registerScanningStateChangeDelegate:self];
    [[CachedBLEDevice defaultInstance] registerAttributeChangedListener:self];
    mAppCurrentState = APP_CURRENT_FOREGROUND;
    scanningState = SCANNING_STATE_OFF;
}

-(void)dealloc
{
    [mManager unRegisterDiscoveryDelgegate:self];
    [mManager unRegisterBluetoothStateChangeDelegate:self];
    [mManager unRegisterConnectDelgegate:self];
    [mManager unRegisterBluetoothStateChangeDelegate:self];
    [[CachedBLEDevice defaultInstance] unregisterAttributeChangedListener:self];
    [stateChangeDelegateList removeAllObjects];
    stateChangeDelegateList = nil;
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)registerStateChangeDelegate:(id<StateChangeDelegate>)delegate
{
    if (delegate == nil)
    {
        return;
    }
    if ([stateChangeDelegateList containsObject:delegate] == NO)
    {
        [stateChangeDelegateList addObject:delegate];
    }
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)unRegisterStateChangeDelegate:(id<StateChangeDelegate>)delegate
{
    if (delegate == nil)
    {
        return;
    }
    if ([stateChangeDelegateList containsObject:delegate] == YES)
    {
        [stateChangeDelegateList removeObject:delegate];
    }
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)startScan:(BOOL)timerOrNot
{
    if (centralManagerState == CBCentralManagerStatePoweredOff)
    {
        [self notifyScanStateChange:SCANNING_STATE_OFF];
        return;
    }
    [mManager startScanning];
    if(timerOrNot == YES)
    {
        self.mScanTimerStarted = YES;
        [self performSelector:@selector(timeoutToStopScan) withObject:nil afterDelay:SCAN_DEVICE_TIMEOUT];
    }
    [self notifyScanStateChange:SCANNING_STATE_ON];
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)stopScan
{
    [mManager stopScanning];
    if(self.mScanTimerStarted == YES)
    {
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(timeoutToStopScan) object:nil];
        self.mScanTimerStarted = NO;
    }
    [self notifyScanStateChange:SCANNING_STATE_OFF];
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)timeoutToStopScan
{
    self.mScanTimerStarted = NO;
    [self stopScan];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(timeoutToStopScan) object:nil];
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(BOOL)connectDevice:(CBPeripheral*)peripheral
{
    if (centralManagerState == CBCentralManagerStatePoweredOff)
    {
        NSLog(@"[BackgroundManager] [connectDevice] current adapter state is off, no need to do connect action");
        [self notifyConnectionStateChange:peripheral connectionState:CONNECTION_STATE_DISCONNECTED];
        return NO;
    }
    tempPeripheral = peripheral;
    self.mConnectTimerStarted = YES;
    self.mConnectTimeout = NO;
    [mManager connectPeripheral:peripheral];
    [self performSelector:@selector(timeoutToStopConnectAction) withObject:nil afterDelay:CONNECT_DEVICE_TIMEOUT];
    return YES;
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(BOOL)disconnectDevice:(CBPeripheral*)peripheral
{
    if (centralManagerState == CBCentralManagerStatePoweredOff)
    {
        NSLog(@"[BackgroundManager] [disconnectDevice] current state is off, no need do disconnect action");
        [self notifyConnectionStateChange:peripheral connectionState:CONNECTION_STATE_DISCONNECTED];
//        return NO;
    }
    [mManager disconnectPeripheral:peripheral];
    tempPeripheral = nil;
    if(self.mConnectTimerStarted == YES)
    {
        [self stopConnectTimer];
        self.mConnectTimerStarted = NO;
    }
    return YES;
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)timeoutToStopConnectAction
{
    self.mConnectTimeout = YES;
    self.mConnectTimerStarted = NO;
    [self disconnectDevice:tempPeripheral];
    [self stopConnectTimer];
    if (tempPeripheral != nil)
    {
        tempPeripheral = nil;
    }
    [self notifyConnectionStateChange:tempPeripheral connectionState:CONNECTION_STATE_DISCONNECTED];
}


/************************************************************************************/
/*  */
/************************************************************************************/
-(void)stopConnectTimer
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(timeoutToStopConnectAction) object:nil];
}

/************************************************************************************/
/*  */
/************************************************************************************/
- (void) discoveryDidRefresh: (CBPeripheral *)peripheral
{
    CachedBLEDevice* device = [CachedBLEDevice defaultInstance];
    if ([device.mDeviceIdentifier length] == 0)
    {
        NSLog(@"[BackgroundManager] [discoveryDidRefresh] deivce is nil");
        return;
    }
    NSLog(@"[BackgroundManager] [discoveryDidRefresh] founded identifier : %@", [[peripheral identifier] UUIDString]);
    NSLog(@"[BackgroundManager] [discoveryDidRefresh] device identifier : %@", device.mDeviceIdentifier);
    NSLog(@"[BackgroundManager] [discoveryDidRefresh] device connection state : %i", device.mConnectionState);
    if ([[[peripheral identifier]UUIDString] isEqualToString:device.mDeviceIdentifier])
    {
        if (device.mConnectionState == CONNECTION_STATE_DISCONNECTED)
        {
            [device setDevicePeripheral:peripheral];
            device.mDeviceIdentifier = [[peripheral identifier] UUIDString];
            [device persistData:2];
        }
    }
}

- (void) discoveryStatePoweredOff
{
    CachedBLEDevice* device = [CachedBLEDevice defaultInstance];
    if ([device.mDeviceIdentifier length] == 0)
    {
        return;
    }
    device.mDevicePeripheral = nil;
}

- (void) scanStateChange:(int)state
{
    [self notifyScanStateChange:state];
}

-(void)onBluetoothStateChange:(int)state
{
    NSLog(@"[BackgroundManager] [onBluetoothStateChange] state : %d", state);
    if (state != centralManagerState)
    {
        centralManagerState = state;
        for(id<StateChangeDelegate> delegate in stateChangeDelegateList)
        {
            [delegate onAdapterStateChange:state];
        }
        if (centralManagerState == CBCentralManagerStatePoweredOff)
        {
            NSLog(@"[BackgroundManager] [onBluetoothStateChange] BT is OFF, notify scan state to be OFF ");
            [self notifyScanStateChange:SCANNING_STATE_OFF];
            if (tempPeripheral != nil)
            {
                [self notifyConnectionStateChange:tempPeripheral connectionState:CONNECTION_STATE_DISCONNECTED];
            }
            else
            {
                [[CachedBLEDevice defaultInstance]updateDeviceConnectionState:nil connectionState:CONNECTION_STATE_DISCONNECTED];
            }
            if (self.mConnectTimerStarted == YES)
            {
                [self stopConnectTimer];
            }
            if (self.mScanTimerStarted == YES)
            {
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(timeoutToStopScan) object:nil];
            }
        }
    }
}

- (void) connectDidRefresh:(int)connectionState deviceName:(CBPeripheral*)peripheral
{
    if (connectionState == CONNECTION_STATE_CONNECTED)
    {
        /* do init cachedBLEDevice action */

        [self stopConnectTimer];
        
        [self notifyConnectionStateChange:peripheral connectionState:CONNECTION_STATE_CONNECTED];
    }
}

- (void) disconnectDidRefresh: (int)connectionState devicename: (CBPeripheral *)peripheral
{
    [self notifyConnectionStateChange:peripheral connectionState:CONNECTION_STATE_DISCONNECTED];
}

- (void) retrieveDidRefresh: (NSArray *)peripherals
{
    
}

-(void)onDeviceAttributeChanged:(int)which
{
    if (which == DEVICE_CONNECTION_STATE_CHANGE)
    {
        if ([CachedBLEDevice defaultInstance].mConnectionState == CONNECTION_STATE_CONNECTED)
        {
            NSLog(@"[BackgroundManager] [onDeviceAttributeChanged] device be CONNECTED, update disconnectFromUX to be NO");
            [self setDisconnectFromUx:NO];
        }
    }
}

-(void)notifyScanStateChange:(int)state
{
    if (state != SCANNING_STATE_OFF && state != SCANNING_STATE_ON)
    {
        NSLog(@"[BackgroundManager] [notifyScanStateChange] unkonw state");
        return;
    }
    if (scanningState != state)
    {
        scanningState = state;
        NSLog(@"[BackgroundManager] [notifyScanStateChange] scanningstate : %d", scanningState);
        for (id<StateChangeDelegate> delegate in stateChangeDelegateList)
        {
            [delegate onScanningStateChange:scanningState];
        }
    }
}

-(void)notifyConnectionStateChange:(CBPeripheral*)perpheral connectionState:(int)state
{
    [[CachedBLEDevice defaultInstance]updateDeviceConnectionState:perpheral connectionState:state];
    for (id<StateChangeDelegate> delegate in stateChangeDelegateList)
    {
        [delegate onConnectionStateChange:perpheral connectionState:state];
    }
}

-(int)getScanningState
{
    return scanningState;
}


- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
    NSLog(@"[BackgroundManager] [didReceLocalNotification] enter");
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(NSString*)buildDeviceName:(BOOL)needNewLine deviceName:(NSString*)str
{
    if (needNewLine == YES)
    {
        return [NSString stringWithFormat:@"[%@]\r\n", str];
    }
    else
    {
        return [NSString stringWithFormat:@"[%@] ", str];
    }
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)setDisconnectFromUx:(BOOL)disconnectFromUx
{
    NSLog(@"[BackgroundManager] [setDisconnectFromUx] disconnectFromUx : %d", disconnectFromUx);
    [[NSUserDefaults standardUserDefaults] setBool:disconnectFromUx forKey:DISCONNECT_FROM_UX];
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(BOOL)getDisconnectFromUx
{
    return [[NSUserDefaults standardUserDefaults] boolForKey:DISCONNECT_FROM_UX];
}

/************************************************************************************/
/*  */
/************************************************************************************/
-(void)setAppBackgroundOrForeground:(BOOL)background
{
    if(background == YES)
    {
        mAppCurrentState = APP_CURRENT_BACKGROUND;
    }
    else
    {
        mAppCurrentState = APP_CURRENT_FOREGROUND;
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
    }
}

@end
