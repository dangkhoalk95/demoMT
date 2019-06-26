//
//  MainTableViewController.m
//  BLEManager
//
//  Created by ken on 14-7-14.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import "MainTableViewController.h"
#import "CachedBLEDevice.h"
#import "MTKDeviceParameterRecorder.h"
#import "BackgroundManager.h"
#import "BtNotify.h"

@interface MainTableViewController () <CachedBLEDeviceDelegate, NotifyCustomDelegate, NotifyFotaDelegate>

@property (weak, nonatomic) IBOutlet UILabel *mDeviceNameLabel;
@property (weak, nonatomic) IBOutlet UILabel *mConnectionStateLabel;
- (IBAction)findAndConnectAction:(UIButton *)sender;
@property (weak, nonatomic) IBOutlet UIButton *mConnectButton;
- (IBAction)fotaTestClickAction:(id)sender;

@property (weak, nonatomic) MTKBleManager* mManager;
@property (weak, nonatomic) CachedBLEDevice *mDevice;
@property (weak, nonatomic) IBOutlet UIButton *compressBtn;
@property (weak, nonatomic) IBOutlet UIButton *notCompressBtn;
@property (weak, nonatomic) IBOutlet UILabel *statusLabel;

@end

@implementation MainTableViewController

@synthesize mDevice;
@synthesize mManager;

@synthesize mDeviceNameLabel;
@synthesize mConnectionStateLabel;

@synthesize mConnectButton;

int mType;
int sendWhich = 0;

int mtotalcount = 0;
int msentcount = 0;


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
    mManager = [MTKBleManager sharedInstance];
    mDevice = [CachedBLEDevice defaultInstance];
    
    [[NSNotificationCenter defaultCenter] addObserver: self selector:@selector(didEnterBackgroundNotification:) name:kEnterBackgroundNotification object: nil];
    [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(didEnterForegroundNotification:) name:kEnterForegroundNotification object: nil];
}

-(void)viewDidAppear:(BOOL)animated
{
    if ([[BtNotify sharedInstance] isReadyToSend] == NO) {
        [_notCompressBtn setEnabled:NO];
        [_compressBtn setEnabled:NO];
    } else {
        [_notCompressBtn setEnabled:YES];
        [_compressBtn setEnabled:YES];
    }
    
    [[BtNotify sharedInstance] registerFotaDelegate:self];
    [[BtNotify sharedInstance] registerCustomDelegate:self];
}

