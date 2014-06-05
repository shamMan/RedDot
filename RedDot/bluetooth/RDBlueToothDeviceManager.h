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
    BLDeviceManagerStatusPowerOff,
    BLDeviceManagerStatusPowerOn,
    BLDeviceManagerStatusResetting,
    BLDeviceManagerStatusUnauthorized,
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
@property (retain, nonatomic) NSMutableArray    *foundPeripherals;
@property (retain, nonatomic) NSMutableArray	*connectedServices;
@property (readonly,nonatomic) BLDeviceManagerStatus   status;
-(BOOL)StartIntelligentConnect;
-(BOOL)StopConnect;
@end
