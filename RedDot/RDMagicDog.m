//
//  RDMagicDog.m
//  RedDot
//
//  Created by ShawLiao on 14-6-17.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDMagicDog.h"
#import "RDConfig.h"

@interface RDMagicDog()<CBCentralManagerDelegate,CBPeripheralDelegate>
-(BOOL)foundAndConnect;
-(BOOL)cleanUp;
@end

@implementation RDMagicDog

+(RDMagicDog*)linkDogWithDelegate:(id<DogDelegate>)delegate;
{
    static RDMagicDog *sharedDog = nil;
    static dispatch_once_t predicate_dog;
    dispatch_once(&predicate_dog, ^{
        sharedDog = [[self alloc] init];
    });
    if (sharedDog.status != DogStatusConnected)
    {
        sharedDog.delegate  =   delegate;
        [sharedDog foundAndConnect];
    }
    return sharedDog;
}

-(id)init
{
    self =  [super init];
    if (self) {
		_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
	}
    return self;
}
-(void)dealloc
{
    [_centralManager release];
    [_peripheral release];
    [super dealloc];
}

-(BOOL)foundAndConnect
{
    if (self.status == DogStatusConnected)
        return TRUE;
    if (_centralManager) {
        [_centralManager scanForPeripheralsWithServices:nil options:nil];
    }
    return TRUE;
}
-(BOOL)cleanUp
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
// 升级
-(BOOL)updateMap
{
}
// 定位
-(BOOL)openNMEAEN
{
}
// 定位
-(BOOL)closeNMEAEN
{
}
// 路况
-(BOOL)startScanRoadInfo
{
}
// 按键
-(BOOL)SendVirtualKey:(MagicDogKeyType)key
{
}
// 投诉
-(BOOL)POI
{
}
// 播放TTS
-(BOOL)playTTS
{
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
    [[Config shareInstance] PLOG:@"centralManagerDidUpdateState:%s %@",__func__,strStatus[central.state]];
    [self.delegate Manager:self statusChanged:(BLDeviceManagerStatus)central.state];
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
    [_centralManager stopScan];
    [self.delegate Manager:self statusChanged:BLDeviceManagerStatusDiscovered];
    [_centralManager connectPeripheral:peripheral options:nil];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    self.peripheral =   peripheral;
    self.peripheral.delegate    =   self;
    [[Config shareInstance] PLOG:@"%s %@",__func__,peripheral];
    [self.delegate Manager:self statusChanged:BLDeviceManagerStatusConnected];
    [self.peripheral discoverServices:nil];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
    [self.delegate Manager:self statusChanged:BLDeviceManagerStatusDisconnected];
}
#pragma mark --
- (void)peripheralDidUpdateName:(CBPeripheral *)peripheral
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)peripheralDidInvalidateServices:(CBPeripheral *)peripheral
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)peripheral:(CBPeripheral *)peripheral didModifyServices:(NSArray *)invalidatedServices
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)peripheralDidUpdateRSSI:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
    NSArray		*services	= nil;
    
    if (error != nil) {
        NSLog(@"Error %@\n", error);
		return ;
	}
    
	services = [peripheral services];
	if (!services || ![services count]) {
		return ;
	}
    
	for (CBService *service in services) {
		[[Config shareInstance] PLOG:@"Service found: %@ isPrimary:%d \t{",service.UUID,service.isPrimary];
        [_peripheral discoverCharacteristics:nil forService:service];
        [[Config shareInstance] PLOG:@"\t}"];
    }
}
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverIncludedServicesForService:(CBService *)service error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
    for(CBCharacteristic*characteristic in service.characteristics)
    {
        [[Config shareInstance] PLOG:@"charac:%@",characteristic];
        [_peripheral setNotifyValue:YES forCharacteristic:characteristic];
    }
}
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForDescriptor:(CBDescriptor *)descriptor error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}
- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForDescriptor:(CBDescriptor *)descriptor error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
}

@end
