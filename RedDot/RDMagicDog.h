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
    MagicDogKeyTypeMute,
    MagicDogKeyTypeRoad
};

typedef NS_ENUM(NSUInteger, DogStatus) {
    DogStatusWaitPoweredOn,
    DogStatusConnected,
    DogStatusDisconnected,
    DogStatusStatusError
};

typedef NS_ENUM(NSUInteger, DogCommand) {
    DogCommandUnknow,
    DogCommandHandShake,        // 握手
    DogCommandAdkUpdate,        // 升级信息
    DogCommandNMEA,             // 输出定位信息
    DogCommandPoi,              // 投诉
    DogCommandRoadInfo          // 路况
};

// 定位输出结构体
typedef struct{
	volatile UINT8  ASK_GPS_HEAD[8];			//包头 GPS_HEAD
    volatile UINT32 Bule_Latitude;				//纬度	X10000
	volatile UINT32 Bule_Longitude;				//经度	X10000
	volatile UINT16 Bule_GroundSpeed;			//相对位移速度 KM/S
	volatile UINT16 Bule_Course;					//相对位移方向
	volatile UINT8 Bule_Year;					//年 北京时间
	volatile UINT8 Bule_Month;					//月
	volatile UINT8 Bule_Day;					//日
	volatile UINT8 Bule_Hour;					//时
	volatile UINT8 Bule_Minute;					//分
	volatile UINT8 Bule_Second;					//秒
	volatile UINT8 RESERVED[2];					//预留
}STU_BLUETOOTH_GPS;

#define GPS_BUFFER_LEN  56

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
{
    int _gpsDataLen;
    unsigned char _gpsDataBuf[GPS_BUFFER_LEN];
}
@property (assign,nonatomic) id<DogDelegate>    delegate;
@property (retain,nonatomic) CBCentralManager*  centralManager;
@property (retain,nonatomic) CBPeripheral*      peripheral;
@property (assign,nonatomic) DogStatus          status;
@property (readonly, nonatomic) DogCommand      lastCommand;
// 型号,Connect 后可用
@property (readonly,nonatomic) NSString*        modename;
// 升级详细信息
@property (readonly,nonatomic) STU_ASK_UPDATE   stu_ask_update;

+(RDMagicDog*)linkDogWithDelegate:(id<DogDelegate>)delegate;
// 握手
-(BOOL)shakeHand:(DogGeneralCommandBlock) block;
// 获取升级详细
-(BOOL)askUpdateDetail:(DogGeneralCommandBlock) block;
// 升级
-(BOOL)updateMap;
// 开启定位
-(BOOL)openNMEAEN;
// 关闭定位
-(BOOL)closeNMEAEN;
// 路况
-(BOOL)roadInfo:(DogGeneralCommandBlock) block;
// 按键（遥控器）
-(BOOL)SendVirtualKey:(MagicDogKeyType)key;
// 投诉
-(BOOL)POI:(DogGeneralCommandBlock) block;
// 播放TTS
-(BOOL)playTTS;

@end
