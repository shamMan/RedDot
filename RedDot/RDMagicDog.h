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

typedef NS_ENUM(NSUInteger,MagicDogKeyType)
{
    MagicDogKeyTypeUp   =   0,
    MagicDogKeyTypeDown,
    MagicDogKeyTypeMode,
    MagicDogKeyTypeMute
};

@protocol RDMagicDogLocationDelegate <NSObject>
@optional
- (void)dog:(id)dog didUpdateHeading:(CLHeading *)newHeading;
- (void)dog:(id)dog didUpdateLocation:(CLLocation *)newLocation;
@end


@interface RDMagicDog : NSObject
@property (assign,nonatomic) id<RDMagicDogLocationDelegate> location_delegate;
@property (retain,nonatomic) CBCentralManager*  centralManager;
@property (retain,nonatomic) CBPeripheral*      peripheral;

// 定位
-(BOOL)startLocationWithDelegate:(id)delegate;
// 路况
-(BOOL)startScanRoadInfo;
// 按键
-(BOOL)SendVirtualKey:(MagicDogKeyType)key;
// 投诉
-(BOOL)POI;

@end
