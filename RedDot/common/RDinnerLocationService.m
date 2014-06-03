//
//  RDMainVC.h
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RDConfig.h"
#import "RDInnerLocationService.h"

@implementation RDInnerLocationService
-(id)init
{
    self = [super init];
    if (self) {
        return self;
    }
    return nil;
}

-(void)dealloc
{
    [self stopLocationService];
    [super dealloc];
}
-(BOOL)startLocationService
{
    if(![CLLocationManager locationServicesEnabled])
    {
        [[Config shareInstance] PLOG:@"用户没有对定位授权!"];
        return FALSE;
    }
    if (!_locationManager) {
        // 获取位置信息
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.distanceFilter     = kCLDistanceFilterNone;
        _locationManager.desiredAccuracy    = kCLLocationAccuracyBest;
        _locationManager.delegate = self;
    }
    if (self.delegate) {
        [self.delegate serviceWillStartLocating:self];
    }
    [_locationManager startUpdatingLocation];
    [_locationManager startUpdatingHeading];
    if (self.delegate) {
        [self.delegate serviceDidStopLocating:self];
    }
    return TRUE;
}
-(BOOL)stopLocationService
{
    if (_locationManager) {
        _locationManager.delegate   =   nil;
        [_locationManager stopUpdatingHeading];
        [_locationManager stopUpdatingLocation];
        [_locationManager release];
        _locationManager    =   nil;
    }
    return TRUE;
}
#pragma mark - Delegate
- (void)locationManager:(CLLocationManager *)manager
didUpdateToLocation:(CLLocation *)newLocation
fromLocation:(CLLocation *)oldLocation
{
    self.curLocation    =   newLocation;
    if (self.delegate) {
        [self.delegate service:self didUpdateLocation:newLocation];
    }
}
- (void)locationManager:(CLLocationManager *)manager
didUpdateHeading:(CLHeading *)newHeading
{
    self.curHeading =   newHeading;
    if (self.delegate) {
        [self.delegate service:self didUpdateHeading:newHeading];
    }
}
- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager
{
    return TRUE;
}
- (void)locationManager:(CLLocationManager *)manager
       didFailWithError:(NSError *)error
{
    if (self.delegate) {
        [self.delegate service:self didFailToLocateUserWithError:error];
    }
}
- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status
{
    [[Config shareInstance] PLOG:@"%s %u",__func__,status];
    if (status == kCLAuthorizationStatusAuthorized)
    {
        [self startLocationService];
    }
}
- (void)locationManager:(CLLocationManager *)manager
didStartMonitoringForRegion:(CLRegion *)region
{
    
}
- (void)locationManagerDidPauseLocationUpdates:(CLLocationManager *)manager
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)locationManagerDidResumeLocationUpdates:(CLLocationManager *)manager
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)locationManager:(CLLocationManager *)manager
didFinishDeferredUpdatesWithError:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s",__func__];
    if (self.delegate) {
        [self.delegate service:self didFailToLocateUserWithError:error];
    }
}
@end
