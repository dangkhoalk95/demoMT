//
//  MTKDevicePrarmeterRecorder.h
//  BleProfile
//
//  Created by ken on 14-7-9.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DeviceParameter.h"
#import "DeviceInfo.h"

@interface MTKDeviceParameterRecorder : NSObject

+(void) deleteDevice:(NSString*) deviceIdentifier;

+(BOOL) setParameters:(int)which
           deviceName:(NSString*)name
     deviceIdentifier:(NSString*)identifier;

+(DeviceParameter *) getParameters: (NSString *)deviceIdentifier;

+(NSMutableArray*) getDeviceParameters;

+ (NSString *) getSavedDeviceIdentifier;

@end
