//
//  RDBlueToothDeviceManager.m
//  RedDot
//
//  Created by ShawLiao on 14-6-4.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDBlueToothDeviceManager.h"

@interface RDBlueToothDeviceManager()<CBCentralManagerDelegate>
{
    CBCentralManager*   _centralManager;
}
@end

@implementation RDBlueToothDeviceManager
-(id)init
{
    self =  [super init];
    if (self) {
		_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
        
		_foundPeripherals = [[NSMutableArray alloc] init];
		_connectedServices = [[NSMutableArray alloc] init];
	}
    return self;
}
-(void)dealloc
{
    [_centralManager release];
    [_foundPeripherals release];
    [_connectedServices release];
    [super dealloc];
}
@end
