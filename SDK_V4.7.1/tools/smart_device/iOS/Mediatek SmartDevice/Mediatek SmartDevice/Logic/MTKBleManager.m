//
//  MTKBleManager.m
//  BleProfile
//
//  Created by ken on 14-7-6.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import "MTKBleManager.h"
#import "VolumeController.h"

NSString *kEnterBackgroundNotification = @"kEnterBackgroundNotification";
NSString *kEnterForegroundNotification = @"kEnterForegroundNotification";

NSString *UserDefaultKey_disconnecManually = @"disconnectManually";
NSString *UserDefaultKey_savedIdentify = @"connectedPeripheralIdentify";
NSString *UserDefaultKey_killedForcely = @"killedForcely";

@interface MTKBleManager() <CBCentralManagerDelegate, CBPeripheralDelegate> {
    CBCentralManager *centralManager;
    
    CBUUID *linkLossServiceUUID;
    CBUUID *immediateAlertServiceUUID;
    CBUUID *txPowerServiceUUID;
    CBUUID *currentTimeServiceUUID;
        
@private
    CBPeripheral *currentPeripheral;
    NSMutableArray *discoveryDelegateList;
    NSMutableArray *connectDelegateList;
    NSMutableArray *proximityDelegateList;
    NSMutableArray *scanStateChangeDelegateList;
    NSMutableArray *bluetoothStateChangeDelegateList;
    
    CBService *linkLossService;
    CBService *immediateAlertService;
    CBService *txPowerService;
    CBService *serviceLocal;
    
    CBCharacteristic *alertLevelCharactistic;
    CBCharacteristic *txPowerLevelCharactistic;
    CBCharacteristic *immediateAlertCharactistic;
    CBCharacteristic *volumeControlCharactistic;
    
    CBPeripheral *servicePheripheral;
    
    MTKBleProximityService *proximityService;
    FmpCentral *fmpCentral;
    AlertService *alertService;
    
    BOOL mInBackground;
    NSUUID *connectedPeripheralIdentify;
    BOOL disconnectManually;

    VolumeController *volumerController;
}
@end

@implementation MTKBleManager

@synthesize foundPeripherals;
@synthesize connectPeripherals;
@synthesize connectedService;

@synthesize scanningState;

@synthesize peripheral = servicePheripheral;
@synthesize tempPeripheral;

 static Boolean isFmpReady=NO;

/* Init */
+ (id) sharedInstance
{
    static MTKBleManager *this = nil;
    if(!this) {
        this = [[MTKBleManager alloc] init];
    }
    return this;
}

