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
#import "dogDefine.h"

// Define Block
typedef void (^DogGeneralCommandBlock) (BOOL bSuccess);

///< 蓝牙详细信息结构体
typedef struct{
	UINT8   ASK_UPDATE_HEAD[8];		//包头 ASUPDATE
	UINT32  DATEDATA;				//基础数据日期
	UINT16  HARDWAREID;             //硬件ID
    UINT8   SERNO[10];	        	//序列号
	UINT8	APPVERSION;				//固件版本号
	UINT8   CAMERADARA_VER[4];		//软件版本
	UINT8   RESERVED[3];			//预留
}STU_ASK_UPDATE;

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
    DogCommandHandShake,        //  握手
    DogCommandAdkUpdate,        //  升级信息
    DogCommandNMEA              //  输出定位信息
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
@property (readonly,nonatomic) NSString*        modename;
// 升级详细信息
@property (readonly,nonatomic) STU_ASK_UPDATE   stu_ask_update;
// TEST
@property (retain,nonatomic) CBCharacteristic*  writeChar;

+(RDMagicDog*)linkDogWithDelegate:(id<DogDelegate>)delegate;
// 握手
-(BOOL)shakeHand:(DogGeneralCommandBlock) block;
// 获取升级详细
-(BOOL)askUpdateDetail:(DogGeneralCommandBlock) block;
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
