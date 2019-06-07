//
//  MainTableViewController.h
//  BLEManager
//
//  Created by ken on 14-7-14.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MainTableViewController : UITableViewController

#define iPhone5 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(640, 1136), [[UIScreen mainScreen] currentMode].size) : NO)

@end
