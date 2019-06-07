//
//  MTKBleManager.m
//  BleProfile
//
//  Created by ken on 14-7-6.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import "MTKBleManager.h"
#import "BtNotify.h"

NSString *const SUPPORT_HEALTHKIT = false;

NSString *const kEnterBackgroundNotification = @"kEnterBackgroundNotification";
NSString *const kEnterForegroundNotification = @"kEnterForegroundNotification";
NSString *const kFinishLaunchNotification    = @"kFinishLaunchNotification";

NSString *const UserDefaultKey_disconnecManually = @"disconnectManually";
NSString *const UserDefaultKey_savedIdentify = @"connectedPeripheralIdentify";
NSString *const UserDefaultKey_killedForcely = @"killedForcely";

NSString *const kDOGPServiceUUIDString = @"000018a0-0000-1000-8000-00805f9b34fb";
NSString *const kDOGPReadCharUUIDString = @"00002aa0-0000-1000-8000-00805f9b34fb";
NSString *const kDOGPWriteCharUUIDString = @"00002aa1-0000-1000-8000-00805f9b34fb";

NSString *const VERSION = @"V_public_bluetooth_v1";

@interface MTKBleManager() <CBCentralManagerDelegate, CBPeripheralDelegate> {
    CBCentralManager *centralManager;
    CBUUID *dogpServiceUUID;
        
@private
    NSMutableArray *discoveryDelegateList;
    NSMutableArray *connectDelegateList;
    NSMutableArray *unregisterConnectDelegateList;
    NSMutableArray *scanStateChangeDelegateList;
    NSMutableArray *bluetoothStateChangeDelegateList;
    NSMutableArray *unregisterStateChangeDelegateList;
    
    CBService *dogpService;
    CBCharacteristic *dogpReadCharacteristic;
    CBCharacteristic *dogpWriteCharacteristic;
    
    CBPeripheral *servicePheripheral;
    
    BOOL mInBackground;
    NSUUID *connectedPeripheralIdentify;
    BOOL disconnectManually;

    int reconnected_retry;
    
    int mConnectState;

    BOOL dogpAlreadyInited;
}
@end

@implementation MTKBleManager

@synthesize foundPeripherals;
@synthesize connectPeripherals;
@synthesize connectedService;

@synthesize scanningState;

@synthesize peripheral = servicePheripheral;
@synthesize tempPeripheral;

static MTKBleManager *this = nil;

/* Init */
+ (id) sharedInstance
{
    if(!this) {
        this = [[MTKBleManager alloc] init];
    }
    return this;
}

- (id) init
{
    NSLog(@"init, version: %@", VERSION);
    self = [super init];
    if (self) {
        centralManager = [[CBCentralManager alloc] initWithDelegate: self queue: dispatch_get_main_queue()];

        foundPeripherals = [[NSMutableArray alloc] init];
        connectPeripherals = [[NSMutableArray alloc] init];
        connectedService = [[NSMutableArray alloc] init];
        
        discoveryDelegateList = [[NSMutableArray alloc] init];
        connectDelegateList = [[NSMutableArray alloc] init];
        unregisterConnectDelegateList = [[NSMutableArray alloc] init];
        scanStateChangeDelegateList = [[NSMutableArray alloc] init];
        bluetoothStateChangeDelegateList = [[NSMutableArray alloc] init];
        unregisterStateChangeDelegateList = [[NSMutableArray alloc] init];
        dogpServiceUUID = [CBUUID UUIDWithString: kDOGPServiceUUIDString];
        dogpService = nil;
        
        mInBackground = NO;
        connectedPeripheralIdentify = nil;
        disconnectManually = NO;

        reconnected_retry = 0;
        
        mConnectState = -1;
        
        dogpAlreadyInited = false;
    }
    return self;
}

- (void) dealloc
{
    assert(NO);
}

- (void)registerDiscoveryDelgegate: (id<BleDiscoveryDelegate>)discoveryDelegate {
    if (discoveryDelegateList) {
        [discoveryDelegateList addObject: discoveryDelegate];
    }
}

