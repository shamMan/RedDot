#import "RDNetwork.h"


@implementation RDNetwork

-(BOOL)ConnectToServer
{
    //CFString
    CFStringRef host    =   CFSTR("update.gps-car.cn");
    const unsigned int port =   21211;
    CFReadStreamRef readStream;
    CFWriteStreamRef    writeStream;
    CFStreamCreatePairWithSocketToHost(NULL,host,port,&readStream,&writeStream);
    return TRUE;
}

@end