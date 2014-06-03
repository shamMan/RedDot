//
//  RDBaseLocationService
//  LocationComponent
//
//  Created by Shaw on 3/28/14.
//  Copyright (c) 2014 . All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CLLocation.h>
#import "BMKGeometry.h"

@interface RDLocationTools : NSObject
+(CLLocationCoordinate2D)BMKCoordinnateFromCLLocationCoord:(CLLocationCoordinate2D)location;
+(CLLocation*)CLLocationApplyBaiDuTransform:(CLLocation*)location;
@end