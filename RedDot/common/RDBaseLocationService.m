
#import <UIKit/UIKit.h>
#import "RDConfig.h"
#import "RDBaseLocationService.h"

@implementation RDBaseLocationService
-(BOOL)startLocationService
{
    return FALSE;
}
-(BOOL)stopLocationService
{
    return FALSE;
}
-(void)dealloc
{
    [_curHeading release];
    [_curLocation release];
    [super dealloc];
}
@end