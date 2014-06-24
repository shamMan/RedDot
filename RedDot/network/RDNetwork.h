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
typedef void (^AskUpdateRequestBlock) (BOOL bSuccess,BOOL bNeedUpdate,int pkgSize,int totalSize);

@interface RDNetwork : NSObject
{
    @private
    int  _bufferLen;
    unsigned char* _readBuffer;
    unsigned char* _writeBuffer;
    
}
@property (readonly) BOOL connect;
@property (copy,nonatomic) AskUpdateRequestBlock askUpdateBlock;
// 当前请求类型
@property (assign)  int curReqType;

+(RDNetwork*)ShareInstance;

-(BOOL)connectToServer;
-(BOOL)close;
// 升级查询
-(BOOL)checkUpdateWithHardType:(NSString*)hardwareType andLocation:(CLLocation*)location andBlock:(AskUpdateRequestBlock) block;
@end
