//
//  ScanTopTableCell.h
//  Mediatek SmartDevice
//
//  Created by user on 14-8-26.
//  Copyright (c) 2014年 Mediatek. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ScanTopTableCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UIImageView *mScanImage;
@property (weak, nonatomic) IBOutlet UILabel *mScanLabel;

-(void)updateScanState:(int)scanState;

@end
