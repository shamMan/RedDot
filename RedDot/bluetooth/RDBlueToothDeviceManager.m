//
//  RDBlueToothDeviceManager.m
//  RedDot
//
//  Created by ShawLiao on 14-6-4.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDBlueToothDeviceManager.h"
#import "RDConfig.h"

@interface RDBlueToothDeviceManager()<CBCentralManagerDelegate>
{
    CBCentralManager*   _centralManager;
    CBPeripheral*       _peripheral;
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
-(BOOL)StartIntelligentConnect
{
    if (_centralManager) {
        [_centralManager scanForPeripheralsWithServices:nil options:nil];
    }
    return TRUE;
}
-(BOOL)StopConnect
{
    if (_centralManager)
    {
        [_centralManager stopScan];
        if (_peripheral) {
            [_centralManager cancelPeripheralConnection:_peripheral];
            [_peripheral release];
            _peripheral =  nil;
        }
        switch (_status) {
                
            default:
                break;
        }
    }
    return TRUE;
}
#pragma mark - CBCentralManagerDelegate
- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    NSString* strStatus[]   =   {
        @"CBCentralManagerStateUnknown",
        @"CBCentralManagerStateResetting",
        @"CBCentralManagerStateUnsupported",
        @"CBCentralManagerStateUnauthorized",
        @"CBCentralManagerStatePoweredOff",
        @"CBCentralManagerStatePoweredOn"
    };
    [[Config shareInstance] PLOG:@"centralManagerDidUpdateState:%@",__func__,strStatus[central.state]];
}

- (void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary *)dict
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,dict];
}

- (void)centralManager:(CBCentralManager *)central didRetrievePeripherals:(NSArray *)peripherals
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,peripherals];
}

- (void)centralManager:(CBCentralManager *)central didRetrieveConnectedPeripherals:(NSArray *)peripherals
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,peripherals];
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    [[Config shareInstance] PLOG:@"%s %@ %@ %@",__func__,peripheral,advertisementData,RSSI];
    
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,peripheral];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}

@end