-(void)viewDidDisappear:(BOOL)animated
{
    [mDevice unregisterAttributeChangedListener:self];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

-(void)viewWillAppear:(BOOL)animated
{
    
    [mDevice registerAttributeChangedListener:self];
    [self updateUxState];
    
    self.tableView.scrollEnabled = NO;
}

/********************************************************************************************/
/* update the UX connection state, ringing state, device name state                         */
/********************************************************************************************/
-(void)updateUxState {

    [mDeviceNameLabel setText:mDevice.mDeviceName];
    
    [self updateConnectionStateLabel];
    [self updateFindConnectButtonState];
}


/********************************************************************************************/
/* update find & connect button state                                                       */
/* if the device is connected, find button is enabled & visible, hide the connect button    */
/* if the device is connecting or disconnecting, find button is invisible, connect button   */
/* visible, but un-clickable                                                                */
/* if the device is disconnected, the find button invisible, the connect button is clickable*/
/* and visible                                                                              */
/********************************************************************************************/
-(void)updateFindConnectButtonState
{
    [mConnectButton setEnabled:false];
    if ((mDevice.mConnectionState == CONNECTION_STATE_CONNECTING)
             || (mDevice.mConnectionState == CONNECTION_STATE_DISCONNECTING))
    {
        [mConnectButton setHidden:false];
        [mConnectButton setEnabled:false];
    }
    else if (mDevice.mConnectionState == CONNECTION_STATE_DISCONNECTED)
    {
        [mConnectButton setHidden:false];
        [mConnectButton setEnabled:true];
    }
}

/********************************************************************************************/
/* convert the connection int state to be a string, which will showed in the UX             */
/********************************************************************************************/
-(void)updateConnectionStateLabel
{
    [mConnectionStateLabel setTextColor: [UIColor blackColor]];
    if (mDevice.mConnectionState == CONNECTION_STATE_CONNECTED)
    {
        [mConnectionStateLabel setText:NSLocalizedString(@"Connected", @"Connected")];
    }
    else if (mDevice.mConnectionState == CONNECTION_STATE_CONNECTING)
    {
        [mConnectionStateLabel setText: NSLocalizedString(@"Connecting", @"Connecting")];
    }
    else
    {
        [mConnectionStateLabel setText: NSLocalizedString(@"Disconnected", @"Disconnected")];
        [mConnectionStateLabel setTextColor:[UIColor grayColor]];
    }
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    if (section == 0)
    {
        return 2;
    }
    if (section == 1)
    {
        return 2;
    }
    return 0;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0 && indexPath.row == 0) {
        UIAlertController *controller = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"Forget", @"Forget")
                                                                            message:mDevice.mDeviceName
                                                                     preferredStyle:UIAlertControllerStyleActionSheet];
        UIAlertAction *delAction = [UIAlertAction actionWithTitle:NSLocalizedString(@"Forget", @"Forget")
                                                            style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                                                                [[BackgroundManager sharedInstance] setDisconnectFromUx:YES];
                                                                
                                                                [[BackgroundManager sharedInstance] disconnectDevice:[mDevice getDevicePeripheral]];
                                                                [[MTKBleManager sharedInstance] forgetPeripheral];
                                                                
                                                                [MTKDeviceParameterRecorder deleteDevice:mDevice.mDeviceIdentifier];
                                                                [[BackgroundManager sharedInstance] stopScan];
                                                                
                                                                [self dismissViewControllerAnimated:NO completion:nil];
                                                            }];
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:NSLocalizedString(@"Cancel", @"Cancel") style:UIAlertActionStyleCancel handler:nil];
        
        [controller addAction:delAction];
        [controller addAction:cancelAction];
        
        [self presentViewController:controller animated:YES completion:nil];
    }
    
    
    
    [self.tableView deselectRowAtIndexPath:indexPath animated:NO];
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if (section == 0)
    {
        return 30;
    }
    if (section == 2)
    {
        if (iPhone5 == YES)
        {
            return 30;
        }
        else
        {
            return 30;
        }
    }
    return 20;
}

/*
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:<#@"reuseIdentifier"#> forIndexPath:indexPath];
    
    // Configure the cell...
    
    return cell;
}
*/

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

/********************************************************************************************/
/* if the device attribute has changed, call this method to notify user                     */
/********************************************************************************************/
-(void)onDeviceAttributeChanged:(int)which
{
    if (which == DEVICE_CONNECTION_STATE_CHANGE
        || which == DEVICE_NAME_CHANGE
        || which == DEVICE_PERIPHERAL_CHANGE)
    {
        [self updateUxState];
        if (which == DEVICE_CONNECTION_STATE_CHANGE)
        {
            [[BackgroundManager sharedInstance] stopConnectTimer];
        }
    }
}

/********************************************************************************************/
/* if the device attribute has changed, call this method to notify user                     */
/********************************************************************************************/
- (IBAction)findAndConnectAction:(UIButton *)sender {
    
    if (mDevice.mConnectionState == CONNECTION_STATE_DISCONNECTED)
    {
        NSLog(@"[MainTableViewController] [findAndConnectAction] do connect action");
        BOOL b = [[BackgroundManager sharedInstance] connectDevice:[mDevice getDevicePeripheral]];
        if (b == YES) {
            mDevice.mConnectionState = CONNECTION_STATE_CONNECTING;
            [self updateFindConnectButtonState];
            [self updateConnectionStateLabel];
        }
    }

}

- (void)didEnterBackgroundNotification:(NSNotification*)notification
{
    NSLog(@"Entered background notification called.");
    [mManager enteredBackground];
}

- (void)didEnterForegroundNotification:(NSNotification*)notification
{
    NSLog(@"Entered foreground notification called.");
    
    [mManager enteredForeground];
}

