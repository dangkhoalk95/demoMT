//
//  ScanTopTableViewController.m
//  BLEManager
//
//  Created by changjiang on 14-7-17.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import "ScanTableViewController.h"
#import "MTKBleManager.h"
#import "CachedBLEDevice.h"
#import "BackgroundManager.h"
#import "ScannedDeviceTableCell.h"
#import "ScanTopTableCell.h"
#import "MTKDeviceParameterRecorder.h"

@interface ScanTableViewController () <StateChangeDelegate, BleDiscoveryDelegate>

@property (weak, nonatomic) MTKBleManager* mManager;
@property (weak, nonatomic) CBPeripheral* tempPeripheral;

@property (weak, nonatomic) ScannedDeviceTableCell* connectingCell;

@property (nonatomic) UIActivityIndicatorView* mScanningIndicator;

@property (nonatomic) BOOL mIsConnectingOneDevice;
@property (nonatomic) NSMutableArray* foundedDevices;

@end

@implementation ScanTableViewController

@synthesize mManager;
@synthesize tempPeripheral;

@synthesize mIsConnectingOneDevice;

@synthesize connectingCell;

@synthesize mScanningIndicator;
@synthesize foundedDevices;


- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    foundedDevices = [[NSMutableArray alloc] init];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    [[BackgroundManager sharedInstance] stopScan];
    
    NSMutableArray* array = [MTKDeviceParameterRecorder getDeviceParameters];
    NSLog(@"[ScanTableViewController] [viewDidLoad] array count : %lu", (unsigned long)array.count);
    
    if (array.count != 0)
    {
        DeviceInfo* para = [array objectAtIndex:0];
        CachedBLEDevice* device = [CachedBLEDevice defaultInstance];
        device.mDeviceIdentifier = para.device_identifier;
        device.mDeviceName = para.device_name;

        UINavigationController* controller = [self.storyboard instantiateViewControllerWithIdentifier:@"MainViewController"];
        [self presentViewController:controller animated:NO completion:nil];
    }
    else
    {
        NSLog(@"[ScanTableViewController] [viewDidLoad] array count is 0");
    }
}

-(void)viewWillAppear:(BOOL)animated
{
    mManager = [MTKBleManager sharedInstance];
    [mManager registerDiscoveryDelgegate:self];
    mIsConnectingOneDevice = NO;
    [foundedDevices removeAllObjects];
    [self.tableView reloadData];
}

-(void)viewDidAppear:(BOOL)animated
{
    [[BackgroundManager sharedInstance] registerStateChangeDelegate:self];

    [self updateTopScanView];
}

-(void)viewDidDisappear:(BOOL)animated
{
    [[BackgroundManager sharedInstance] unRegisterStateChangeDelegate:self];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

/********************************************************************************************/
/* table cell click listener, if the scan state is off, do start scan action                */
/* if the scan state is on, do stop scan action                                             */
/********************************************************************************************/
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0)
    {
        if([[BackgroundManager sharedInstance] getScanningState] == SCANNING_STATE_OFF)
        {
            NSLog(@"[ScanTableViewController] [didSelectRowAtIndexPath] start scan action");
            [foundedDevices removeAllObjects];
            [[BackgroundManager sharedInstance] startScan:YES];
        }
        else
        {
            NSLog(@"[ScanTableViewController] [didSelectRowAtIndexPath] stop scan action");
            [[BackgroundManager sharedInstance] stopScan];
        }
    }
    else if (indexPath.section == 1)
    {
        if (mIsConnectingOneDevice == NO)
        {
            NSLog(@"[ScanTableViewController] selected row is : %ld", (long)[indexPath row]);
            // should do connect action, after connect succeed, show main view controller
            
            NSLog(@"[ScanTableViewController] [didSelectRowAtIndexPath] call to connect action");

            CBPeripheral* pp = [foundedDevices objectAtIndex:indexPath.row];
            tempPeripheral = pp;
            NSLog(@"[ScanTableViewController] [didSelectRowAtIndexPath] click devicename : %@ ", [pp name]);
            
            connectingCell = (ScannedDeviceTableCell*)[tableView cellForRowAtIndexPath:indexPath];
            [connectingCell showIndicator:true];
            
            [[BackgroundManager sharedInstance] connectDevice:pp];
        }
        else if (mIsConnectingOneDevice == YES)
        {
            NSLog(@"[ScanTableViewController] [didSelectRowAtIndexPath] current connecting devices");
        }
    }
    
    [tableView deselectRowAtIndexPath:indexPath animated:NO];
}

