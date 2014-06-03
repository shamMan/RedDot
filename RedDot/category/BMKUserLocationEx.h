//
//  BMKUserLocation+modifyProperty.h
//  RedDot
//
//  Created by air on 14-6-3.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "BMKUserLocation.h"

@interface BMKUserLocationEx :BMKUserLocation
{
    @private
    CLLocation* private_location;
    CLHeading*  private_heading;
}
-(void)SetCLLocation:(CLLocation*)newLocation;
-(void)SetCLHeading:(CLHeading*)newHeading;
@end
