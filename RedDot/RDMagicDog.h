//
//  RDMagicDog.h
//  RedDot
//
//  Created by ShawLiao on 14-6-17.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreLocation/CLLocation.h>
#import <CoreLocation/CLHeading.h>

// Define Block
typedef void (^DogGeneralCommandBlock) (BOOL bSuccess);

typedef NS_ENUM(NSUInteger,MagicDogKeyType)
{
    MagicDogKeyTypeUp   =   0,
    MagicDogKeyTypeDown,
    MagicDogKeyTypeMode,
    MagicDogKeyTypeMute
};

typedef NS_ENUM(NSUInteger, DogStatus) {
    DogStatusWaitPoweredOn,
    DogStatusConnected,
    DogStatusDisconnected,
    DogStatusStatusError
};

typedef NS_ENUM(NSUInteger, DogCommand) {
    DogCommandUnknow,
    DogCommandAskMode      //  握手
};

@class RDMagicDog;

@protocol DogDelegate <NSObject>
@optional
// Status
- (void)dog:(RDMagicDog*)dog statusChanged:(DogStatus) status;
// Location
- (void)dog:(RDMagicDog*)dog didUpdateHeading:(CLHeading *)newHeading;
- (void)dog:(RDMagicDog*)dog didUpdateLocation:(CLLocation *)newLocation;
@end

@interface RDMagicDog : NSObject
@property (assign,nonatomic) id<DogDelegate>    delegate;
@property (retain,nonatomic) CBCentralManager*  centralManager;
@property (retain,nonatomic) CBPeripheral*      peripheral;
@property (assign,nonatomic) DogStatus          status;
@property (readonly, nonatomic) DogCommand      lastCommand;
// 型号,Connect 后可用
@property (readonly,nonatomic) NSString*    modename;

// TEST
@property (retain,nonatomic) CBCharacteristic*  writeChar;

+(RDMagicDog*)linkDogWithDelegate:(id<DogDelegate>)delegate;
// 获取版本
-(BOOL)askModeWithCompleteBlock:(DogGeneralCommandBlock) block;
// 升级
-(BOOL)updateMap;
// 定位
-(BOOL)openNMEAEN;
// 定位
-(BOOL)closeNMEAEN;
// 路况
-(BOOL)startScanRoadInfo;
// 按键
-(BOOL)SendVirtualKey:(MagicDogKeyType)key;
// 投诉
-(BOOL)POI;
// 播放TTS
-(BOOL)playTTS;

@end
