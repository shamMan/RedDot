#import "RDNetwork.h"
#import "RDConfig.h"
#import "dogDefine.h"


#define NONE_REU_TYPE 0
static const int c_pkg_head_len =   6;

@interface RDNetwork()<NSStreamDelegate>
@property (retain,nonatomic) NSInputStream* iStream;
@property (retain,nonatomic) NSOutputStream* oStream;
// 获取当前日期，返回 20120221
-(unsigned int)now;
@end

@implementation RDNetwork
+(RDNetwork*)ShareInstance
{
    static RDNetwork *sharedNetworkInstance = nil;
    
    static dispatch_once_t network_predicate;
    dispatch_once(&network_predicate, ^{
        sharedNetworkInstance = [[self alloc] init];
    });
    return sharedNetworkInstance;
}
-(id)init
{
    self    =   [super init];
    _connect    =   FALSE;
    
    _curReqType =   NONE_REU_TYPE;
    _bufferLen  =   1400;
    _readBuffer     =   (unsigned char*)malloc(_bufferLen);
    _writeBuffer    =   (unsigned char*)malloc(_bufferLen);
    
    return self;
}
-(unsigned int)now
{
    unsigned int ret    =   0;
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
    NSDate *now =   [NSDate date];
    NSInteger unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit |NSWeekdayCalendarUnit | NSHourCalendarUnit | NSMinuteCalendarUnit | NSSecondCalendarUnit;
    NSDateComponents *comps = [calendar components:unitFlags fromDate:now];
    NSInteger year    = [comps year];
    NSInteger month   = [comps month];
    NSInteger day     = [comps day];
    // 20140221
    ret =   year * 10000 + month * 100 + day;
    return ret;
}
-(BOOL)close
{
    if (_connect) {
        self.iStream    =   nil;
        self.oStream    =   nil;
        _connect    =   FALSE;
    }
    return TRUE;
}
-(BOOL)connectToServer
{
    //CFString
    NSString* DAS_HOST    =   [[Config shareInstance] GetServerAddress];
    unsigned short DAS_PORT =   [[Config shareInstance] GetServerPort];
    CFStringRef host    =   (CFStringRef)DAS_HOST;
    const unsigned int port =   DAS_PORT;
    CFReadStreamRef readStream;
    CFWriteStreamRef    writeStream;
    CFStreamCreatePairWithSocketToHost(NULL,host,port,&readStream,&writeStream);
    do {
        if (readStream && writeStream)
        {
            if(!CFReadStreamOpen(readStream))
            {
                [[Config shareInstance] PLOG:@"CFReadStreamOpen Failed!"];
                break;
            }
            if(!CFWriteStreamOpen(writeStream))
            {
                [[Config shareInstance] PLOG:@"CFWriteStreamOpen Failed!"];
                break;
            }
            CFReadStreamSetProperty(readStream,kCFStreamPropertyShouldCloseNativeSocket,kCFBooleanTrue);
            CFWriteStreamSetProperty(writeStream,kCFStreamPropertyShouldCloseNativeSocket,kCFBooleanTrue);
            
            _iStream =(NSInputStream *)readStream;
            [_iStream retain];
            [_iStream setDelegate:self];
            [_iStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
            
            _oStream = (NSOutputStream *)writeStream;
            [_oStream retain];
            [_oStream setDelegate:self];
            [_oStream scheduleInRunLoop:[NSRunLoop currentRunLoop]forMode:NSDefaultRunLoopMode];
        }
        return TRUE;
    } while (0);
    if (readStream) {
        CFReadStreamClose(readStream);
        CFRelease(readStream);
    }
    if (writeStream) {
        CFWriteStreamClose(writeStream);
        CFRelease(writeStream);
    }
    [[Config shareInstance] PLOG:@"CFStreamCreatePairWithSocketToHost Failed!"];
    return FALSE;
}
-(void)dealloc
{
    [_iStream release];
    [_oStream release];
    if (_readBuffer) {
        free(_readBuffer);
    }
    if (_writeBuffer) {
        free(_writeBuffer);
    }
    [super dealloc];
}
#pragma mark -- 公开请求
-(BOOL)checkUpdateWithHardType:(NSString*)hardwareType andLocation:(CLLocation*)location andBlock:(AskUpdateRequestBlock) block
{
    if (![self.oStream hasSpaceAvailable]) {
        [[Config shareInstance] PLOG:@"checkUpdateWithHardType Failed,because of ostream not ready"];
        return FALSE;
    }
    self.askUpdateBlock =   block;
    // 序列号，固件本版，硬件版本 等未启用参数
    const char* _serNo  =   "000000000000";
    INT32   _SWVer  =   0;
    INT32   _HWVer  =   0;
    int _HWTypeLen  =   hardwareType.length;
    if (_HWTypeLen > 12) {
        _HWTypeLen  =   12;
    }
    const char*  _HWType =   [hardwareType UTF8String];
    // 位置
    UINT16  _RMCLatitude =   location.coordinate.latitude * 100;
    UINT16  _RMCLongitude=   location.coordinate.longitude * 100;
    
    STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)_writeBuffer;
	STU_UPDATE_CHECK_REQ* req = (STU_UPDATE_CHECK_REQ*)(_writeBuffer + sizeof(STU_PACKAGE_HEADER));
	memset(req, 0, sizeof(STU_UPDATE_CHECK_REQ));
	memcpy(req->SerNo,_serNo,12);
	memcpy(req->HWType,_HWType,_HWTypeLen);
	req->FWVer      =   _SWVer;					//固件版本
	req->HWVer      =   _HWVer;					//硬件版本
	req->DataTime   =   [self now];             //最新数据日期
	req->nStartLatitude =   0;					//当前最新数据的起始点纬度，单位为度；
	req->nStartLongitude=   0;					//当前最新数据的起始点经度，单位为度；
	req->nEndLatitude   =   0;					//当前最新数据的结束点纬度，单位为度；
	req->nEndLongitude  =   0;					//当前最新数据的结束点经度，单位为度；
	req->nDataOption    =   0;					//数据选项，待定义 0 部分升级 ，1 全部升级；
	req->nPtLatitude    =   _RMCLatitude;       //当前所在位置的纬度，单位为度；
	req->nPtLongitude   =   _RMCLongitude;		//当前所在位置的经度，单位为度；
	req->nBufferSize    =   0;					//内部数据包缓冲区大小
	
	header->nReq        = REQ_UPDATE_CHECK_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_UPDATE_CHECK_REQ));
    
	int len = sizeof(STU_PACKAGE_HEADER) + sizeof(STU_UPDATE_CHECK_REQ);
    NSInteger writed  =   [self.oStream write:_writeBuffer maxLength:len];
    if (writed != len)
    {
        NSLog(@"ostream write failed,need:%d writed:%lu",len,writed);
        return FALSE;
    }
    self.curReqType =   REQ_UPDATE_CHECK_REQ;
    return TRUE;
