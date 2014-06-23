/**
 @file RDNetwork.h
 @author Shaw
 @version 1.0
 @date 2014/06/07
 @brief 红点电子狗TCP通讯
 */
#import <CoreFoundation/CFStream.h>
#import <CFNetwork/CFSocketStream.h>
#import <CoreLocation/CLLocation.h>

// Define Block
typedef void (^AskUpdateRequestBlock) (BOOL bNeedUpdate,int pkgSize,int totalSize);

@interface RDNetwork : NSObject
@property (readonly) BOOL connect;
@property (copy,nonatomic) AskUpdateRequestBlock askUpdateBlock;

+(RDNetwork*)ShareInstance;

-(BOOL)connectToServer;
-(BOOL)close;
// 升级查询
-(BOOL)checkUpdateWithHardType:(unsigned int)haderType andSoftVersion:(unsigned int)softVersion andLocation:(CLLocation*)location andBlock:(AskUpdateRequestBlock) block;
@end