- (void)registerConnectDelgegate: (id<BleConnectDlegate>)connectDelegate {
    if (connectDelegateList) {
        [connectDelegateList addObject: connectDelegate];
    }
}

- (void)registerScanningStateChangeDelegate:(id<BleScanningStateChangeDelegate>)scanStateChangeDelegate {
    if (scanStateChangeDelegateList) {
        [scanStateChangeDelegateList addObject: scanStateChangeDelegate];
    }
}

-(void)registerBluetoothStateChangeDelegate:(id<BluetoothAdapterStateChangeDelegate>)bluetoothStateChangeDelegate
{
    if (bluetoothStateChangeDelegate == nil)
    {
        return;
    }
    if([bluetoothStateChangeDelegateList containsObject:bluetoothStateChangeDelegate] == NO)
    {
        [bluetoothStateChangeDelegateList addObject:bluetoothStateChangeDelegate];
    }
}

- (void)unRegisterDiscoveryDelgegate: (id<BleDiscoveryDelegate>)discoveryDelegate {
    if (discoveryDelegateList) {
        if ([discoveryDelegateList containsObject:discoveryDelegate] == YES) {
            [discoveryDelegateList removeObject:discoveryDelegate];
        }
    }
}

- (void)unRegisterConnectDelgegate: (id<BleConnectDlegate>)connectDelegate {
    [unregisterConnectDelegateList addObject:connectDelegate];
}

- (void)unRegisterScanningStateChangeDelegate:(id<BleScanningStateChangeDelegate>)scanStateChangeDelegate {
    if (scanStateChangeDelegateList) {
        if ([scanStateChangeDelegateList containsObject:scanStateChangeDelegate] == YES) {
            [scanStateChangeDelegateList removeObject:scanStateChangeDelegate];
        }
    }
}

-(void)unRegisterBluetoothStateChangeDelegate:(id<BluetoothAdapterStateChangeDelegate>)bluetoothStateChangeDelegate
{
    if (bluetoothStateChangeDelegate == nil)
    {
        return;
    }
    [unregisterStateChangeDelegateList addObject:bluetoothStateChangeDelegate];
}

- (void)startScanning
{
    NSLog(@"[BLE][BleManager]startScanning ++");

    if ([centralManager state] == CBCentralManagerStatePoweredOff) {
        NSLog(@"[BLE][BleManager]startScanning:: Bluetooth is not powered on, return\n");
        return;
    }

    /// before start scanning, clear all the scanned devices last time.
    for (CBPeripheral *pp in foundPeripherals) {
        pp.delegate = nil;
    }
    [[self foundPeripherals] removeAllObjects];
    
    NSDictionary *options = [NSDictionary dictionaryWithObject: [NSNumber numberWithBool: NO] forKey: CBCentralManagerScanOptionAllowDuplicatesKey];
    
    scanningState = SCANNING_STATE_ON;

    for (id<BleScanningStateChangeDelegate>scanStateChangeDelegate in scanStateChangeDelegateList) {
        [scanStateChangeDelegate scanStateChange: SCANNING_STATE_ON];
    }

    [centralManager scanForPeripheralsWithServices: nil options: options];

}

- (void)stopScanning
{
    NSLog(@"[BLE][BleManager]stopScanning ++");
    
    scanningState = SCANNING_STATE_OFF;

    for (id<BleScanningStateChangeDelegate>scanStateChangeDelegate in scanStateChangeDelegateList) {
        [scanStateChangeDelegate scanStateChange: SCANNING_STATE_OFF];
    }
    
    [centralManager stopScan];

}

-(int)getScanningState
{
    return scanningState;
}

