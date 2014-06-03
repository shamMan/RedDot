//
//  BMKLocationService.h
//  LocationComponent
//
//  Created by Baidu on 3/28/14.
//  Copyright (c) 2014 baidu. All rights reserved.
//

#import <CoreLocation/CLLocationManager.h>
#import <CoreLocation/CLLocationManagerDelegate.h>
#import "RDBaseLocationService.h"
/// 内置GPS定位
@interface RDInnerLocationService : RDBaseLocationService <CLLocationManagerDelegate>
{
@private
    CLLocationManager*  _locationManager;
}
@end