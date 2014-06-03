//
//  Config.h
//  Dadahan
//
//  Created by 廖 文伟 on 12-5-19.
//  Copyright (c) 2012年 nil. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@protocol DebugProctocal <NSObject>
@optional
- (void)AppendLog:(NSString*)log;
@end

// appsotre 发布版
//#define APPSTORE_RELEASE
// 是否使用 CLCoreLocation
#define USE_CORELOCATION

// 经纬度是否有效
#define validateLocation(longitude,latitude)  (longitude >= -180 && longitude <= 180 && latitude >= -90 && latitude <= 90)

// 判断是不是iphone5
#define iPhone5 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(640, 1136), [[UIScreen mainScreen] currentMode].size) : NO)

// 地图相关
#define MYBUNDLE_NAME @"mapapi.bundle"
#define MYBUNDLE_PATH [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: MYBUNDLE_NAME]
#define MYBUNDLE [NSBundle bundleWithPath: MYBUNDLE_PATH]

#define UNUSED_PARAM(__param__)     __param__ __attribute__((unused))

//static NSString* kBaiduMapKey   =   @"XG44GGahgs0Xt1rAtvAOxKWm";
static NSString* UNUSED_PARAM(kBaiduMapKey)   =   @"XG44GGahgs0Xt1rAtvAOxKWm";

@interface Config : NSObject
@property (assign,nonatomic) id<DebugProctocal> delegateD;

#pragma mark - Singleton
+ (Config*)shareInstance;
#pragma mark - Unity
//- (BOOL)initBaiduMap;

#pragma mark - Debug Tool
- (void)PLOG:(NSString *)format, ...;
#pragma mark - URL Address
- (NSString*)GetServerAddress;

@end