-(CBCentralManager*)getCBCentralManager
{
    return centralManager;
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    NSLog(@"[BLE][BleManager]didDiscoverPeripheral ++, name =%@\n", [peripheral name]);
    BOOL alreadyExist = NO;
    
    NSString *savedPeripheralId = [[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify];
    if (!disconnectManually && savedPeripheralId != nil && [[[peripheral identifier] UUIDString] isEqualToString: savedPeripheralId]) {
        NSLog(@"[BLE][BleManager]didDiscoverPeripheral::found saved device\n");
        [self connectPeripheral: peripheral];
        [self stopScanning];
        return;
    }
    
    if ([[self foundPeripherals] count] == 0) {
        [[self foundPeripherals] addObject: peripheral];
        NSLog(@"[BLE][BleManager]didDiscoverPeripheral, no exist\n");
    } else {
        NSArray *tempArray = [NSArray arrayWithArray: foundPeripherals];
        for (CBPeripheral *existPeripheral in tempArray) {
            if ([[[existPeripheral identifier] UUIDString] isEqualToString: [[peripheral identifier] UUIDString]]) {
                NSLog(@"[BLE][BleManager]didDiscoverPeripheral::already exist\n");
                [[self foundPeripherals] removeObject: existPeripheral];
                [[self foundPeripherals] addObject: peripheral];
                existPeripheral.delegate = nil;
                peripheral.delegate = self;
                alreadyExist = YES;
                break;
            }
        }
        if (!alreadyExist) {
            peripheral.delegate = self;
            [[self foundPeripherals] addObject: peripheral];
        }
    }
    
    for (id<BleDiscoveryDelegate> discoveryDelegate in discoveryDelegateList) {
        [discoveryDelegate discoveryDidRefresh: peripheral];
    }
}

-(void)peripheralDidUpdateName:(CBPeripheral *)peripheral {
    for (id<BleDiscoveryDelegate> discoveryDelegate in discoveryDelegateList) {
        [discoveryDelegate discoveryDidRefresh: peripheral];
    }
}

- (NSArray *)retrievePeripherals: (NSArray *)uuids
{
    NSLog(@"[BLE][BleManager]retrievePeripherals ++.\n");
    return [centralManager retrievePeripheralsWithIdentifiers: uuids];
}

- (void)connectPeripheral: (CBPeripheral *)peripheral
{
    NSLog(@"[BLE][BleManager]connectPeripheral ++.\n");
    if (peripheral == nil) {
        NSLog(@"[BLE][BleManager]connectPeripheral:: peripheral == null, connect saved peripheral");
        
        [self connectSavedPeripheral];

        return;
    }
    
    //[self stopScanning];
    if ([peripheral state] == CBPeripheralStateDisconnected) {
        NSLog(@"[BLE][BleManager]connectPeripheral::start connect.\n");
        
        tempPeripheral = peripheral;
        [centralManager connectPeripheral: tempPeripheral options: nil];
        disconnectManually = NO;
    }
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    if (![connectPeripherals containsObject: peripheral]) {
        [connectPeripherals addObject: peripheral];
    }

    if(connectDelegateList) {
        for (id<BleConnectDlegate> connectDelegate in connectDelegateList) {
            [connectDelegate connectDidRefresh:CONNECTION_STATE_CONNECTED deviceName:peripheral];
        }
    }
    servicePheripheral = peripheral;
    [servicePheripheral setDelegate: self];
    [self discoveryPeripheral: servicePheripheral];
    
    if ([[self foundPeripherals] containsObject: peripheral]) {
        [[self foundPeripherals] removeObject: peripheral];
    }
    
    connectedPeripheralIdentify = [peripheral identifier];
    [[NSUserDefaults standardUserDefaults] setObject: [connectedPeripheralIdentify UUIDString] forKey:UserDefaultKey_savedIdentify];

    [[NSUserDefaults standardUserDefaults] setInteger: 0 forKey: UserDefaultKey_disconnecManually];
    
    mConnectState = CONNECTION_STATE_CONNECTED;
    
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"[BLE][BleManager]didFailToConnectPeripheral ++.\n");
    if ([connectPeripherals containsObject: peripheral]) {
        [connectPeripherals removeObject: peripheral];
        if (connectDelegateList) {
            for (id<BleConnectDlegate>connectDelegate in connectDelegateList) {
                [connectDelegate connectDidRefresh:CONNECTION_STATE_DISCONNECTED deviceName:peripheral];
            }
        }
        if (unregisterConnectDelegateList.count != 0) {
            for (id<BleConnectDlegate> delegate in unregisterConnectDelegateList) {
                [connectDelegateList removeObject:delegate];
            }
            [unregisterConnectDelegateList removeAllObjects];
        }
    }
}

