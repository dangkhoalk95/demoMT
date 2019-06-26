//
//  ScanTopTableCell.m
//  Mediatek SmartDevice
//
//  Created by user on 14-8-26.
//  Copyright (c) 2014年 Mediatek. All rights reserved.
//

#import "ScanTopTableCell.h"
#import "MTKBleManager.h"

@implementation ScanTopTableCell

static NSString* ADD_DEVICE_STRING = @"Tap to add device";
static NSString* STOP_SCAN_DEVICE_STRING = @"Searching devices...\nTap to stop searching";



@synthesize mScanImage;
@synthesize mScanLabel;

- (void)awakeFromNib
{
    // Initialization code
    [super awakeFromNib];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

-(void)updateScanState:(int)scanState
{
    UIImage* addImage = [UIImage imageNamed:@"ic_add_device"];
    UIImage* scanImage = [UIImage imageNamed:@"ic_search_device"];
    if (scanState == SCANNING_STATE_OFF)
    {
        [mScanImage setImage:addImage];
        [mScanLabel setText: NSLocalizedString(@"Tap to add device", @"Tap to add device")];
    }
    else if (scanState == SCANNING_STATE_ON)
    {
        [mScanImage setImage:scanImage];
        [mScanLabel setText: NSLocalizedString(@"Searching devices", @"Searching devices")];
    }
}

@end
