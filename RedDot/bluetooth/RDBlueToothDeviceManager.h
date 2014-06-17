//
//  RDBlueToothDeviceManager.h
//  RedDot
//
//  Created by ShawLiao on 14-6-4.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@class RDBlueToothDeviceManager;

typedef NS_ENUM(NSUInteger, BLDeviceManagerStatus) {
    BLDeviceManagerStateUnknown,
    BLDeviceManagerStatusResetting,
    BLDeviceManagerStateUnsupported,
    BLDeviceManagerStatusUnauthorized,
    BLDeviceManagerStatusPowerOff,
    BLDeviceManagerStatusPowerOn,
    BLDeviceManagerStatusInit,
    BLDeviceManagerStatusScanning,
    BLDeviceManagerStatusDiscovered,
    BLDeviceManagerStatusConnected,
    BLDeviceManagerStatusDisconnected,
    BLDeviceManagerStatusError
};

@protocol RDBuleToothDeviceDelegate <NSObject>
@optional
-(void)Manager:(RDBlueToothDeviceManager*) manager statusChanged:(BLDeviceManagerStatus) status;
@end

@interface RDBlueToothDeviceManager : NSObject
@property (assign,nonatomic) id<RDBuleToothDeviceDelegate>  delegate;
@property (readonly,nonatomic) BLDeviceManagerStatus   status;
+(RDBlueToothDeviceManager*)shareInstance;
-(BOOL)StartIntelligentConnect;
-(BOOL)StopConnect;
@end
