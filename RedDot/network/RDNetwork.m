#import "RDNetwork.h"
#import "RDConfig.h"
#import "dogDefine.h"


#define NONE_REU_TYPE 0
static const int c_pkg_head_len =   6;

@interface RDNetwork()<NSStreamDelegate>
@property (retain,nonatomic) NSInputStream* iStream;
@property (retain,nonatomic) NSOutputStream* oStream;
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
    self.seqNo      =   @"000000000000";
    return self;
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
    [_seqNo release];
    if (_readBuffer) {
        free(_readBuffer);
    }
    if (_writeBuffer) {
        free(_writeBuffer);
    }
    
    [super dealloc];
}
#pragma mark -- 公开请求
-(BOOL)checkUpdateWithHardType:(NSString*)hardwareType andDate:(unsigned int)date andLocation:(CLLocation*)location andBlock:(AskUpdateRequestBlock) block
{
    if (![self.oStream hasSpaceAvailable]) {
        [[Config shareInstance] PLOG:@"checkUpdateWithHardType Failed,because of ostream not ready"];
        return FALSE;
    }
    self.askUpdateBlock =   block;
    // 序列号，固件本版，硬件版本 等未启用参数
    const char* serNo  =   [self.seqNo UTF8String];
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
	memcpy(req->SerNo,serNo,12);
	memcpy(req->HWType,_HWType,_HWTypeLen);
	req->FWVer      =   _SWVer;					//固件版本
	req->HWVer      =   _HWVer;					//硬件版本
	req->DataTime   =   date;                   //最新数据日期
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
        NSLog(@"ostream write failed,need:%d writed:%d",len,writed);
        return FALSE;
    }
    self.curReqType =   REQ_UPDATE_CHECK_REQ;
    return TRUE;
}

-(BOOL)fetchUpdateByPakageIndex:(int)index andIsFinall:(BOOL)bEnd andBlock:(FetchUpdateRequestBlock) block
{
    if (![self.oStream hasSpaceAvailable]) {
        [[Config shareInstance] PLOG:@"checkUpdateWithHardType Failed,because of ostream not ready"];
        return FALSE;
    }
    self.fetchUpdateBlock =   block;
    STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)_writeBuffer;
	STU_UPDATE_REQ* req     =   (STU_UPDATE_REQ*)(_writeBuffer + sizeof(STU_PACKAGE_HEADER));
	memset(req, 0, sizeof(STU_UPDATE_REQ));
	req->nPktIndex      = index;
	req->nPackSize      = 1024;
	req->nLastPacket    = bEnd;
	header->nReq        = REQ_UPDATE_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_UPDATE_REQ));
	int len = sizeof(STU_PACKAGE_HEADER) + sizeof(STU_UPDATE_REQ);
    NSInteger writed  =   [self.oStream write:_writeBuffer maxLength:len];
    if (writed != len)
    {
        NSLog(@"ostream write failed,need:%d writed:%d",len,writed);
        return FALSE;
    }
    self.curReqType     =   REQ_UPDATE_REQ;
    return TRUE;
}
-(BOOL)reportPoiAddOrDelete:(BOOL)bIsAdd atLocation:(CLLocation*)location andDirect:(int)dir andBlock:(ReportPoiRequestBlock) block
{
    if (![self.oStream hasSpaceAvailable]) {
        [[Config shareInstance] PLOG:@"checkUpdateWithHardType Failed,because of ostream not ready"];
        return FALSE;
    }
    self.reportPoiBlock =   block;
    STU_PACKAGE_HEADER* header  = (STU_PACKAGE_HEADER*)_writeBuffer;
	STU_POI_UPLOAD_REQ* req     = (STU_POI_UPLOAD_REQ*)(_writeBuffer + sizeof(STU_POI_UPDATE_REQ));
	memset(req, 0, sizeof(STU_UPDATE_REQ));
    // 位置
    UINT16  _RMCLatitude =   location.coordinate.latitude * 100;
    UINT16  _RMCLongitude=   location.coordinate.longitude * 100;
	req->nPtLatitude    = _RMCLatitude;
	req->nPtLongitude   = _RMCLongitude;
	req->angle          = dir;
	req->voiceID        = 0;
    req->flag           = bIsAdd;
	header->nReq        = REQ_POI_UPLOAD_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_POI_UPLOAD_REQ));
	int len = sizeof(STU_PACKAGE_HEADER) + sizeof(STU_POI_UPLOAD_REQ);
    NSInteger writed  =   [self.oStream write:_writeBuffer maxLength:len];
    if (writed != len)
    {
        NSLog(@"ostream write failed,need:%d writed:%d",len,writed);
        return FALSE;
    }
    self.curReqType     =   REQ_POI_UPLOAD_REQ;
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
            //NSString* errorStr  =   nil;
            NSInteger readed    =   [self.iStream read:_readBuffer maxLength:_bufferLen];
            NSAssert(readed > c_pkg_head_len, @"iStream received pakage len < head len!");
            int readedType  =   CheckPkgHead(_readBuffer, readed);
            NSAssert(_curReqType == readedType, @"iStream received type not match!");
            unsigned char* pContent =   _readBuffer + sizeof(STU_PACKAGE_HEADER);
            switch (self.curReqType)
            {
                case REQ_UPDATE_CHECK_REQ:
                {
                    STU_UPDATE_CHECK_ACK* req = (STU_UPDATE_CHECK_ACK*)pContent;
                    self.askUpdateBlock(TRUE,req->nResult,req->nResult,req->dwBytes);
                    break;
                }
                case REQ_UPDATE_REQ:
                {
                    STU_UPDATE_REQ* req = (STU_UPDATE_REQ*)pContent;
                    unsigned char* realData =   pContent + sizeof(STU_UPDATE_REQ);
                    self.fetchUpdateBlock(TRUE,req->nPktIndex,realData,1024);
                    break;
                }
                case REQ_POI_UPLOAD_REQ:
                {
                    STU_POI_UPLOAD_CHK_ACK* req = (STU_POI_UPLOAD_CHK_ACK*)pContent;
                    BOOL success =  (req->result == 0)?TRUE:FALSE;
                    self.reportPoiBlock(success);
                    break;
                }
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