- (id) init
{
    self = [super init];
    if (self) {
        centralManager = [[CBCentralManager alloc] initWithDelegate: self queue: dispatch_get_main_queue()];

        foundPeripherals = [[NSMutableArray alloc] init];
        connectPeripherals = [[NSMutableArray alloc] init];
        connectedService = [[NSMutableArray alloc] init];
        
        linkLossServiceUUID         = [CBUUID UUIDWithString: kLinkLossServiceUUIDString];
        immediateAlertServiceUUID   = [CBUUID UUIDWithString: kImmediateAlertServiceUUIDString];
        txPowerServiceUUID          = [CBUUID UUIDWithString: kTxPowerServiceUUIDString];
        currentTimeServiceUUID      = [CBUUID UUIDWithString: kCurrentTimeUUIDString];

        currentPeripheral = [[CBPeripheral alloc] init];
        
        discoveryDelegateList = [[NSMutableArray alloc] init];
        connectDelegateList = [[NSMutableArray alloc] init];
        proximityDelegateList = [[NSMutableArray alloc] init];
        scanStateChangeDelegateList = [[NSMutableArray alloc] init];
        bluetoothStateChangeDelegateList = [[NSMutableArray alloc] init];
        
        proximityService = nil;
        fmpCentral=nil;
        alertService=nil;
        
        alertLevelCharactistic = nil;
        txPowerLevelCharactistic = nil;
        immediateAlertCharactistic = nil;
        volumeControlCharactistic =  nil;
        
        //alertService=[[AlertService alloc]init];
        
        mInBackground = NO;
        connectedPeripheralIdentify = nil;
        disconnectManually = NO;
        
        volumerController = [VolumeController volumeContorllerInstance];
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

- (void)registerProximityDelgegate: (id<ProximityAlarmProtocol>)proximityDelegate {
    if (proximityDelegateList) {
        [proximityDelegateList addObject: proximityDelegate];
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
    NSLog(@"[BLE][BleManager]unRegisterDiscoveryDelgegate ++\n");
    if (discoveryDelegateList) {
        for (int index = 0; index < [discoveryDelegateList count]; index ++) {
            if ([discoveryDelegateList objectAtIndex: index] == discoveryDelegate) {
                NSLog(@"[BLE][BleManager]unRegisterDiscoveryDelgegate::found, delete\n");
                [discoveryDelegateList removeObjectAtIndex: index];
                break;
            }
        }
    }
}

- (void)unRegisterConnectDelgegate: (id<BleConnectDlegate>)connectDelegate {
    NSLog(@"[BLE][BleManager]unRegisterConnectDelgegate ++\n");
    if (connectDelegateList) {
        for (int index = 0; index < [connectDelegateList count]; index ++) {
            if ([connectDelegateList objectAtIndex: index] == connectDelegate) {
                NSLog(@"[BLE][BleManager]unRegisterConnectDelgegate::found, delete\n");
                [connectDelegateList removeObjectAtIndex: index];
                break;
            }
        }
    }
}

- (void)unRegisterProximityDelgegate: (id<ProximityAlarmProtocol>)proximityDelegate {
    NSLog(@"[BLE][BleManager]unRegisterProximityDelgegate ++\n");
    if (proximityDelegateList) {
        for (int index = 0; index < [proximityDelegateList count]; index ++) {
            if ([proximityDelegateList objectAtIndex: index] == proximityDelegate) {
                NSLog(@"[BLE][BleManager]unRegisterProximityDelgegate::found, delete\n");
                [proximityDelegateList removeObjectAtIndex: index];
                break;
            }
        }
    }
}

- (void)unRegisterScanningStateChangeDelegate:(id<BleScanningStateChangeDelegate>)scanStateChangeDelegate {
    NSLog(@"[BLE][BleManager]unRegisterScanningStateChangeDelegate ++\n");
    if (scanStateChangeDelegateList) {
        for (int index = 0; index < [scanStateChangeDelegateList count]; index ++) {
            if ([scanStateChangeDelegateList objectAtIndex: index] == scanStateChangeDelegate) {
                NSLog(@"[BLE][BleManager]unRegisterScanningStateChangeDelegate::found, delete\n");
                [scanStateChangeDelegateList removeObjectAtIndex: index];
                break;
            }
        }
    }
}

-(void)unRegisterBluetoothStateChangeDelegate:(id<BluetoothAdapterStateChangeDelegate>)bluetoothStateChangeDelegate
{
    if (bluetoothStateChangeDelegate == nil)
    {
        return;
    }
    if ([bluetoothStateChangeDelegateList containsObject:bluetoothStateChangeDelegate] == YES)
    {
        [bluetoothStateChangeDelegateList removeObject:bluetoothStateChangeDelegate];
    }
}

- (void)startScanning
{
    NSLog(@"[BLE][BleManager]startScanning ++ .\n");

    if ([centralManager state] == CBCentralManagerStatePoweredOff) {
        NSLog(@"[BLE][BleManager]startScanning:: Bluetooth is not powered on, return\n");
        return;
    }

    /// before start scanning, clear all the scanned devices last time.
    [[self foundPeripherals] removeAllObjects];
    
    NSArray *serviceArray = [NSArray arrayWithObjects:linkLossServiceUUID,
                             immediateAlertServiceUUID,
                             txPowerServiceUUID,
                             //currentTimeServiceUUID,
                             nil];
    NSDictionary *options = [NSDictionary dictionaryWithObject: [NSNumber numberWithBool: NO] forKey: CBCentralManagerScanOptionAllowDuplicatesKey];
    
    scanningState = SCANNING_STATE_ON;

    for (id<BleScanningStateChangeDelegate>scanStateChangeDelegate in scanStateChangeDelegateList) {
        [scanStateChangeDelegate scanStateChange: SCANNING_STATE_ON];
    }
    
    //[centralManager scanForPeripheralsWithServices: serviceArray options: options];
    [centralManager scanForPeripheralsWithServices: nil options: options];

}

- (void)stopScanning
{
    NSLog(@"[BLE][BleManager]stopScanning --");
    
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
    
    NSString *savedPeripheralId = [MTKDeviceParameterRecorder getSavedDeviceIdentifier];
    NSLog(@"[BLE][BleManager]didDiscoverPeripheral::savedPeripheralId = %@, manudisconnect = %d", savedPeripheralId, disconnectManually);
    NSLog(@"[BLE][BleManager]didDiscoverPeripheral::new found id =%@\n", [[peripheral identifier] UUIDString]);
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
                alreadyExist = YES;
                break;
            }
        }
        if (!alreadyExist) {
            [[self foundPeripherals] addObject: peripheral];
        }
    }
    
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
    NSLog(@"[BLE][BleManager]didConnectPeripheral ++.\n");
    [alertService setCurrentState:YES];
    if (![connectPeripherals containsObject: peripheral]) {
        [connectPeripherals addObject: peripheral];
        

    }

    NSLog(@"[BLE][BleManager]didConnectPeripheral first, read rris immediately.\n");
    [peripheral readRSSI];

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
    
    if (volumerController) {
        [volumerController setIsConnected: YES];
    }
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
     NSLog(@"[BLE][BleManager]didRetrieveConnectedPeripherals");
}