/************************************************************************/
/* While the tableview reloadData called, the callback will be called   */
/* and add the cell to the tableview, which will list the device name   */
/* in the cell                                                          */
/************************************************************************/
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{

    if (indexPath.section == 0)
    {
        NSString* identifier = @"ScanTopCell";
        static BOOL nibRegistered = NO;
        
        if (!nibRegistered)
        {
            UINib* nib = [UINib nibWithNibName: @"ScanTopTableCell" bundle:nil];
            [tableView registerNib:nib forCellReuseIdentifier:identifier];
            nibRegistered = YES;
        }
        
        ScanTopTableCell* cell = [tableView dequeueReusableCellWithIdentifier:identifier];
        
        [cell updateScanState:[[BackgroundManager sharedInstance] getScanningState]];
        return cell;
    }
    else if (indexPath.section == 1)
    {
        NSString* identifier = @"ScannedDeviceTableCell";
        static BOOL nibRegistered = NO;
        
        if (!nibRegistered)
        {
            UINib* nib = [UINib nibWithNibName: @"ScannedDeviceTableCell" bundle:nil];
            [tableView registerNib:nib forCellReuseIdentifier:identifier];
            nibRegistered = YES;
        }
        
        ScannedDeviceTableCell* cell = [tableView dequeueReusableCellWithIdentifier:identifier];
        
//        if ([mManager.foundPeripherals count] != 0)
        if ([foundedDevices count] != 0)
        {
            CBPeripheral* perpheral = [foundedDevices objectAtIndex:indexPath.row];
            NSString* name = [perpheral name];
            NSString* deviceTitle = nil;
            if (name == nil || [name length] == 0)
            {
                deviceTitle = [[perpheral identifier] UUIDString];
            }
            else
            {
                deviceTitle = name;
            }

            if (tempPeripheral != nil && [[[tempPeripheral identifier] UUIDString] isEqualToString:[[perpheral identifier] UUIDString]])
            {
                NSLog(@"[ScanTableViewController] [cellForRowAtIndexPath] same CBPeripheral kepp the indicator");
                [cell showIndicator:YES];
            }
            [cell setDeviceName:deviceTitle];
        }
        return cell;
    }
    
    return nil;
}

-(UIView*)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    if (section == 1)
    {
        UIView* view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, 40)];
        UILabel* label = [[UILabel alloc] initWithFrame:CGRectMake(12, 2.5, 100, 40)];
        label.text = NSLocalizedString(@"DEVICES", @"DEVICES");
        label.textColor = [UIColor grayColor];
        label.font = [UIFont fontWithName:@"System" size:10];
        [view addSubview:label];
        
        mScanningIndicator = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(80, 2.5, 30, 40)];
        mScanningIndicator.activityIndicatorViewStyle = UIActivityIndicatorViewStyleGray;
        mScanningIndicator.hidesWhenStopped = true;
        if ([[BackgroundManager sharedInstance] getScanningState] == SCANNING_STATE_OFF)
        {
            [mScanningIndicator stopAnimating];
        }
        else if ([[BackgroundManager sharedInstance] getScanningState] == SCANNING_STATE_ON)
        {
            [mScanningIndicator startAnimating];
        }
        [view addSubview:mScanningIndicator];
        return view;
    }
    return nil;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0)
    {
        return 100;
    }
    return 44;
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if (section == 1)
    {
        return 40;
    }
    return 20;
}

-(void)updateTopScanView
{
    if ([[BackgroundManager sharedInstance] getScanningState] == SCANNING_STATE_ON)
    {
        [mScanningIndicator startAnimating];
    }
    else
    {
        [mScanningIndicator stopAnimating];
    }
}

-(void)onAdapterStateChange:(int)state
{
    
}

-(void)onConnectionStateChange:(CBPeripheral*)peripheral connectionState:(int)state
{
    tempPeripheral = nil;
    if (state == CONNECTION_STATE_CONNECTED)
    {
        [self hideConnectionIndicator];
        NSLog(@"[ScanTableViewController] [onConnectionStateChange] connection state : CONNECTION_STATE_CONNECTED");
        
        mIsConnectingOneDevice = NO;
        
        CachedBLEDevice* device = [CachedBLEDevice defaultInstance];
        
        device.mDeviceName = [peripheral name];
        device.mDeviceIdentifier = [[peripheral identifier] UUIDString];
        device.mConnectionState = CONNECTION_STATE_CONNECTED;
        [device setDevicePeripheral:peripheral];
        
        [device persistData:1];
        
        [[BackgroundManager sharedInstance] stopScan];
        
        UINavigationController* controller = [self.storyboard instantiateViewControllerWithIdentifier:@"MainViewController"];
        [self presentViewController:controller animated:YES completion:nil];
        
        [[BackgroundManager sharedInstance] stopConnectTimer];
    }
    else if (state == CONNECTION_STATE_DISCONNECTED)
    {
        NSLog(@"[ScanTableViewController] [onConnectionStateChange] connection state : CONNECTION_STATE_DISCONNECTED");
        [self hideConnectionIndicator];
        mIsConnectingOneDevice = NO;
        [[BackgroundManager sharedInstance] stopConnectTimer];
    }

}

-(void)onScanningStateChange:(int)state
{
    if (state == SCANNING_STATE_OFF)
    {
        [self updateTopScanView];
    }
    else if (state == SCANNING_STATE_ON)
    {
        [self updateTopScanView];
    }
    [self.tableView reloadData];
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (section == 0)
    {
        return 1;
    }
    if (section == 1)
    {
        return foundedDevices.count;//[[mManager foundPeripherals] count];
    }
    return 0;
}

-(void)hideConnectionIndicator
{
    if (connectingCell != nil)
    {
        [connectingCell showIndicator:NO];
    }
}

- (void) discoveryDidRefresh: (CBPeripheral *)peripheral
{
    NSLog(@"[ScanTableViewController] [discoveryDidRefresh] enter");
    if (foundedDevices != nil)
    {
        if (![foundedDevices containsObject:peripheral])
        {
            [foundedDevices addObject:peripheral];
        }
    }
    [self.tableView reloadData];
}

- (void) discoveryStatePoweredOff
{
    NSLog(@"[ScanTableViewController] [discoveryStatePoweredOff] enter");
    [self.tableView reloadData];
}


@end
