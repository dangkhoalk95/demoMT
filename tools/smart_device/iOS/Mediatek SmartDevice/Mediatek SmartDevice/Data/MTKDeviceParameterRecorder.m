//
//  MTKDevicePrarmeterRecorder.m
//  BleProfile
//
//  Created by ken on 14-7-9.
//  Copyright (c) 2014年 MTK. All rights reserved.
//

#import "MTKDeviceParameterRecorder.h"
#import "MTKAppDelegate.h"

@implementation MTKDeviceParameterRecorder

//@synthesize mManagedObjectContext;

/* Public Interface */
+(void) deleteDevice:(NSString*) deviceIdentifier
{
    if(deviceIdentifier == nil || deviceIdentifier.length == 0)
    {
        NSLog(@"[MTKDeviceParameterRecorder] [deleteDevice] deviceIdentifier is WRONG");
        return;
    }
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    MtkAppDelegate* delegate = (MtkAppDelegate *)[[UIApplication sharedApplication] delegate];
    NSEntityDescription* user = [NSEntityDescription entityForName:@"DeviceInfo" inManagedObjectContext:delegate.managedObjectContext];
    [request setEntity:user];
    NSPredicate* predicate = [NSPredicate predicateWithFormat:@"device_identifier=%@", deviceIdentifier];
    [request setPredicate:predicate];
    NSError* error;
    NSMutableArray* mutableResult = [[delegate.managedObjectContext executeFetchRequest:request error:&error] mutableCopy];
    if (mutableResult == nil)
    {
        NSLog(@"[MTKDevicePrarmeterRecorder] [deleteDevice] mutableResult is nill & error : %@", error);
        return;
    }
    for(DeviceInfo* info in mutableResult)
    {
        [delegate.managedObjectContext deleteObject:info];
    }
    if (![delegate.managedObjectContext save:&error])
    {
        NSLog(@"[MTKDeviceParameterRecorder] [deleteDevice] ERROR : %@, %@", error, [error userInfo]);
        return;
    }
    NSLog(@"[MTKDeviceParameterRecorder] [deleteDevice] delete succeed");
}

/**************************************************************************************/
/* which is 1, insert; is 2, update */
/**************************************************************************************/
+(BOOL) setParameters:(int) which
           deviceName:(NSString*)name
     deviceIdentifier:(NSString*)identifier
{
    if (identifier == nil || [identifier length] == 0)
    {
        NSLog(@"[MTKDeviceParameterRecorder] [setParameters] identifier is WRONG");
        return false;
    }
    MtkAppDelegate* delegate = (MtkAppDelegate*)[[UIApplication sharedApplication] delegate];
    BOOL isSuccess = false;
    if (which == 1)
    {
    
        DeviceInfo* info = (DeviceInfo*) [NSEntityDescription insertNewObjectForEntityForName:@"DeviceInfo" inManagedObjectContext:delegate.managedObjectContext];
        NSError* err;
        [info setDevice_identifier:identifier];
        [info setDevice_name:name];
        isSuccess = [delegate.managedObjectContext save:&err];
        if (!isSuccess)
        {
            NSLog(@"[MTKDeviceParameterRecorder] [setParameters] insert ERROR %@ ", err);
        }
    }
    else if (which == 2)
    {
        NSFetchRequest* request = [[NSFetchRequest alloc] init];
        NSEntityDescription* user = [NSEntityDescription entityForName:@"DeviceInfo" inManagedObjectContext:delegate.managedObjectContext];
        [request setEntity:user];
        NSPredicate* predicate = [NSPredicate predicateWithFormat:@"device_identifier=%@", identifier];
        [request setPredicate:predicate];
        NSError* erro = nil;
        NSMutableArray* mutableFetchResult = [[delegate.managedObjectContext executeFetchRequest:request error:&erro] mutableCopy];
        if (mutableFetchResult == nil)
        {
            NSLog(@"[MTKDeviceParameterRecorder] [setParameters] device is not in DB error : %@", erro);
            return false;
        }
        NSLog(@"[MTKDeviceParameterRecorder] [setParameters] mutable fetch result : %lu", (unsigned long)[mutableFetchResult count]);
        for (DeviceInfo* info in mutableFetchResult)
        {
            [info setDevice_name:name];
        }
        BOOL isSuccess = [delegate.managedObjectContext save:&erro];
        
        if (!isSuccess)
        {
            NSLog(@"[MTKDeviceParameterRecorder] [setParameters] ERROR update failed error : %@", erro);
        }
    }
    return isSuccess;
}