- (void)disconnectPeripheral: (CBPeripheral *)peripheral
{
    NSLog(@"[BLE][BleManager] [disconnectPeripheral] peripheral state : %d", peripheral.state);
    if ([peripheral state] != CBPeripheralStateDisconnected) {
        [centralManager cancelPeripheralConnection: peripheral];
        disconnectManually = YES;

        [[NSUserDefaults standardUserDefaults] setInteger: 1 forKey: UserDefaultKey_disconnecManually];
    }
}

//Discovery Service
- (void) discoveryPeripheral: (CBPeripheral *)peripheral
{
    NSLog(@"[BLE][BleManager]discoveryPeripheral ++\n");
    
    NSArray *serviceArray = [NSArray arrayWithObjects:linkLossServiceUUID,
                             immediateAlertServiceUUID,
                             txPowerServiceUUID,
                             nil];
    [peripheral discoverServices: serviceArray];
}

- (void) peripheral: (CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    NSLog(@"[BLE][BleManager]didDiscoverServices ++\n");
    NSArray *services;
    
    if (servicePheripheral != peripheral) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices Wrong Peripheral.\n");
        return;
    }
    
    if (error != nil) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices, error: %@\n", error);
        return;
    }
    
    services = [peripheral services];
    if (!services || ![services count]) {
        NSLog(@"[BLE][BleManager]didDiscoveryServices, no service found\n");
        return;
    }
    
    for (CBService *service in services) {
//        NSLog(@"[BLE][BleManager]didDiscoveryServices:: found service uuid = %@\n", [[service UUID] UUIDString]);
    }
    
    linkLossService = nil;
    immediateAlertService = nil;
    txPowerService = nil;
    
    for (CBService *service in services) {
        if ([[service UUID] isEqual: linkLossServiceUUID]) {
            NSLog(@"[BLE][BleManager]didDiscoverServices::found LinkLostService.\n");
            linkLossService = service;
            continue;
        } else if ([[service UUID] isEqual: immediateAlertServiceUUID]) {
            NSLog(@"[BLE][BleManager]didDiscoverServices::found ImmediateAlertService.\n");
            immediateAlertService = service;
            continue;
        } else if ([[service UUID] isEqual: txPowerServiceUUID]) {
            NSLog(@"[BLE][BleManager]didDiscoverServices::found TxPwoerService.\n");
            txPowerService = service;
            continue;
        }
    }
    
    if (linkLossService) {
        NSArray *uuidLinkLoss = [NSArray arrayWithObject: [CBUUID UUIDWithString: kAlertLevelCharactisticUUIDString]];
        [peripheral discoverCharacteristics: uuidLinkLoss forService:linkLossService];
    }
    
    if (txPowerService) {
        NSArray *uuidTxPower = [NSArray arrayWithObject: [CBUUID UUIDWithString: kTxPowerLevelCharactisticUUIDString]];
        [peripheral discoverCharacteristics: uuidTxPower forService: txPowerService];
    }
    if (immediateAlertService){
        NSLog(@"[BLE][BleManager]found immediateAlertService and discover characteristic.\n");
        NSArray *uuidAlertLevel = [NSArray arrayWithObject: [CBUUID UUIDWithString: kAlertLevelCharactisticUUIDString]];
        [peripheral discoverCharacteristics: uuidAlertLevel forService: immediateAlertService];
        
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
    
    if (service != linkLossService && service != txPowerService && service != immediateAlertService) {
        NSLog(@"[BLE][BleManager]Wrong Service.\n");
        return;
    }
    
    NSLog(@"didDiscoverCharacteristicsForService, cout = %d", [characteristics count]);
    NSLog(@"didDiscoverCharacteristicsForService, service uuid = %@", [[service UUID] UUIDString]);
    for (CBCharacteristic *characteristic in characteristics) {
//        NSLog(@"[BLE][BleManager]Discovered charactistic %@", [[characteristic UUID] UUIDString]);
        
        if ([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kAlertLevelCharactisticUUIDString]]
            && [[service UUID] isEqual: [CBUUID UUIDWithString: kLinkLossServiceUUIDString]]) {
            NSLog(@"[BLE][BleManager]found AlertLevel Charactistic.\n");
            alertLevelCharactistic = characteristic;
            continue;
        } else if([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kTxPowerLevelCharactisticUUIDString]]
            && [[service UUID] isEqual: [CBUUID UUIDWithString: kTxPowerServiceUUIDString]]) {
            
            NSLog(@"[BLE][BleManager]found TxPower Charactistic.\n");
            txPowerLevelCharactistic = characteristic;
            continue;
        } else if([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kAlertLevelCharactisticUUIDString]]
                  && [[service UUID] isEqual: [CBUUID UUIDWithString: kImmediateAlertServiceUUIDString]]) {
            NSLog(@"[BLE][BleManager]found immediateAlert Charactistic.\n");
            immediateAlertCharactistic = characteristic;
            continue;
        } else if([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kVolumeControlCharacteristicUUID]]
                  && [[service UUID] isEqual: [CBUUID UUIDWithString: kVolumeControlServiceUUID]]) {
            NSLog(@"[BLE][BleManager]found volumeControl Charactistic.\n");
            volumeControlCharactistic = characteristic;
            continue;
        }
    }

    if (alertLevelCharactistic && txPowerLevelCharactistic) {
        proximityService = [[MTKBleProximityService alloc] initWithPeripheral:servicePheripheral txpower:txPowerLevelCharactistic alertLevel: alertLevelCharactistic bleManager: self controller: proximityDelegateList];
        if (proximityService) {
            [proximityService start];
        }
    }
    if(immediateAlertCharactistic && ([[immediateAlertCharactistic.service UUID] isEqual: immediateAlertServiceUUID ])){
        
        //add fmpCentral initialization
        fmpCentral=[[FmpCentral alloc]initWithPeripheral:peripheral alertLevel: immediateAlertCharactistic];
        if(fmpCentral){
          [fmpCentral setIsReadyFind:YES];
            isFmpReady=YES;
        }else{
            isFmpReady=NO;
        }
    }

}

