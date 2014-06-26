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
@property (copy,nonatomic) DogGeneralCommandBlock commandBlock;
-(BOOL)foundAndConnect;
-(BOOL)cleanUp;
-(BOOL)writeStr:(NSString*)str;
-(NSString*)now;
@end

@implementation RDMagicDog

-(NSString*)now
{
    NSString* date;
    NSDateFormatter* formatter = [[[NSDateFormatter alloc]init] autorelease];
    [formatter setDateFormat:@"YYYYMMddhhmmss"];
    date = [formatter stringFromDate:[NSDate date]];
    return date;
}

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
        memset(&_stu_ask_update, 0, sizeof(STU_ASK_UPDATE));
		_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
	}
    return self;
}
-(void)dealloc
{
    [_centralManager release];
    [_peripheral release];
    [_modename release];
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
-(BOOL)writeStr:(NSString*)str
{
    NSLog(@"send str:%@", str);
    NSData* aData = [str dataUsingEncoding: NSASCIIStringEncoding];
    [self.peripheral writeValue:aData forCharacteristic:self.writeChar type:CBCharacteristicWriteWithoutResponse];
    return TRUE;
}
-(BOOL)shakeHand:(DogGeneralCommandBlock) block;
{
    [self writeStr:@"Ego"];
    self.commandBlock   =   block;
    _lastCommand    =   DogCommandHandShake;
    return TRUE;
}
// 获取升级详细
-(BOOL)askUpdateDetail:(DogGeneralCommandBlock) block
{
    //NSString* command   =   [NSString stringWithFormat:@"UPDATAREG-%@",[self now]];
    NSString* command   =   @"UPDATAREG-20140113162212";
    [self writeStr:command];
    self.commandBlock   =   block;
    _lastCommand    =   DogCommandAdkUpdate;
    return TRUE;
}
// 升级
-(BOOL)updateMap
{
    return TRUE;
}
// 定位
-(BOOL)openNMEAEN
{
    [self writeStr:@"NMEAEN"];
    _lastCommand    =   DogCommandNMEA;
    return TRUE;
}
// 定位
-(BOOL)closeNMEAEN
{
    [self writeStr:@"NMEADIS"];
    return TRUE;
}
// 路况
-(BOOL)roadInfo:(DogGeneralCommandBlock) block
{
    [self writeStr:@"ROADKEY"];
    self.commandBlock   =   block;
    _lastCommand    =   DogCommandRoadInfo;
    return TRUE;
}
// 按键
-(BOOL)SendVirtualKey:(MagicDogKeyType)key
{
    NSString* str   =   nil;
    switch (key) {
        case MagicDogKeyTypeMode:
            //str =   @"MODEKEY";
            str     =   @"UPDATAREG-20140113162212";
            break;
        case MagicDogKeyTypeUp:
            str =   @"UPKEY";
            break;
        case MagicDogKeyTypeDown:
            str =   @"DOWNKEY";
            break;
        case MagicDogKeyTypeMute:
            str =   @"MUTEKEY";
            break;
        default:
            break;
    }
    [self writeStr:str];
    return TRUE;
}
// 投诉
-(BOOL)POI:(DogGeneralCommandBlock) block;
{
    [self writeStr:@"POIKEY"];
    self.commandBlock   =   block;
    _lastCommand    =   DogCommandPoi;
    return TRUE;
}
// 播放TTS
-(BOOL)playTTS
{
    return TRUE;
}

-(void)setStatus:(DogStatus)status
{
    if (_status != status) {
        _status =   status;
    }
    if (_delegate) {
        [_delegate dog:self statusChanged:status];
    }
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
    if (central.state == CBCentralManagerStatePoweredOff) {
        self.status =   DogStatusWaitPoweredOn;
    }
    if (central.state == CBCentralManagerStatePoweredOn) {
        [self foundAndConnect];
    }
}

- (void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary *)dict
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,dict];
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    [[Config shareInstance] PLOG:@"%s %@ %@ %@",__func__,peripheral,advertisementData,RSSI];
    [_centralManager stopScan];
    [_centralManager connectPeripheral:peripheral options:nil];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    self.peripheral =   peripheral;
    self.peripheral.delegate    =   self;
    [[Config shareInstance] PLOG:@"%s %@",__func__,peripheral];
    //self.status =   DogStatusConnected;
    [self.peripheral discoverServices:nil];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
    self.status =   DogStatusDisconnected;
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s %@",__func__,error];
    self.status =   DogStatusDisconnected;
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
		[[Config shareInstance] PLOG:@"Service found: %@ isPrimary:%d \t{",service,service.isPrimary];
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
    [[Config shareInstance] PLOG:@"%s service:%@ error:%@",__func__,service,error];
    for(CBCharacteristic*characteristic in service.characteristics)
    {
        [[Config shareInstance] PLOG:@"charac:%@ ,0x%x",characteristic,characteristic.properties];
        switch (characteristic.properties) {
            case CBCharacteristicPropertyRead:
                [_peripheral readValueForCharacteristic:characteristic];
                break;
            case CBCharacteristicPropertyWriteWithoutResponse:
                self.writeChar  =   characteristic;
                self.status =   DogStatusConnected;
                break;
            case CBCharacteristicPropertyNotify:
                [_peripheral setNotifyValue:YES forCharacteristic:characteristic];
                break;
            default:
                break;
        }
        //[_peripheral readValueForCharacteristic:characteristic];
        //[_peripheral setNotifyValue:YES forCharacteristic:characteristic];
    }
}
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    NSString *value = [[[NSString alloc] initWithData:characteristic.value encoding:NSUTF8StringEncoding] autorelease];
    NSLog(@"Value %@",value);
    [[Config shareInstance] PLOG:@"%s %@ char:%@ value:%@",__func__,error,characteristic,value];
    switch (self.lastCommand) {
        case DogCommandHandShake:
        {
            // SF4201-CONNECT
            if ([value hasSuffix:@"-CONNECT\r\n"]) {
                int index   =   [value length] - 10;
                _modename   =   [[value substringToIndex:index] retain];
                self.commandBlock(TRUE);
            }
            else
            {
                int len     =   characteristic.value.length;
                const void* data =   characteristic.value.bytes;
                if (!data || len < sizeof(STU_ASK_UPDATE)) {
                    [[Config shareInstance] PLOG:@"查询蓝牙信息返回长度不够:%d",len];
                    self.commandBlock(FALSE);
                }
                else
                {
                    STU_ASK_UPDATE* req =   (STU_ASK_UPDATE*)data;
                    _stu_ask_update =   *req;
                    self.commandBlock(TRUE);
                }
            }
        }
            break;
        case DogCommandAdkUpdate:
        {
            
            break;
        }
        case DogCommandNMEA:
        {
            break;
        }
        case DogCommandPoi:
        case DogCommandRoadInfo:
        {
            if ([value compare:@"STU_BLUETOOTH_GPS"] == 0)
                self.commandBlock(TRUE);
            else
                self.commandBlock(TRUE);
            break;
        }
        default:
            break;
    }
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

- (NSString*)debugDescription
{
    NSString* ret   =   [NSString stringWithFormat:@"DUMP RDMagicDog:\n Mode=%@ \n 基础数据日期:%d \n 硬件ID:%d \n 序列号:%s \n 固件版本:%d \n 软件版本:%s \n ",_modename,_stu_ask_update.DATEDATA,_stu_ask_update.HARDWAREID,_stu_ask_update.SERNO,_stu_ask_update.APPVERSION,_stu_ask_update.CAMERADARA_VER];
    return ret;
}

@end
