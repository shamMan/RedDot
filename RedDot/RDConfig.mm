//
//  Config.m
//  Dadahan
//
//  Created by 廖 文伟 on 12-5-19.
//  Copyright (c) 2012年 nil. All rights reserved.
//

#include "RDConfig.h"


@implementation Config

#pragma mark - Singleton
+ (Config*)shareInstance;
{
    static Config *sharedConfigInstance = nil;
    
    static dispatch_once_t predicate;
    dispatch_once(&predicate, ^{
        sharedConfigInstance = [[self alloc] init];
    });
    
    return sharedConfigInstance;
}



- (NSString*)GetServerAddress
{
    //return @"update.gps-car.cn";
    return @"data.gps-car.cn";
}
- (unsigned short)GetServerPort
{
    return 21211;
}

- (void)PLOG:(NSString *)format, ...
{
    if (self.delegateD) {
        if ([self.delegateD respondsToSelector:@selector(AppendLog:)])
        {
            @synchronized(self)
            {
                va_list args;
                va_start(args, format);
                NSString* log   =   [[[NSString alloc] initWithFormat:format arguments:args] autorelease];
                va_end(args);
                [self.delegateD AppendLog:log];
            }
        }
    }
}

#pragma mark - NSObject
- (void)dealloc 
{
    [super dealloc];
}



@end