-(void)findTarget:(int) level
{
    if(isFmpReady){
        [fmpCentral findTarget:level];
    }else{
        NSLog(@"Fmp is not ready to find");
    }
        
}

- (void)peripheralDidUpdateRSSI:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"[BLE][BleManager]peripheralDidUpdateRSSI ++\n");
    
    if (peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]Wrong Peripheral.\n");
        return;
    }

    if (proximityService) {
        [proximityService getRssiRsultBack: peripheral error: error];
    } else {
        NSLog(@"[BLE][BleManager]peripheralDidUpdateRSSI:: proximityService = nil");

        if (proximityDelegateList) {
            for (id<ProximityAlarmProtocol> alarmDelegate in proximityDelegateList) {
                [alarmDelegate distanceChangeAlarm: servicePheripheral distance: abs([[peripheral RSSI] intValue])];
            }
        }
    }
}

//txpower read callback
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
//    NSLog(@"[BLE][BleManager]didUpdateValueForCharacteristic, char uuid = %@\n", [[characteristic UUID] UUIDString]);

    if ([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kTxPowerLevelCharactisticUUIDString]]) {
        NSLog(@"[BLE][BleManager]TxPower Charactistic read.\n");

        if (proximityService) {
            [proximityService readTxPowerResultBack: characteristic error: error];
        }
    }

}

