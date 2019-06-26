//
//  ScannedDeviceTableCell.h
//  BLEManager
//
//  Created by user on 14-7-30.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ScannedDeviceTableCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UILabel *device_name;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *connecting_indicator;

-(void)setDeviceName:(NSString*)name;
-(void)showIndicator:(BOOL)showOrHide;

@end
