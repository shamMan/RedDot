//
//  RDBaseLocationService
//  LocationComponent
//
//  Created by Shaw on 3/28/14.
//  Copyright (c) 2014 . All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CLLocation.h>
#import <CoreLocation/CLHeading.h>

/// 定位服务Delegate,调用startUserLocationService定位成功后，用此Delegate来获取定位数据
@protocol RDLocationServiceDelegate <NSObject>
@optional
- (void)serviceWillStartLocating:(id)service;
- (void)serviceDidStopLocating:(id)service;
- (void)service:(id)service didUpdateHeading:(CLHeading *)newHeading;
- (void)service:(id)service didUpdateLocation:(CLLocation *)newLocation;
- (void)service:(id)service didFailToLocateUserWithError:(NSError *)error;
@end

@interface RDBaseLocationService : NSObject
@property (nonatomic, assign) id<RDLocationServiceDelegate> delegate;
@property (retain, nonatomic) CLHeading*  curHeading;
@property (retain, nonatomic) CLLocation* curLocation;
-(BOOL)startLocationService;
-(BOOL)stopLocationService;
@end