//write alert level callback
- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
//    NSLog(@"[BLE][BleManager]didUpdateValueForCharacteristic, char uuid = %@\n", [[characteristic UUID] UUIDString]);
    
    if (peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]didUpdateValueForCharacteristic::wrong peripheral, return\n");
        return;
    }
    
    if ([[characteristic UUID] isEqual: [CBUUID UUIDWithString: kAlertLevelCharactisticUUIDString]]) {
        if (proximityService) {
            [proximityService writeAlertLevelResultBack: characteristic error: error];
        }
    }
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
        return;
    }
    
    if (proximityService) {
        [proximityService stop];
    }
    
    if (connectDelegateList) {
        for (id<BleConnectDlegate> connectDelegate in connectDelegateList) {
            [connectDelegate disconnectDidRefresh: CONNECTION_STATE_DISCONNECTED devicename: peripheral];
        }
    }
    
    if (proximityService) {
        [proximityService stop];
        proximityService = nil;
    }
    [alertService setCurrentState:NO];
    
    if ([[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually] == 0) {
        NSLog(@"[BLE][BleManager] didDisconnectPeripheral by outof range, scanning again");
        [self connectPeripheral:peripheral];
    }
    
    if (volumerController) {
        [volumerController setIsConnected: NO];
    }

}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    NSLog(@"[BLE][BleManager]centralManagerDidUpdateState ++, state = %d\n", [centralManager state]);
    static CBCentralManagerState previoutState = -1;
    
    switch ([centralManager state]) {
        case CBCentralManagerStatePoweredOff:
            for (id<BluetoothAdapterStateChangeDelegate>delegate in bluetoothStateChangeDelegateList)
            {
                [delegate onBluetoothStateChange:CBCentralManagerStatePoweredOff];
            }
            break;
        
        case CBCentralManagerStatePoweredOn:
            for (id<BluetoothAdapterStateChangeDelegate> delegate in bluetoothStateChangeDelegateList)
            {
                [delegate onBluetoothStateChange:CBCentralManagerStatePoweredOn];
            }
            if ([[NSUserDefaults standardUserDefaults] boolForKey:UserDefaultKey_killedForcely]) {
                NSLog(@"[BLE][BleManager]startScanning::AP was killed before, disconnectmanually = %d", [[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually]);
                [[NSUserDefaults standardUserDefaults] setBool: NO forKey:UserDefaultKey_killedForcely];
                
                if ([[NSUserDefaults standardUserDefaults] integerForKey: UserDefaultKey_disconnecManually] == 1) {
                    return;
                }
                
                NSLog(@"[BLE][BleManager]startScanning::saved identify: %@", [[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify]);

                [self connectSavedPeripheral];
            }
            break;
            
        default:
            break;
    }
    
    previoutState = [centralManager state];
}

- (void)updatePxpSetting:(CBPeripheral *)peripheral {
    NSLog(@"[BLE][BleManager]updatePxpSetting\n");
    if (peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]updatePxpSetting::wrong peripheral\n");
        return;
    }
    if (proximityService) {
        [proximityService updatePxpSetting];
    }
}

- (int)queryDistance: (CBPeripheral *) peripheral {
    NSLog(@"[BLE][BleManager]queryDistance\n");
    if(peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]queryDistance::wrong peripheral");
        return -1;
    }
    
    if (proximityService) {
        return [proximityService queryDistanceValue];
    }
    
    return -1;
}

- (BOOL)getIsNotifyRemote: (CBPeripheral *)peripheral {
    NSLog(@"[BLE][BleManager]getIsNotifyRemote\n");
    if(peripheral != servicePheripheral) {
        NSLog(@"[BLE][BleManager]getIsNotifyRemote::wrong peripheral");
        return NO;
    }
    
    if (proximityService) {
        return [proximityService getIsNotifyRemote];
    }
    
    return  NO;
}

- (void) connectSavedPeripheral {
    NSLog(@"[BLE][BleManager]connectSavedPeripheral ++");
    
    NSArray *serviceArray = [NSArray arrayWithObjects:linkLossServiceUUID,
                             immediateAlertServiceUUID,
                             txPowerServiceUUID,
                             //currentTimeServiceUUID,
                             nil];
    NSArray *result = [centralManager retrieveConnectedPeripheralsWithServices: serviceArray];
    NSLog(@"result1 = %d", [result count]);
    if ([result count] > 0) {
        for (CBPeripheral *peripheralGetFromRetrieve in result) {
            if ([[[peripheralGetFromRetrieve identifier] UUIDString] isEqualToString:[[NSUserDefaults standardUserDefaults] valueForKey:UserDefaultKey_savedIdentify]]) {
                self.peripheral = [result objectAtIndex: 0];
                [self connectPeripheral: self.peripheral];
                [self stopScanning];
                break;
            }
        }
    }
}

//testing
- (void)alertRemote {
    NSLog(@"[BLE][BleManager]ALERT REMOTE\n");
    if ([connectedService count] != 0) {
        MTKBleProximityService *service = [connectedService firstObject];
        [service alertRemote];
    }
}

- (void)stopRemoteAlert {
    NSLog(@"[BLE][BleManager]stopRemoteAlert\n");
    if ([connectedService count] != 0) {
        MTKBleProximityService *service = [connectedService firstObject];
        [service stopRemoteAlert];
    }
}

//testing end

/* Behave properly when heading into and out of the background */
- (void)enteredBackground {
    NSLog(@"[BLE][BleManager]enteredBackground ++");
    mInBackground = YES;
}

- (void)enteredForeground {
    NSLog(@"[BLE][BleManager]enteredForeground ++");
    mInBackground = NO;
}

@end