//
//	if (m_pSocket->writen(m_pSendBuffer, len) != len)
//		return FALSE;
//	len = m_pSocket->read(m_pSendBuffer, 1400);
//	if (len < 6 || CheckPkgHead(m_pSendBuffer, len) != REQ_UPDATE_CHECK_ACK)
//	{
//		return FALSE;
//	}
//	STU_UPDATE_CHECK_ACK ack;
//	memcpy((UINT8*)&ack,m_pSendBuffer+sizeof(STU_PACKAGE_HEADER),sizeof(ack));
//	if (ack.nResult == 0)
//		return FALSE;
//    
//	m_dwUpdateBytes = ack.dwBytes;
	
	return TRUE;
}

#pragma mark -- NSStreamDelegate
- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode
{
    if (aStream == _iStream) {
        // InputStream
        
    }
    else if(aStream == _oStream)
    {
        // OutputStream
        
    }
    switch (eventCode) {
        case NSStreamEventNone:
            break;
        case NSStreamEventOpenCompleted:
            [[Config shareInstance] PLOG:@"NSStreamEventOpenCompleted !"];
            break;
        case NSStreamEventHasBytesAvailable:
        {
            // read
            NSInteger readed    =   [self.iStream read:_readBuffer maxLength:_bufferLen];
            switch (self.curReqType) {
                case REQ_UPDATE_CHECK_REQ:
                {
                    NSString* errorStr  =   nil;
                    do {
                        if(readed < c_pkg_head_len)
                        {
                            errorStr    =   @"received len:%d < pakage head len!";
                            break;
                        }
                        if (CheckPkgHead(_readBuffer, readed) != REQ_UPDATE_ACK) {
                            errorStr    =   @"received pasre error!";
                            break;
                        }
                        STU_UPDATE_CHECK_ACK* req = (STU_UPDATE_CHECK_ACK*)(_readBuffer+sizeof(STU_PACKAGE_HEADER));
                        self.askUpdateBlock(TRUE,req->nResult,req->nResult,req->dwBytes);
                    } while (0);
                    if (errorStr) {
                        [[Config shareInstance] PLOG:@"%@",errorStr];
                        self.askUpdateBlock(FALSE,FALSE,0,0);
                    }
                }
                    break;
                    
                default:
                    [[Config shareInstance] PLOG:@"received data len:%d,but how to parse?",readed];
                    break;
            }
            self.curReqType =   NONE_REU_TYPE;
            
        }
            break;
        case NSStreamEventHasSpaceAvailable:
            break;
        case NSStreamEventErrorOccurred:
            break;
        case NSStreamEventEndEncountered:
            break;
        default:
            break;
    }
}
@end