//
//  BMKUserLocation+modifyProperty.m
//  RedDot
//
//  Created by air on 14-6-3.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "BMKUserLocationEx.h"

@implementation BMKUserLocationEx
-(id)init
{
    self    =   [super init];
    if (self) {
        private_location    =   nil;
        private_heading     =   nil;
        return self;
    }
    return nil;
}
-(void)SetCLLocation:(CLLocation*)newLocation
{
    if (private_location != newLocation)
    {
        [private_location release];
        if (newLocation) {
            private_location    =   [newLocation retain];
        }
        else
        {
            private_location    =   nil;
        }
    }
}
-(void)SetCLHeading:(CLHeading*)newHeading
{
    if (private_heading != newHeading)
    {
        [private_heading release];
        if (private_heading) {
            private_heading    =   [private_heading retain];
        }
        else
        {
            private_heading    =   nil;
        }
    }
}
-(CLHeading*)heading
{
    //NSLog(@"%s",__func__);
    return private_heading;
}
-(CLLocation*)location
{
    //NSLog(@"%s",__func__);
    return private_location;
}
-(void)dealloc
{
    [private_heading release];
    [private_location release];
    [super dealloc];
}
@end