+(DeviceParameter *) getParameters: (NSString *)deviceIdentifier
{
    DeviceParameter* parameter = [[DeviceParameter alloc] init];
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    MtkAppDelegate* delegate = (MtkAppDelegate*)[[UIApplication sharedApplication] delegate];
    NSEntityDescription* user = [NSEntityDescription entityForName:@"DeviceInfo" inManagedObjectContext:delegate.managedObjectContext];
    NSPredicate* predicate = [NSPredicate predicateWithFormat:@"device_identifier=%@", deviceIdentifier];
    [request setEntity:user];
    [request setPredicate:predicate];
    NSError* error;
    NSMutableArray* array = [[delegate.managedObjectContext executeFetchRequest:request error:&error] mutableCopy];
    if (array == nil)
    {
        NSLog(@"[MTKDeviceParameterRecorder] [getParameters] query result is nil, error : %@", error);
        return nil;
    }
    if (array.count != 1)
    {
        NSLog(@"[MTKDeviceParameterRecorder] [getParameters] query count is not equal 1");
        return nil;
    }
    DeviceInfo* info = [array objectAtIndex:0];
    NSLog(@"[MTKDeviceParameterRecorder] [getParameters] %@", info);
    parameter.mDeviceName = info.device_name;
    return parameter;
}

+(NSMutableArray*) getDeviceParameters
{
    NSFetchRequest* request=[[NSFetchRequest alloc] init];
    MtkAppDelegate* delegate = (MtkAppDelegate*)[[UIApplication sharedApplication] delegate];
    NSEntityDescription* user=[NSEntityDescription entityForName:@"DeviceInfo" inManagedObjectContext:delegate.managedObjectContext];
    [request setEntity:user];
    NSError* error=nil;
    NSMutableArray* mutableFetchResult=[[delegate.managedObjectContext executeFetchRequest:request error:&error] mutableCopy];
    if (mutableFetchResult==nil) {
        NSLog(@"Error:%@",error);
        return nil;
    }
    NSLog(@"[MTKDeviceParameterRecorder] [getDeviceParameters] The count of entry: %lu", (unsigned long)[mutableFetchResult count]);
    return mutableFetchResult;
}

+ (NSString *) getSavedDeviceIdentifier {
    NSString *result = nil;
    NSFetchRequest* request = [[NSFetchRequest alloc] init];
    MtkAppDelegate* delegate = (MtkAppDelegate*)[[UIApplication sharedApplication] delegate];
    NSEntityDescription* user = [NSEntityDescription entityForName:@"DeviceInfo" inManagedObjectContext:delegate.managedObjectContext];
    //NSPredicate* predicate = [NSPredicate predicateWithFormat:@"device_identifier=%@", deviceIdentifier];
    [request setEntity:user];
    //[request setPredicate:predicate];
    NSError* error;
    NSMutableArray* array = [[delegate.managedObjectContext executeFetchRequest:request error:&error] mutableCopy];
    if (array == nil)
    {
        NSLog(@"[MTKDeviceParameterRecorder] [getParameters] query result is nil, error : %@", error);
        return nil;
    }
    if (array.count != 1)
    {
        NSLog(@"[MTKDeviceParameterRecorder] [getParameters] query count is not equal 1");
        return nil;
    }
    DeviceInfo* info = [array objectAtIndex:0];
    NSLog(@"[MTKDeviceParameterRecorder] [getParameters] %@", info);
    
    result = info.device_identifier;
    
    return result;
}

@end
