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

#pragma mark - Baidu Map
- (void)onGetNetworkState:(int)iError
{
    [self PLOG:@"BAIDU-MAP::onGetNetworkState:%d",iError];
}
- (void)onGetPermissionState:(int)iError
{
    [self PLOG:@"BAIDU-MAP::onGetPermissionState:%d",iError];
}

- (BOOL)initBaiduMap
{
    if (_mapManager) {
        return TRUE;
    }
    _mapManager = [[BMKMapManager alloc]init];
    // 如果要关注网络及授权验证事件，请设定generalDelegate参数
    BOOL ret = [_mapManager start:kBaiduMapKey generalDelegate:self];
    if (!ret)
    {
        [self PLOG:@"initBaiduMap Failed!"];
    }
    return ret;
}

- (NSString*)GetServerAddress
{
    NSUserDefaults* userDefault =   [NSUserDefaults standardUserDefaults];
    NSString* ret   =   [userDefault stringForKey:@"server-address"];
    if (ret == nil)
    {
       ret = @"srv1.vurbleapp.com";
    }
    return ret;
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
    [_mapManager release];
    [super dealloc];
}



@end
