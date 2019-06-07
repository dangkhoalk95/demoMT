//
//  DeviceInfo.h
//  BLEManager
//
//  Created by changjiang on 14-7-21.
//  Copyright (c) 2014年 com.mediatek. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface DeviceInfo : NSManagedObject

@property (nonatomic, retain) NSNumber * alert_enabler;
@property (nonatomic, retain) NSString * device_name;
@property (nonatomic, retain) NSNumber * ringtone_enabler;
@property (nonatomic, retain) NSNumber * range_alert_enabler;
@property (nonatomic, retain) NSNumber * range_type;
@property (nonatomic, retain) NSNumber * range_value;
@property (nonatomic, retain) NSNumber * disconnect_alert_enabler;
@property (nonatomic, retain) NSNumber * vibration_enabler;
@property (nonatomic, retain) NSString * device_identifier;

@end
