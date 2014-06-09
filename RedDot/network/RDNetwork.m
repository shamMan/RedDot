#import "RDNetwork.h"
#import "RDConfig.h"

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
-(BOOL)connectToServer
{
    //CFString
    CFStringRef host    =   CFSTR(DAS_HOST);
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
    [super dealloc];
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