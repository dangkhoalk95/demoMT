//
//  CachedBLEDevice.m
//  BLEManager
//
//  Created by ken on 14-7-11.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import "CachedBLEDevice.h"
#import "MTKDeviceParameterRecorder.h"

@implementation CachedBLEDevice

@synthesize mDeviceName;
@synthesize mDeviceIdentifier;
@synthesize mConnectionState;

@synthesize mManager;


@synthesize mAttributeListeners;

static CachedBLEDevice* instance;

+(CachedBLEDevice*)defaultInstance {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSLog(@"[CachedBLEDevice] [defaultInstance] begin to init");
        instance = [[CachedBLEDevice alloc] init];
        [instance initDeviceState];
    });
    return instance;
}

-(void)initDeviceState
{
    mAttributeListeners = [[NSMutableArray alloc] init];
    mManager = [MTKBleManager sharedInstance];
}

-(void)persistData:(int)which
{
    if (which != 1 && which != 2)
    {
        NSLog(@"[CachedBLEDevice] [persistData] which is not equal 1 & 2");
        return;
    }
    
    [MTKDeviceParameterRecorder setParameters:which deviceName:mDeviceName deviceIdentifier:mDeviceIdentifier];
}

/********************************************************************************************/
/* update the remote bluetooth device peripheral which used to do connect & disconnect      */
/********************************************************************************************/
-(void)setDevicePeripheral:(CBPeripheral*)peripheral
{
    self.mDevicePeripheral = peripheral;
    [self notifyAttributeChanged:DEVICE_PERIPHERAL_CHANGE];
}

-(CBPeripheral*)getDevicePeripheral
{
    return self.mDevicePeripheral;
}

/********************************************************************************************/
/* if the device attribute has changed, call this method to notify user                     */
/********************************************************************************************/
-(void)notifyAttributeChanged:(int)which
{
    if (mAttributeListeners != nil)
    {
        if ([mAttributeListeners count] != 0)
        {
            for (id<CachedBLEDeviceDelegate> delegate in mAttributeListeners)
            {
                [delegate onDeviceAttributeChanged:which];
            }
        }
    }
}

/********************************************************************************************/
/* register the device attribute change listener. which used to notify the attribute changed*/
/********************************************************************************************/
-(void)registerAttributeChangedListener:(id<CachedBLEDeviceDelegate>)delegate
{
    if (delegate == nil)
    {
        NSLog(@"[CachedBLEDevice] [registerAttributeChangedListener] delegate is nil");
        return;
    }
    if (mAttributeListeners != nil)
    {
        if (![mAttributeListeners containsObject:delegate])
        {
            [mAttributeListeners addObject:delegate];
        }
    }
}

/********************************************************************************************/
/* unregister the device attribute change listener.                                         */
/********************************************************************************************/
-(void)unregisterAttributeChangedListener:(id<CachedBLEDeviceDelegate>)delegate
{
    if (delegate == nil)
    {
        NSLog(@"[CachedBLEDevice] [unregisterAttributeChangedListener] delegate is nil");
        return;
    }
    if (mAttributeListeners != nil)
    {
        if ([mAttributeListeners containsObject:delegate])
        {
            [mAttributeListeners removeObject:delegate];
        }
    }
}


-(void)updateDeviceConfiguration:(int)which changedValue:(int)value
{
    NSLog(@"[CachedBLEDevice] [updateDeviceConfiguration] which : %d, value : %d", which, value);
    BOOL changed = NO;

    if (changed == YES)
    {
        [self notifyAttributeChanged:which];
        [self persistData:2];
    }
}

-(void)updateDeviceConnectionState:(CBPeripheral*)peripheral connectionState:(int)state;
{
    if (peripheral != nil && ([[[peripheral identifier] UUIDString] isEqualToString:mDeviceIdentifier] == NO))
    {
        NSLog(@"[CachedBLEDevice] [updateDeviceConnectionState] peripheral identifier not match");
        return;
    }
    NSLog(@"[CachedBLEDevice] [updateDeviceConnectionState] mConnectionState : %d, state : %d", mConnectionState, state);
    if (mConnectionState != state)
    {
        mConnectionState = state;
        if (mConnectionState == CONNECTION_STATE_CONNECTED)
        {
            NSLog(@"[CachedBLEDevice] [updateDeviceConnectionState] update device connection state to be CONNECTION_STATE_CONNECTED");
            [self setDevicePeripheral:peripheral];
        }
        [self notifyAttributeChanged:DEVICE_CONNECTION_STATE_CHANGE];
    }
}

-(BOOL)changeStatus:(int)status
{
    if (status == CONFIGURATION_STATE_ON)
    {
        return YES;
    }
    else if (status == CONFIGURATION_STATE_OFF)
    {
        return NO;
    }
    return NO;
}

@end
