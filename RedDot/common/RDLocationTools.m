//
//  RDBaseLocationService
//  LocationComponent
//
//  Created by Shaw on 3/28/14.
//  Copyright (c) 2014 . All rights reserved.
//

#import "RDLocationTools.h"

@implementation RDLocationTools
+(CLLocationCoordinate2D)BMKCoordinnateFromCLLocationCoord:(CLLocationCoordinate2D)location
{
    NSDictionary* dic   =   BMKConvertBaiduCoorFrom(location, BMK_COORDTYPE_GPS);
    if (dic) {
        return BMKCoorDictionaryDecode(dic);
    }
    return CLLocationCoordinate2DMake(0, 0);
}
+(CLLocation*)CLLocationApplyBaiDuTransform:(CLLocation*)location
{
    CLLocationCoordinate2D  cood    =   [RDLocationTools BMKCoordinnateFromCLLocationCoord:location.coordinate];
    CLLocation* ret =   [[[CLLocation alloc] initWithCoordinate:cood altitude:location.altitude horizontalAccuracy:location.horizontalAccuracy verticalAccuracy:location.verticalAccuracy course:location.course speed:location.speed timestamp:location.timestamp] autorelease];
    NSLog(@"Conver: [%0.4f,%0.4f] ==> [%0.4f,%0.4f]",location.coordinate.latitude,location.coordinate.longitude,cood.latitude,cood.longitude);
    return ret;
}
@end