- (IBAction)fotaTestClickAction:(id)sender {
    
    NSBundle* bundle = [NSBundle mainBundle];
    
    NSString* path = [bundle pathForResource:@"image_20170105" ofType:@"bin"];
    NSLog(@"surplus :: Fota Test Click Action File : %@", path);
    NSData* data = [[NSData alloc] initWithContentsOfFile:path];
    NSLog(@"surplus :: fotaTestClickAction :: data count : %lu", (unsigned long)[data length]);
    
    if ([data length] == 0)
    {
        NSLog(@"fotaTestClickAction data length is 0");
        return;
    }
    [[BtNotify sharedInstance] sendFotaData:FBIN_FOTA_UPDATE firmwareData:data];
}

- (IBAction)FotaTestNotCompressedAction:(id)sender {
    
    NSBundle* bundle = [NSBundle mainBundle];
    
    NSString* path = [bundle pathForResource:@"image_20170105_n" ofType:@"bin"];
    NSData* data = [[NSData alloc] initWithContentsOfFile:path];
    NSLog(@"surplus :: FotaTestNotCompressedAction :: data count : %lu", (unsigned long)[data length]);
    
    if ([data length] == 0)
    {
        NSLog(@"fotaTestClickAction data length is 0");
        return;
    }
    [[BtNotify sharedInstance] sendFotaData:FBIN_FOTA_UPDATE firmwareData:data];
}

-(void)onFotaTypeReceived:(int)fotaType {
    NSLog(@"surplus, fotaType : %d", fotaType);
    mType = fotaType;
}


-(void)onFotaVersionReceived:(FotaVersion*)version {
    NSLog(@"surplus, version : %@", version.version);
}

-(void)onFotaStatusReceived:(int)status {
    NSLog(@"surplus, status : %d", status);
    NSString *str = nil;
    switch (status) {
        case FOTA_UPDATE_VIA_BT_UPDATE_FAILED:
            str = @"Update failed";
            break;
        case FOTA_UPDATE_VIA_BT_COMMON_ERROR:
            str = @"Common Error";
            break;
        case FOTA_UPDATE_VIA_BT_TRANSFER_FAILED:
            str = @"Transfer Failed";
            break;
        case FOTA_UPDATE_VIA_BT_DISK_FULL:
            str = @"Deivce Disk Full";
            break;
        case FOTA_UPDATE_VIA_BT_TRIGGER_FAILED:
            str = @"Trigger Failed";
            break;
        case FOTA_UPDATE_VIA_BT_TRIGGER_FAILED_CAUSE_LOW_BATTERY:
            str = @"Trigger Failed Cause Low Battery";
            break;
        case FOTA_UPDATE_VIA_BT_TRANSFER_SUCCESS:
            str = @"Transfer Succeed";
            break;
        case FOTA_UPDATE_VIA_BT_WRITE_FILE_FAILED:
            str = @"Write File Failed";
            break;
        case FOTA_UPDATE_VIA_BT_UPDATE_SUCCESS:
            str = @"Update Succeed";
            break;
        default:
            break;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [_statusLabel setText:str];
    });
}

-(void)onFotaProgress:(float)progress {
    NSLog(@"surplus : progress : %f", progress);

    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *ff = [NSString stringWithFormat:@"%f", progress];
        NSLog(@"ff = %@", ff);
        [_statusLabel setText:ff];
    });
}

-(void)onReadyToSend:(BOOL)ready {
    NSLog(@"[surplus] onReadyToSend Enter +++++++");
    dispatch_async(dispatch_get_main_queue(), ^{
        if (ready == YES) {
            [_compressBtn setEnabled:YES];
            [_notCompressBtn setEnabled:YES];
            [_statusLabel setText:@"Ready to send"];
        } else {
            [_compressBtn setEnabled:NO];
            [_notCompressBtn setEnabled:NO];
        }
    });
}

-(void)onProgress:(NSString *)sender newProgress:(float)progress {
    NSLog(@"onProgress : sender = %@", sender);
}

-(void)onDataArrival:(NSString *)receiver arrivalData:(NSData *)data {
    NSLog(@"data arrival : %@ - %@", receiver, data);

}

@end
