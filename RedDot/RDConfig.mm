//
//  Config.m
//  Dadahan
//
//  Created by 廖 文伟 on 12-5-19.
//  Copyright (c) 2012年 nil. All rights reserved.
//

#include "RDConfig.h"

#define kAllowUpdateOn3G        @"kAllowUpdateOn3G"
#define kAutoConnect            @"kAutoConnect"
#define kAutoUpdate             @"kAutoUpdate"
#define KCanReportInBackground  @"kCanReportInBackground"

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
- (void)saveUserDefaultValue:(BOOL)value forKey:(NSString*)key
{
    NSUserDefaults* userDefault =   [NSUserDefaults standardUserDefaults];
    [userDefault setBool:value forKey:key];
    [userDefault synchronize];
}
- (BOOL)getUserDefaultForKey:(NSString*)key default:(BOOL)dValue
{
    NSUserDefaults* userDefault =   [NSUserDefaults standardUserDefaults];
    BOOL    ret;
    if (nil == [userDefault objectForKey:key])
    {
        ret = dValue;
    }
    else
    {
        ret   =   [userDefault boolForKey:key];
    }
    return ret;
}
- (BOOL)allowUpdateOn3G
{
    return [self getUserDefaultForKey:kAllowUpdateOn3G default:YES];
}
- (void)setAllowUpdateOn3G:(BOOL)allowUpdateOn3G
{
    [self saveUserDefaultValue:allowUpdateOn3G forKey:kAllowUpdateOn3G];
}
- (BOOL)autoConnect
{
    return [self getUserDefaultForKey:kAutoConnect default:YES];
}
- (void)setAutoConnect:(BOOL)autoConnect
{
    [self saveUserDefaultValue:autoConnect forKey:kAutoConnect];
}
- (BOOL)autoUpdate
{
    return [self getUserDefaultForKey:kAutoUpdate default:YES];
}
- (void)setAutoUpdate:(BOOL)autoUpdate
{
    [self saveUserDefaultValue:autoUpdate forKey:kAutoUpdate];
}
- (BOOL)canReportInBackground
{
    return [self getUserDefaultForKey:KCanReportInBackground default:YES];
}
- (void)setCanReportInBackground:(BOOL)canReportInBackground
{
    [self saveUserDefaultValue:canReportInBackground forKey:KCanReportInBackground];
}
- (void)resetSettingData
{
    [self saveUserDefaultValue:YES forKey:kAllowUpdateOn3G];
    [self saveUserDefaultValue:YES forKey:kAutoConnect];
    [self saveUserDefaultValue:YES forKey:kAutoUpdate];
    [self saveUserDefaultValue:YES forKey:KCanReportInBackground];
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