- (void)centralManager:(CBCentralManager *)central didRetrievePeripherals:(NSArray *)peripherals {
    NSLog(@"[BLE][BleManager]didRetrievePeripherals");
    if (connectDelegateList) {
        for (id<BleConnectDlegate>connectDelegate in connectDelegateList) {
            [connectDelegate retrieveDidRefresh: peripherals];
        }
    }
}

- (void) centralManager:(CBCentralManager *)central didRetrieveConnectedPeripherals:(NSArray *)peripherals {
}

- (void)disconnectPeripheral: (CBPeripheral *)peripheral
{
    NSLog(@"[BLE][BleManager] [disconnectPeripheral] peripheral state : %ld", (long)peripheral.state);
    if ([peripheral state] != CBPeripheralStateDisconnected) {
        [centralManager cancelPeripheralConnection: peripheral];
        disconnectManually = YES;
    }
    [[NSUserDefaults standardUserDefaults] setInteger: 1 forKey: UserDefaultKey_disconnecManually];
}

//Discovery Service
- (void) discoveryPeripheral: (CBPeripheral *)peripheral
{
    NSMutableArray *uuidArray = [[NSMutableArray alloc] init];

    [uuidArray addObject: dogpServiceUUID];

    [peripheral discoverServices:uuidArray];
}

- (void) peripheral: (CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    NSLog(@"[BLE][BleManager]didDiscoverServices ++\n");
    NSArray *services;
    
    if (servicePheripheral != peripheral) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices Wrong Peripheral.\n");
        return;
    }
    
    if (error) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices, error: %@\n", error);
        return;
    }
    
    services = [peripheral services];
    if (!services || ![services count]) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices, no service found\n");
        return;
    }
    
    dogpService = nil;
    
    NSLog(@"[BLE][BleManager] service count: %lu", (unsigned long)services.count);
    
    for (CBService *service in services) {
        if([[service UUID] isEqual: dogpServiceUUID]) {
            NSLog(@"[BLE][BleManager]didDiscoverServices::found DOGP Service.");
            dogpService = service;
        }
        [peripheral discoverCharacteristics: nil forService: service];
        
    }

}

- (void) peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    NSLog(@"didDiscoverCharacteristicsForService ++");
    NSArray *characteristics = [service characteristics];
    
    if (servicePheripheral != peripheral) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices Wrong Peripheral.\n");
        return;
    }
    
    NSLog(@"didDiscoverCharacteristicsForService, service uuid = %@", [service UUID]);
    for (CBCharacteristic *characteristic in characteristics) {
        NSLog(@"[BLE][BleManager]Discovered charactistic %@", [characteristic UUID]);
        
        if([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kDOGPReadCharUUIDString]]) {
            NSLog(@"[BLE][BleManager]found DOGP read Charactistic.\n");
            dogpReadCharacteristic = characteristic;
            continue;
        } else if ([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kDOGPWriteCharUUIDString]]) {
            NSLog(@"[BLE][BleManager]found DOGP write Charactistic.\n");
            dogpWriteCharacteristic = characteristic;
            continue;
        }
    }
    
    if (dogpReadCharacteristic && dogpWriteCharacteristic && !dogpAlreadyInited) {
        dogpAlreadyInited = true;
        [self dogpInit];
    }

}

- (void)dogpInit {
    NSLog(@"[BLE][BleManager]dogpInit ++");

    // need new lib
    [[BtNotify sharedInstance] setGattParameters:servicePheripheral writeCharacteristic:dogpWriteCharacteristic readCharacteristic:dogpReadCharacteristic];
    [[BtNotify sharedInstance] updateConnectionState:CBPeripheralStateConnected];
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    NSLog(@"[BLE][BleManager]didUpdateValueForCharacteristic, char uuid = %@", [characteristic UUID]);

    if ([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kDOGPReadCharUUIDString]]) {
        // need new lib
        [[BtNotify sharedInstance] handleReadReceivedData:characteristic error:error];
    }
}

