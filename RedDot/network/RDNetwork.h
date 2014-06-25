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
typedef void (^FetchUpdateRequestBlock) (BOOL bSuccess,int index,unsigned char* data,int datalen);
typedef void (^ReportPoiRequestBlock) (BOOL bSuccess);

@interface RDNetwork : NSObject
{
    @private
    int  _bufferLen;
    unsigned char* _readBuffer;
    unsigned char* _writeBuffer;
    
}
// 序列号
@property (retain,nonatomic) NSString*      seqNo;
@property (readonly) BOOL connect;
@property (copy,nonatomic) AskUpdateRequestBlock    askUpdateBlock;
@property (copy,nonatomic) FetchUpdateRequestBlock  fetchUpdateBlock;
@property (copy,nonatomic) ReportPoiRequestBlock    reportPoiBlock;
// 当前请求类型
@property (assign)  int curReqType;

+(RDNetwork*)ShareInstance;

-(BOOL)connectToServer;
-(BOOL)close;
// 升级查询
-(BOOL)checkUpdateWithHardType:(NSString*)hardwareType andDate:(unsigned int)date andLocation:(CLLocation*)location andBlock:(AskUpdateRequestBlock) block;
// 升级包下载
-(BOOL)fetchUpdateByPakageIndex:(int)index andIsFinall:(BOOL)bEnd andBlock:(FetchUpdateRequestBlock) block;
// 投诉点上传
-(BOOL)reportPoiAddOrDelete:(BOOL)bIsAdd atLocation:(CLLocation*)location andDirect:(int)dir andBlock:(ReportPoiRequestBlock) block;
@end
