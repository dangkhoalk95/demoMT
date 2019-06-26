//
//  MtkAppDelegate.h
//  Mediatek SmartDevice
//
//  Created by user on 14-8-26.
//  Copyright (c) 2014年 Mediatek. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>

@interface MtkAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (readonly, strong, nonatomic) NSManagedObjectContext* managedObjectContext;
@property (readonly, strong, nonatomic) NSManagedObjectModel* managedObjectModel;
@property (readonly, strong, nonatomic) NSPersistentStoreCoordinator* persistentStoreCoordinator;

-(void)saveContext;
-(NSURL*) applicationDocumentDirectory;

@end