//write alert level callback
- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    if (error) {
        NSLog(@"[BLE][BleManager]didWriteValueForCharacteristic error");
    }
    
    if (peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]didWriteValueForCharacteristic::wrong peripheral, return\n");
        return;
    }

    if ([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kDOGPWriteCharUUIDString]]) {
        // need new lib
        [[BtNotify sharedInstance] handleWriteResponse:characteristic error:error];
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didModifyServices:(NSArray *)invalidatedServices {
    if (peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]didModifyServices::pheripheral wrong");
        return;
    }
    
    [self discoveryPeripheral: peripheral];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"[BLE][BleManager]didDisconnectPeripheral\n");
    
    if (servicePheripheral != peripheral) {
        NSLog(@"[BLE][BleManager]didDisconnectPeripheral::wrong peripheral\n");
        if (connectDelegateList) {
            for (id<BleConnectDlegate> connectDelegate in connectDelegateList) {
                [connectDelegate disconnectDidRefresh: CONNECTION_STATE_DISCONNECTED devicename: peripheral];
            }
        }
        if (unregisterConnectDelegateList.count != 0) {
            for (id<BleConnectDlegate> delegate in unregisterConnectDelegateList) {
                [connectDelegateList removeObject:delegate];
            }
            [unregisterConnectDelegateList removeAllObjects];
        }
        return;
    }
    
    if (error) {
         NSLog(@"[BLE][BleManager]didDisconnectPeripheral::error = %@", [error localizedDescription]);
         NSLog(@"[BLE][BleManager]didDisconnectPeripheral::error = %@", [error localizedFailureReason]);
         NSLog(@"[BLE][BleManager]didDisconnectPeripheral::code = %ld, domain = %@", (long)[error code], [error domain]);
    }
    
    if (connectDelegateList) {
        for (id<BleConnectDlegate> connectDelegate in connectDelegateList) {
            [connectDelegate disconnectDidRefresh: CONNECTION_STATE_DISCONNECTED devicename: peripheral];
        }
    }
    if (unregisterConnectDelegateList.count != 0) {
        for (id<BleConnectDlegate> delegate in unregisterConnectDelegateList) {
            [connectDelegateList removeObject:delegate];
        }
        [unregisterConnectDelegateList removeAllObjects];
    }
    
    if ([[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually] == 0
        && [[[peripheral identifier] UUIDString] isEqualToString:
            [[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify]]
        && error ) {
        NSLog(@"[BLE][BleManager]didDisconnectPeripheral unexpected");
        [self connectPeripheral:peripheral];
    }
    
    // need new lib
    [[BtNotify sharedInstance] updateConnectionState:CBPeripheralStateDisconnected];
    
    mConnectState = CONNECTION_STATE_DISCONNECTED;

    dogpReadCharacteristic = nil;
    dogpWriteCharacteristic = nil;
    dogpAlreadyInited = false;
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    NSLog(@"[BLE][BleManager]didUpdateNotificationStateForCharacteristic ++");
    if (error) {
        NSLog(@"[BLE][BleManager]didUpdateNotificationStateForCharacteristic::error = %@, failReason: %@", [error localizedDescription], [error localizedFailureReason]);
        return;
    }
    
    if ([characteristic.UUID isEqual:dogpReadCharacteristic.UUID]) {
        NSLog(@"[BLE][MTKBleManager] equal the DOGP characteristic");
        [peripheral readValueForCharacteristic:dogpReadCharacteristic];
    }
    return;
}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    NSLog(@"[BLE][BleManager]centralManagerDidUpdateState ++, state = %ld", (long)[centralManager state]);
    static CBCentralManagerState previoutState;
    
    switch ([centralManager state]) {
        case CBCentralManagerStatePoweredOff:
            mConnectState = CONNECTION_STATE_DISCONNECTED;
            for (id<BluetoothAdapterStateChangeDelegate>delegate in bluetoothStateChangeDelegateList)
            {
                [delegate onBluetoothStateChange:CBCentralManagerStatePoweredOff];
            }
            if (unregisterStateChangeDelegateList.count != 0) {
                for (id<BluetoothAdapterStateChangeDelegate> delegate in unregisterStateChangeDelegateList) {
                    [bluetoothStateChangeDelegateList removeObject:delegate];
                }
                [unregisterStateChangeDelegateList removeAllObjects];
            }
            dogpReadCharacteristic = nil;
            dogpWriteCharacteristic = nil;
            dogpAlreadyInited = false;
            
            // need new lib
            [[BtNotify sharedInstance] updateConnectionState:CBPeripheralStateDisconnected];

            break;
        
        case CBCentralManagerStatePoweredOn:
            for (id<BluetoothAdapterStateChangeDelegate> delegate in bluetoothStateChangeDelegateList)
            {
                [delegate onBluetoothStateChange:CBCentralManagerStatePoweredOn];
            }
            if ([[NSUserDefaults standardUserDefaults] boolForKey:UserDefaultKey_killedForcely]) {
                NSLog(@"[BLE][BleManager]startScanning::AP was killed before, disconnectmanually = %ld", (long)[[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually]);
                [[NSUserDefaults standardUserDefaults] setBool: NO forKey:UserDefaultKey_killedForcely];
                
                if ([[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually] == 1) {
                    return;
                }
                
                NSLog(@"[BLE][BleManager]startScanning::saved identify: %@", [[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify]);

                [self connectSavedPeripheral];
            }
            NSLog(@"[BLE][BleManager]centralManagerDidUpdateState:: disconnect = %ld", (long)[[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually]);
            if ([[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually] == 0) {
                NSLog(@"[BLE][BleManager]centralManagerDidUpdateState::power on, and connect device again");

                [self performSelector: @selector(connectSavedPeripheral) withObject: nil afterDelay: 2];
            }
            break;
            
        default:
            break;
    }
    
    previoutState = (CBCentralManagerState)[centralManager state];
    
}

- (void) connectSavedPeripheral {
    NSString *savedUUID = [[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify];
    if (savedUUID == nil) {
        NSLog(@"[BLE][BleManager]connectSavedPeripheral NON saved peripheral");
        return;
    }
    
    NSMutableArray *uuidArray = [[NSMutableArray alloc] init];
    
    NSArray *result = [centralManager retrieveConnectedPeripheralsWithServices: uuidArray];
    if ([result count] > 0) {
        reconnected_retry = 0;
        for (CBPeripheral *peripheralGetFromRetrieve in result) {
            if ([[[peripheralGetFromRetrieve identifier] UUIDString] isEqualToString:[[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify]]) {
                self.peripheral = [result objectAtIndex: 0];
                [self connectPeripheral: self.peripheral];
                [self stopScanning];
                break;
            }
        }
    } else if([result count] == 0 && reconnected_retry < 20) {
        [self performSelector: @selector(connectSavedPeripheral) withObject: nil afterDelay: 1];
        reconnected_retry ++;
        return;
    }
    
    if (reconnected_retry >= 20) {
        [self startScanning];
        reconnected_retry = 0;
    }

}

/* Behave properly when heading into and out of the background */
- (void)enteredBackground {
    NSLog(@"[BLE][BleManager]enteredBackground ++");
    mInBackground = YES;
}

- (void)enteredForeground {
    NSLog(@"[BLE][BleManager]enteredForeground ++");
    mInBackground = NO;
}

- (void)forgetPeripheral {
    NSLog(@"[BLE][BleManager]forgetPeripheral ++");
    [[NSUserDefaults standardUserDefaults] removeObjectForKey: UserDefaultKey_savedIdentify];
}

- (int)getCurrentConnectState {
    return mConnectState;
}

@end
