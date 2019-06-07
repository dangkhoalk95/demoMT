//
//  ScannedDeviceTableCell.m
//  BLEManager
//
//  Created by user on 14-7-30.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import "ScannedDeviceTableCell.h"

@implementation ScannedDeviceTableCell

@synthesize device_name;
@synthesize connecting_indicator;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
        
    }
    return self;
}

- (void)awakeFromNib
{
    // Initialization code
    [super awakeFromNib];
    [connecting_indicator stopAnimating];
    connecting_indicator.hidesWhenStopped = true;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

-(void)setDeviceName:(NSString*)name
{
    [device_name setText:name];
}

-(void)showIndicator:(BOOL)showOrHide
{
    if (showOrHide == YES)
    {
        [connecting_indicator startAnimating];
    }
    else if (showOrHide == NO)
    {
        [connecting_indicator stopAnimating];
    }
}

@end
